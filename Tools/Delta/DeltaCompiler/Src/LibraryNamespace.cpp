// LibraryNamespace.h
// Namespaces for library functionsAndConsts in Delta.
// ScriptFighter Project.
// A. Savidis, August 2009.
//

#include "LibraryNamespace.h"
#include "LibraryNamespaceHolder.h"
#include "LibraryFuncSignatures.h"
#include "LibraryDefsParser.h"
#include "DeltaCompErrorDefs.h"
#include "Symbol.h"
#include "ufunctors.h"
#include <algorithm>

///////////////////////////////////////////////////////////

DeltaLibraryNamespace::DeltaLibraryNamespace (
		const std::string&				_name, 
		const DeltaLibraryNamespace*	_parent
	) : parent(_parent), name(_name) {
	if (parent && parent->parent) 
		fullyQualifiedName = parent->GetFullyQualifiedName() + DELTA_LIBRARYNAMESPACE_SEPARATOR + name;
	else
		fullyQualifiedName = name;

	if (!parent)
		fullPathWithSeparator = DELTA_LIBRARYNAMESPACE_SEPARATOR;
	else
		fullPathWithSeparator = parent->GetFullPathWithSeparator() + name + DELTA_LIBRARYNAMESPACE_SEPARATOR;
}

DeltaLibraryNamespace::~DeltaLibraryNamespace() 
	{ Clear(); }

void DeltaLibraryNamespace::Clear (void) {
	
	std::for_each(
		namespaces.begin(),
		namespaces.end(),
		udestroy_second< std::pair<std::string, DeltaLibraryNamespace*> >()
	);
	namespaces.clear();

	std::for_each(
		types.begin(),
		types.end(),
		udestroy_second< std::pair<std::string, const DeltaLibraryUserDefinedType*> >()
	);
	types.clear();
}

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespace::IssueConst (const std::string& constDef) {

	std::string constName, constVal;
	if (!DeltaLibraryDefsParser::SplitConstDefinition(constDef, &constName, &constVal)) {
		DELTACOMP_ERROR_LIBCONST_ILLEGAL_NAME_OR_DEF(constDef);
		return NIL_SYMBOL;
	}
	
	std::string fullName(
					(parent ? GetFullyQualifiedName() + DELTA_LIBRARYNAMESPACE_SEPARATOR : "") + 
					constName
				);

	if (DeltaSymbol* sym = DELTASYMBOLS.Lookup(fullName)) {
		DASSERT( DPTR(sym)->IsLibraryFunction() || DPTR(sym)->IsLibraryConst());
		DELTACOMP_ERROR_LIBCONST_ALREADY_DEFINED(
			constName,
			DPTR(sym)->IsLibraryFunction() ? "function" : "const"
		);
		return NIL_SYMBOL;
	}
	else {
		std::string str, error;
		double num;
		DeltaLibraryDefsParser::ConstType result = DeltaLibraryDefsParser::ParseConstValue(constVal, &num, &str, &error);
		if (!error.empty()) {
			DASSERT(result == DeltaLibraryDefsParser::ConstError); 
			DELTACOMP_ERROR_LIBCONST_VALUE(constName, error); 
			return NIL_SYMBOL;
		}
		else {
			DASSERT(functionsAndConsts.find(constName) == functionsAndConsts.end());
			sym = DELTASYMBOLS.NewSymbol(fullName, false);

			DPTR(sym)->isLibraryConst = true;
			if (result == DeltaLibraryDefsParser::ConstNumber)
				DPTR(sym)->constExpr  = EXPRFACTORY.Make(num);
			else {
				DASSERT(result == DeltaLibraryDefsParser::ConstString);
				DPTR(sym)->constExpr  = EXPRFACTORY.MakeConst(str);
			}

			return functionsAndConsts[constName] = sym;
		}
	}
}

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespace::IssueFunction (const std::string& funcProto) {

	std::string funcName, funcSigs;
	if (!DeltaLibraryDefsParser::SplitSigDefinition(funcProto, &funcName, &funcSigs)) {
		DELTACOMP_ERROR_LIBFUNC_ILLEGAL_NAME_OR_PROTO(funcProto);
		return NIL_SYMBOL;
	}
	
	DeltaLibraryFuncSignatures* sigs = (DeltaLibraryFuncSignatures*) 0;
	if (!funcSigs.empty()) {
		std::string error;
		if (!(sigs = DeltaLibraryFuncSignatures::New(funcSigs, &error, &DELTANAMESPACES))) {
			DASSERT(!error.empty());
			DELTACOMP_ERROR_LIBFUNC_ILLEGAL_SIG(funcProto, error);
			return NIL_SYMBOL;
		}
	}

	std::string fullName(
					(parent ? GetFullyQualifiedName() + DELTA_LIBRARYNAMESPACE_SEPARATOR : "") + 
					funcName
				);

	if (DeltaSymbol* sym = DELTASYMBOLS.Lookup(fullName)) {

		DASSERT(functionsAndConsts.find(funcName) != functionsAndConsts.end());

		if (!DPTR(sym)->IsLibraryFunction()) {
			DASSERT(DPTR(sym)->IsLibraryConst());
			DELTACOMP_ERROR_LIBFUNC_ALREADY_DEFINED_AS_CONST(funcName);
			return NIL_SYMBOL;
		}
		
		if (sigs) {											// Parsed a signature.
			if (!DPTR(sym)->GetLibraryFuncSignatures())		// Had no sigs before.
				DPTR(sym)->libFuncSigs	= sigs;				// Set this.
			else {											// Had sigs, so normal append
				DPTR(DPTR(sym)->GetLibraryFuncSignatures())->Append(*DPTR(sigs));
				udelete(sigs);
			}
		}
		return sym;
	}
	else {
		DASSERT(functionsAndConsts.find(funcName) == functionsAndConsts.end());
		sym = DELTASYMBOLS.NewSymbol(fullName, false);
		DPTR(sym)->isLibraryFunc = true;
		DPTR(sym)->libFuncSigs	 = sigs;
		return functionsAndConsts[funcName] = sym;
	}
}

///////////////////////////////////////////////////////////

DeltaSymbol* DeltaLibraryNamespace::LookupFunctionOrConst (const std::string& funcName) {
	Symbols::iterator i = functionsAndConsts.find(funcName);
	return i != functionsAndConsts.end() ? i->second : NIL_SYMBOL;
}

///////////////////////////////////////////////////////////

const DeltaLibraryUserDefinedType* DeltaLibraryNamespace::IssueType (const std::string& typeDef) {

	std::string typeName, baseDefs;
	if (!DeltaLibraryDefsParser::SplitTypeDefinition(typeDef, &typeName, &baseDefs)) {
		DELTACOMP_ERROR_ILLEGAL_TYPE_DEFINITION(typeDef);
		return (DeltaLibraryUserDefinedType*) 0;
	}

	if (types.find(typeName) != types.end()) {
		DELTACOMP_ERROR_USER_TYPE_REDEFINED(
			typeName, 
			parent ? GetFullyQualifiedName() : "global namespace"
		);
		return (DeltaLibraryUserDefinedType*) 0;
	}
	
	std::string fullPath = GetFullyQualifiedName() + DELTA_LIBRARYNAMESPACE_SEPARATOR + typeName;
	DeltaLibraryUserDefinedType* type = DNEWCLASS(
											DeltaLibraryUserDefinedType,
											(typeName, fullPath)
										);
	DeltaLibraryDefsParser parser;
	parser.SetNamespaceHolder(&DELTANAMESPACES);
	std::pair<CompilerComponentDirectory*, DeltaLibraryUserDefinedType*> closure(COMPONENT_DIRECTORY(), type);
	parser.SetOnNewBase(&OnNewBase, &closure);

	if (!parser.ParseBaseTypes(baseDefs)) {
		DELTACOMP_ERROR_LIBFUNC_ILLEGAL_BASES_DEFINITION(typeDef, parser.GetError());
		DDELETE(type);
		return (DeltaLibraryUserDefinedType*) 0;
	}
	else
		return types[typeName] = type;	// Always installed at the end.
}

///////////////////////////////////////////////////////////

const DeltaLibraryUserDefinedType* DeltaLibraryNamespace::LookupType (const std::string& typeName) const {
	Types::const_iterator i = types.find(typeName);
	if (i != types.end())
		return i->second;
	else
	if (parent)
		return DPTR(parent)->LookupType(typeName);
	else
		return (DeltaLibraryUserDefinedType*) 0;
}

///////////////////////////////////////////////////////////

DeltaLibraryNamespace* DeltaLibraryNamespace::NewNamespace (const std::string& namespaceName) {
	Namespaces::iterator i = namespaces.find(namespaceName);
	if (i != namespaces.end())
		return i->second;
	else {
		DeltaLibraryNamespace* ns = DNEWCLASS(DeltaLibraryNamespace, (namespaceName, this));
		INIT_COMPILER_COMPONENT_DIRECTORY(ns, COMPONENT_DIRECTORY());
		namespaces[namespaceName] = ns;
		return ns;
	}
}

///////////////////////////////////////////////////////////

DeltaLibraryNamespace* DeltaLibraryNamespace::LookupNamespace (const std::string& namespaceName) {
	Namespaces::iterator i = namespaces.find(namespaceName);
	return i != namespaces.end() ? i->second : (DeltaLibraryNamespace*) 0;
}

const DeltaLibraryNamespace* DeltaLibraryNamespace::LookupNamespace (const std::string& namespaceName) const {
	Namespaces::const_iterator i = namespaces.find(namespaceName);
	return i != namespaces.end() ? i->second : (DeltaLibraryNamespace*) 0;
}

///////////////////////////////////////////////////////////

DeltaLibraryNamespace* DeltaLibraryNamespace::LookupNamespace (const NameList& namespacePath) {
	return	const_cast<DeltaLibraryNamespace*>(
				static_cast<const DeltaLibraryNamespace*>(this)->LookupNamespace(namespacePath)
			); 
}

const DeltaLibraryNamespace* DeltaLibraryNamespace::LookupNamespace (const NameList& namespacePath) const {

	DASSERT(!namespacePath.empty());
	const DeltaLibraryNamespace* ns = this;

	for (NameList::const_iterator i = namespacePath.begin(); i != namespacePath.end(); ++i)
		if (!(ns = DPTR(ns)->LookupNamespace(*i)))
			return (DeltaLibraryNamespace*) 0;
	return ns;
}

///////////////////////////////////////////////////////////

void DeltaLibraryNamespace::OnNewBase (const std::string& name, const StringList& namespacePath, void* closure) {

#undef DELTANAMESPACES
#define DELTANAMESPACES DELTANAMESPACES_EX(data->first)

#undef COMPMESSENGER
#define COMPMESSENGER	COMPMESSENGER_EX(data->first)

	std::pair<CompilerComponentDirectory*, DeltaLibraryUserDefinedType*>* data =
		(std::pair<CompilerComponentDirectory*, DeltaLibraryUserDefinedType*>*) closure;

	DeltaLibraryUserDefinedType* type			= data->second;
	const DeltaLibraryUserDefinedType* baseType	= (DeltaLibraryUserDefinedType*) 0;

	if (namespacePath.empty())
		baseType = DELTANAMESPACES.LookupType(name);
	else
		baseType = DELTANAMESPACES.LookupType(namespacePath, name);

	if (baseType) {
		if (DPTR(type)->HasBaseType(baseType))
			DELTACOMP_ERROR_HAS_ALREADY_BASE_TYPE(DPTR(type)->GetName(), DPTR(baseType)->GetFullPath());
		else
			DPTR(type)->AddBaseType(baseType);
	}
}

///////////////////////////////////////////////////////////
