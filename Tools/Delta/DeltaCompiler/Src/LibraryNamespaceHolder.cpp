// LibraryNamespaceHolder.cpp
// Library namespace holder for Delta.
// ScriptFighter Project.
// A. Savidis, August 2009.
//

#include "LibraryNamespaceHolder.h"
#include "DeltaCompErrorDefs.h"
#include "LibraryDefsParser.h"

DeltaLibraryNamespaceHolder::DeltaLibraryNamespaceHolder (void) 
	{ unullify(globalNamespace); }

DeltaLibraryNamespaceHolder::~DeltaLibraryNamespaceHolder() 
	{ udeleteunlessnull(globalNamespace); }

void DeltaLibraryNamespaceHolder::Initialise (void) {
	globalNamespace = DNEWCLASS(DeltaLibraryNamespace, (DELTA_LIBRARYNAMESPACE_GLOBAL));
	INIT_COMPILER_COMPONENT_DIRECTORY(globalNamespace, COMPONENT_DIRECTORY());
	namespaceStack.push_front(globalNamespace);
	openedNamespaces[DELTA_LIBRARYNAMESPACE_GLOBAL] = globalNamespace;
}

void DeltaLibraryNamespaceHolder::CleanUp (void) {
	directlyVisibleFunctionAndConsts.clear();
	openedNamespaces.clear();
	directlyVisibleNamespaces.clear();
	namespaceStack.clear();
	udeleteunlessnull(globalNamespace);
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespaceHolder::EnteringNamespace (const std::string& namespaceName) {

	if (!DeltaLibraryDefsParser::IsIdent(namespaceName))
		DELTACOMP_ERROR_NAMESPACE_ILLEGAL_IDENT(namespaceName);

	// Intentionally fallback here. We make the namespace with the illegal name.
	DeltaLibraryNamespace* ns = DPTR(GetCurrentNamespace())->NewNamespace(namespaceName);
	if (GetCurrentNamespace() == globalNamespace)	// If added in global namespace it is directly visible.
		directlyVisibleNamespaces[namespaceName] = ns;
	namespaceStack.push_front(ns);
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespaceHolder::ExitingNamespace (void) {
	DASSERT(!namespaceStack.empty());
	namespaceStack.pop_front();
	DASSERT(!namespaceStack.empty());	// The global namespace is always in.
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespaceHolder::IssueConst (const std::string& constDef) {
	DeltaSymbol* sym = DPTR(GetCurrentNamespace())->IssueConst(constDef);
	if (sym && GetCurrentNamespace() == globalNamespace)		// If added in global namespace it is directly visible.
		directlyVisibleFunctionAndConsts[DPTR(sym)->GetName()] = sym;		// We add it to test for name conflicts when using namespaces.
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespaceHolder::IssueFunction (const std::string& funcProto) {
	DeltaSymbol* sym = DPTR(GetCurrentNamespace())->IssueFunction(funcProto);
	if (sym && GetCurrentNamespace() == globalNamespace)		// If added in global namespace it is directly visible.
		directlyVisibleFunctionAndConsts[DPTR(sym)->GetName()] = sym;	// We add it to test for name conflicts when using namespaces.
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespaceHolder::IssueType (const std::string& typeDef) 
	{ DPTR(GetCurrentNamespace())->IssueType(typeDef); }

///////////////////////////////////////////////////////////

DeltaLibraryNamespace* DeltaLibraryNamespaceHolder::LookupNamespace (const std::string& name) {
	for (Namespaces::iterator j = openedNamespaces.begin(); j != openedNamespaces.end(); ++j)
		if (DeltaLibraryNamespace* ns = DPTR(j->second)->LookupNamespace(name))
			return ns;
	return (DeltaLibraryNamespace*) 0;
}

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespaceHolder::LookupFunctionOrConst (const std::string& funcName) {

	Symbols::iterator i = directlyVisibleFunctionAndConsts.find(funcName);
	if (i == directlyVisibleFunctionAndConsts.end())
		return NIL_SYMBOL;

	// Check if the function name is ambiguous (i.e. it can be found in at least
	// two open namespaces).

	util_ui32 n = 0;
	for (Namespaces::iterator j = openedNamespaces.begin(); j != openedNamespaces.end(); ++j)
		if (DPTR(j->second)->LookupFunctionOrConst(funcName))
			++n;

	DASSERT(n);
	if (n != 1) {
		DELTACOMP_ERROR_FUNCTION_IN_MULTIPLE_NAMESPACES(funcName);
		return NIL_SYMBOL;
	}
	else
		return i->second;
}

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespaceHolder::LookupFunctionOrConstAtGlobalNamespace (const std::string& name) 
	{ return GetGlobalNamespace()->LookupFunctionOrConst(name); }

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespaceHolder::LookupFunctionOrConstAtGlobalNamespace (
		const NameList&		namespacePath, 
		const std::string&	funcName
	) {
	if (DeltaLibraryNamespace* ns = DPTR(GetGlobalNamespace())->LookupNamespace(namespacePath))
		if (DeltaSymbol* func = ns->LookupFunctionOrConst(funcName))
			return func;
		else {
			DELTACOMP_ERROR_NAMESPACE_FUNCTION_NOT_FOUND(
				std::string(DELTA_LIBRARYNAMESPACE_SEPARATOR) + MakeFullyQualifiedName(namespacePath), 
				funcName
			);
		return NIL_SYMBOL;
	}
	else {
		DELTACOMP_ERROR_USING_NAMESPACE_PATH_INVALID(
			std::string(DELTA_LIBRARYNAMESPACE_SEPARATOR) + MakeFullyQualifiedName(namespacePath)
		);
		return NIL_SYMBOL;
	}
}

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespaceHolder::LookupFunctionOrConst (
		const NameList&		namespacePath, 
		const std::string&	funcName
	) {
	
	DeltaSymbol*	func	= NIL_SYMBOL;
	bool			found	= false;
	bool			nsFound	= false;

	for (Namespaces::iterator i = openedNamespaces.begin(); i != openedNamespaces.end(); ++i)
		if (DeltaLibraryNamespace* ns = DPTR(i->second)->LookupNamespace(namespacePath)) {
			nsFound = true;
			if ((func = ns->LookupFunctionOrConst(funcName))) {
				if (found) {
					DELTACOMP_ERROR_FUNCTION_NAMESPACE_PATH_AMBIGUOUS(
						MakeFullyQualifiedName(namespacePath) + DELTA_LIBRARYNAMESPACE_SEPARATOR, 
						funcName
					);
					return NIL_SYMBOL;
				}
				else
					found = true;
			}
		}

	DASSERT(nsFound || (!found && !func));
	DASSERT(!found || func);

	if (!nsFound)
		DELTACOMP_ERROR_LIBFUNC_INVALID_NAMESPACE_PATH(
			MakeFullyQualifiedName(namespacePath), 
			funcName
		);
	if (!func)
		DELTACOMP_ERROR_NAMESPACE_FUNCTION_NOT_FOUND(
			MakeFullyQualifiedName(namespacePath), 
			funcName
		);
	return func;
}

///////////////////////////////////////////////////////////

const DeltaLibraryUserDefinedType* DeltaLibraryNamespaceHolder::LookupType (const std::string& typeName) const {
	if (const DeltaLibraryUserDefinedType* type = GetCurrentNamespace()->LookupType(typeName))
		return type;
	else {
		DELTACOMP_ERROR_TYPE_NOT_IN_CURRENT_OR_ENCLOSING_NAMESPACE(
			typeName,  
			DPTR(GetCurrentNamespace())->GetName()
		);
		return (DeltaLibraryUserDefinedType*) 0;
	}
}

///////////////////////////////////////////////////////////

const DeltaLibraryUserDefinedType* DeltaLibraryNamespaceHolder::LookupType (
		const NameList&		namespacePath,
		const std::string&	typeName
	)  const {
	
	DASSERT(!namespacePath.empty());

	if (const DeltaLibraryNamespace* ns = DPTR(GetGlobalNamespace())->LookupNamespace(namespacePath)) 
		if (const DeltaLibraryUserDefinedType* type = DPTR(ns)->LookupType(typeName))
			return type;
		else {
			DELTACOMP_ERROR_TYPE_IN_NAMESPACE_PATH_NOT_FOUND(
				MakeFullyQualifiedName(namespacePath) + DELTA_LIBRARYNAMESPACE_SEPARATOR,
				typeName
			);
			return (DeltaLibraryUserDefinedType*) 0; 
		}	
	else {
		DELTACOMP_ERROR_TYPE_INVALID_NAMESPACE_PATH(
			MakeFullyQualifiedName(namespacePath) + DELTA_LIBRARYNAMESPACE_SEPARATOR,
			typeName
		);
		return (DeltaLibraryUserDefinedType*) 0; 
	}
}

///////////////////////////////////////////////////////////

const std::string DeltaLibraryNamespaceHolder::MakeFullyQualifiedName (const NameList& namespacePath) {

	DASSERT(!namespacePath.empty());

	std::string fullName;
	for (NameList::const_iterator i = namespacePath.begin(); i != namespacePath.end(); ++i) {
		if (!fullName.empty() && fullName != DELTA_LIBRARYNAMESPACE_SEPARATOR)
			fullName.append(DELTA_LIBRARYNAMESPACE_SEPARATOR);
		fullName.append(*i);
	}

	return fullName;
}

///////////////////////////////////////////////////////////

bool DeltaLibraryNamespaceHolder::Open (const std::string& name) {

	DeltaLibraryNamespace* found	= (DeltaLibraryNamespace*) 0;
	DeltaLibraryNamespace* inside	= (DeltaLibraryNamespace*) 0;

	for (	Namespaces::iterator i = openedNamespaces.begin();
			i != openedNamespaces.end();
			++i	  )
			if (DeltaLibraryNamespace* ns = DPTR(i->second)->LookupNamespace(name)) {	// Found in a visible namespace.
				if (found)	{									// But was already found earlier in another one.
					DASSERT(inside);
					DELTACOMP_ERROR_USING_NAMESPACE_AMBIGUOUS(
						name, 
						DPTR(inside)->GetFullPathWithSeparator() + name,
						DPTR(i->second)->GetFullPathWithSeparator() + name
					);
					unullify(ns);
				}
				else {
					inside = i->second;
					found = ns;
				}
			}
	if (found)
		return Open(found);
	else {
		if (!inside)
			DELTACOMP_ERROR_NAMESPACE_NOT_FOUND(name);
		return false;
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryNamespaceHolder::Open (const NameList&	namespacePath) {

	if (namespacePath.front() == DELTA_LIBRARYNAMESPACE_SEPARATOR) {	// using ::<ns>

		NameList path(namespacePath);
		path.pop_front();	// Drop leading '::'.

		if (DeltaLibraryNamespace* ns = GetGlobalNamespace()->LookupNamespace(path))
			return Open(ns);

		DELTACOMP_ERROR_USING_NAMESPACE_PATH_INVALID(MakeFullyQualifiedName(namespacePath));
		return false;
	}
	else {
		for (	Namespaces::iterator i = openedNamespaces.begin();
				i != openedNamespaces.end();
				++i	  )
				if (DeltaLibraryNamespace* ns = DPTR(i->second)->LookupNamespace(namespacePath))	// Found in a visible namespace.
					return Open(ns);

		DELTACOMP_ERROR_NAMESPACE_NOT_FOUND(MakeFullyQualifiedName(namespacePath));
		return false;
	}
}

///////////////////////////////////////////////////////////

bool DeltaLibraryNamespaceHolder::Open (DeltaLibraryNamespace* ns) {

	// We check whether opening a namespace creates potential conflicts among its 
	// inner namespaces and other already directly visible namespaces.

	for (	Namespaces::iterator i = directlyVisibleNamespaces.begin();
			i != directlyVisibleNamespaces.end();
			++i	  )
			if (DeltaLibraryNamespace* alreadyVisible = DPTR(ns)->LookupNamespace(i->first))
				if (alreadyVisible != ns)	// There is a different inner namespace with the same name.
					DELTACOMP_WARNING_OPEN_NAMESPACE_INNER_CONFLICT(ns->GetName(), i->first);

	// We check whether opening a namespace creates conflicts among its 
	// revealed functions / consts and other already visible library functions / consts,
	// or user defined items.

	Symbols all(ns->GetFunctionsAndConsts());
	for (Symbols::iterator i = all.begin(); i != all.end(); ++i)
		if (directlyVisibleFunctionAndConsts.find(i->first) != directlyVisibleFunctionAndConsts.end())
			DELTACOMP_WARNING_OPEN_NAMESPACE_LIBSYM_CONFLICT(
				ns->GetName(), 
				i->second->GetTypeDescription(),
				i->first
			);
		else
		if (DeltaSymbol* sym = DELTASYMBOLS.Lookup(i->first))
		DELTACOMP_WARNING_OPEN_NAMESPACE_USERSYM_CONFLICT(
			ns->GetName(),
			i->second->GetTypeDescription(),
			i->first,
			sym->GetTypeDescription(), 
			sym->GetLine()
		);

	// Add the newly visible functions and namespaces.

	for (Symbols::iterator i = all.begin(); i != all.end(); ++i)
		directlyVisibleFunctionAndConsts[i->first] = i->second;

	Namespaces inner(ns->GetInnerNamespaces());
	for (Namespaces::iterator i = inner.begin(); i != inner.end(); ++i) 
		directlyVisibleNamespaces[DPTR(i->second)->GetFullyQualifiedName()] = i->second;

	openedNamespaces[DPTR(ns)->GetFullyQualifiedName()] = ns;
	return true;
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespaceHolder::Install (const LibrarySpecifications& libs) {

	for (LibrarySpecifications::const_iterator l = libs.begin(); l != libs.end(); ++l) {

		util_ui32	nesting = 0;
		util_ui32	tokenNo	= 0;
		
		for (UPTR(const char*) token = *l; *token; ++token, ++tokenNo)
			if (ustrprefix(DELTA_LIBRARYNAMESPACE_COMMENT, *token))			// comment
				continue;
			else
			if (!strcmp(*token, DELTA_LIBRARYNAMESPACE_ENTER)) {			// enter namespace
				++nesting;
				++token;
				++tokenNo;
				if (*token)
					EnteringNamespace(*token);
				else
					DELTACOMP_WARNING_ENTER_AT_EOF();
			}
			else
			if (!strcmp(*token, DELTA_LIBRARYNAMESPACE_EXIT))				// exit namespace
				if (!nesting)
					DELTACOMP_WARNING_INVALID_NAMESPACE_EXIT(tokenNo);
				else 
					{ --nesting; ExitingNamespace(); }
			else
			if (!strcmp(*token, DELTA_LIBRARYNAMESPACE_TYPE)) {				// library type definition
				++token;
				++tokenNo;
				if (*token)
					IssueType(*token);
				else
					DELTACOMP_WARNING_TYPE_AT_EOF();
			}
			else
			if (!strcmp(*token, DELTA_LIBRARYNAMESPACE_CONST)) {			// library const definition
				++token;
				++tokenNo;
				if (*token)
					IssueConst(*token);
				else
					DELTACOMP_WARNING_CONST_AT_EOF();
			}
			else
				IssueFunction(*token);										// library function sig

		if (nesting) {	// Forgot to exit? do it automatically.
			DELTACOMP_WARNING_MISSING_NAMESPACE_EXITS();
			while (nesting--)
				ExitingNamespace();
		}
	}
}

///////////////////////////////////////////////////////////
