// AutoCompletion.cpp
// Simple and straight auto-completion support for Delta.
// ScriptFigher Project.
// A. Savidis, August 2010.
//

#include "utypes.h"
#include "DDebug.h"
#include "AutoCompletion.h"
#include "ustrings.h"
#include "Namespace.h"
#include "DeltaStdLibFuncNames.h"
#include "ugeometry.h"
#include "ustlhelpers.h"
#include "GenericReader.h"
#include "DeltaByteCodeTypes.h"
#include "LanguageModule.h"
#include "usystem.h"
#include "ufiles.h"
#include "ustrings.h"
#include "uerrorclass.h"
#include <ctype.h>
#include <vector>

//	#define	COMMENT_OUT_LIBFUNC_ARGUMENTS

#define	STD_LIB_INTERNAL_NAME		"***STD***"
#define	NS_GLOBAL					DELTA_LIBRARYNAMESPACE_SEPARATOR
#define	PARSEBACK_EOF				-1	// begin of file
#define	LISTBOX_MAX_SIZE			15
#define	FUNCS_PREFIX				"funcs"
#define	VARS_PREFIX					"vars"

#define	TYPE_FUNCTION				std::string("?0")
#define	TYPE_CONST					std::string("?7")
#define	TYPE_INDEX					std::string("?2")
#define	TYPE_GLOBAL_VAR				std::string("?3")
#define	TYPE_ATTR					std::string("?4")
#define	TYPE_NAMESPACE				std::string("?6")
#define	TYPE_NOIDEA					std::string("?5")

util_ui32							DeltaAutoCompletion::refCounter			= 0;
util_ui32							DeltaAutoCompletion::groupMinSize		= 0xffffffff;
DeltaAutoCompletion::SigGroups*		DeltaAutoCompletion::sigGroups			= (SigGroups*) 0;
DeltaAutoCompletion::GroupNsMap*	DeltaAutoCompletion::groupToNamespace	= (GroupNsMap*) 0;
DeltaAutoCompletion::LibMap*		DeltaAutoCompletion::libs				= (LibMap*) 0;
DeltaAutoCompletion::LangModMap*	DeltaAutoCompletion::langModules		= (LangModMap*) 0;
DeltaAutoCompletion::ByteCodeLibs*	DeltaAutoCompletion::byteCodeLibs		= (ByteCodeLibs*) 0;
DeltaAutoCompletion::ByteCodePaths*	DeltaAutoCompletion::byteCodePaths		= (ByteCodePaths*) 0;
bool								DeltaAutoCompletion::hadUserLibs		= false;
bool								DeltaAutoCompletion::selfAccess			= false;

#ifdef	COMMENT_OUT_LIBFUNC_ARGUMENTS
#define	TRANSFORM_LIBFUNC_ARGUMENTS		CommentOutFormalArguments
#else
#define	TRANSFORM_LIBFUNC_ARGUMENTS		MakeUntypedFormalArguments
#endif

static bool StringCaseInsensitiveLessThan (const std::string& s1, const std::string& s2) 
	{ std::string a(s1), b(s2); return ustrlower(a) < ustrlower(b); }

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::Initialise (void) {
	if (!refCounter++) {
		DASSERT(!sigGroups && !groupToNamespace); 
		unew(libs);
		unew(langModules);
		unew(byteCodeLibs);
		unew(byteCodePaths);
		unullify(sigGroups); 
		unullify(groupToNamespace);
		IncludeStdLibrarySigs(); 
	}
}

//******************************

void DeltaAutoCompletion::CleanUp (void) {

	DASSERT(refCounter);

	if (!--refCounter) {
		udelete(libs); 
		udelete(langModules);
		udelete(byteCodeLibs);
		udelete(byteCodePaths);
	}
	else
		DASSERT(sigGroups && groupToNamespace);
}

///////////////////////////////////////////////////////////////////////////////

const DeltaAutoCompletion::SigList DeltaAutoCompletion::SplitOverloadedSigs (const std::string& sig) {
	std::string name;
	SigList		sigs, args, rets;
	util_ui32	start = 0, end = 0;

	for (util_ui32 i = 0, n = (util_ui32) sig.length(); i < n; ++i)
		if (sig[i] == '(') {
			if (args.empty())
				name = sig.substr(0, i);
			else
				rets.push_back(sig.substr(end, i - end));
			start = i;
		}
		else
		if (sig[i] == ')')
			{ args.push_back(sig.substr(start, i - start + 1)); end = i + 1; }

	if (args.empty() || args.size() == 1)
		sigs.push_back(sig);
	else {
		rets.push_back(end < sig.length() ? sig.substr(end) : "");
		DASSERT(rets.size() == args.size());
		for (SigList::iterator i = args.begin(), j = rets.begin(); i != args.end(); ++i, ++j)
			sigs.push_back(name + *i + *j);
	}
	return sigs;
}

///////////////////////////////////////////////////////////////////////////////

DeltaAutoCompletion::PredefinedGroup DeltaAutoCompletion::GetPredefinedGroup (const std::string& prefix) {
	if (prefix == DELTA_LIBRARYNAMESPACE_SEPARATOR)
		return PredefinedGroupGlobals;
	if (prefix == FUNCS_PREFIX)
		return PredefinedGroupGlobalFuncs;
	if (prefix == VARS_PREFIX)
		return PredefinedGroupGlobalVars;
	return PredefinedGroupNone;
}				  	

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::MakeUntypedFormalArguments (const std::string& s) {

	std::list<std::string> tokens = utokenizestr(StripAllBlanks(s), "(");
	if (tokens.size() == 1)
		return s;

	std::string func = tokens.front();		// Record func name.
	tokens.pop_front();						// Drop function name.
	if (tokens.front()[0] == ')')			// Empty sig?
		return func + "()";

	tokens = utokenizestr(tokens.front(), ")");
	DASSERT(!tokens.front().empty());		// Has non-empty sig.

	std::string	sig, args = tokens.front();
	char end = 0;
	for (std::string::iterator i = args.begin(); i != args.end(); ++i)
		if (end) {
			if (end == ',' && *i == '{')
				end = '}';
			else
			if (*i == end)
				if (end == '}')
					end = ',';
				else
					sig += ",", end = 0;
		}
		else
		if (*i == ':')
			end = ',';
		else
			sig += *i;

	return func + "(" + sig + ")";
}

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::CommentOutFormalArguments (const std::string& s) {

	std::string result, remaining;
	bool lp = false;

	for (util_ui32 i = 0, n = (util_ui32) s.length(); i < n; ++i)
		if (!isspace(s[i])) {
			if (s[i] == '(' && !lp)
				{ lp = true; result += "()/*"; }	// put sig inside comments
			result += remaining + s[i]; 
			remaining.clear();
		}
		else
			remaining = " ";	// absorb white space into one space

	return result + (!lp ? "()" : "*/");	// close sig 
}

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::MakeNamespacePathPrefix (const NsPath& l) {
	if (l.empty())
		return "";
	else
	if (l.size() == 1)
		return l.front() + DELTA_LIBRARYNAMESPACE_SEPARATOR;
	else {
		std::string s;
		for (NsPath::const_iterator i = l.begin(); i != l.end(); ++i)
			s += *i + DELTA_LIBRARYNAMESPACE_SEPARATOR;
		return s;
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::InsideComments (ProgramDescription& progDesc, int startPos, int endPos) {

	ProgramDescription::RangeList&	comments = progDesc.GetComments();
	Range							curr(startPos, endPos);

	for (ProgramDescription::RangeList::iterator i = comments.begin(); i!= comments.end(); ++i)
		if (i->Contains(curr))	// inside comments
			return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::AddSigToAllNamespacesAcrossThePath (const NsPath& ns, const std::string& sig) {
	std::string s;
	for (NsPath::const_iterator i = ns.begin(); i != ns.end(); ++i) {
		s += *i + DELTA_LIBRARYNAMESPACE_SEPARATOR;
		(*sigGroups)[s].push_back(sig);
	}
	(*sigGroups)[NS_GLOBAL].push_back(sig);
}

///////////////////////////////////////////////////////////////////////////////

const DeltaAutoCompletion::SigListConstIter DeltaAutoCompletion::SkipTillNextLibraryName (
		const SigList&			defs, 
		const SigListConstIter&	i
	) {

	DASSERT(IsLibNameDefinition(*i));

	for (SigListConstIter j = i; ++j != defs.end();)
		if (IsLibNameDefinition(*j))
			return j;

	return defs.end();
}

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::StripAllBlanks (const std::string& s) {
	std::string result;
	for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
		if (!isspace(*i))
			result += *i;
	return result;
}

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::StripPreceedingAndTrailingBlanks (const std::string& s) {
	util_ui32 i = 0, j = (util_ui32) s.length() - 1;
	while (true) {
		if (isspace(s[i])) ++i;
		else
		if (isspace(s[j])) --j;
		else
			break;
		DASSERT(i < j);
	}
	return s.substr(i, j - i + 1);
}

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::ExtractConstIdent (const char* s) {
	DASSERT(s); 
	while (isspace(*s))
		++s;
	std::string id;
	DASSERT(isalpha(*s));
	do id += *s++; while (IsIdentChar(*s));
	return id;
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::SetLibraryFunctions (const SigList& defs, void* langModule) {

	NameList	groups;	// Stack
	NsPath		nsPath;
	std::string lib;

	// When no lib names are provided explicitly, they are all added in the shared stdlib.
	SetCurrentProcessedLibrary(lib = STD_LIB_INTERNAL_NAME);

	if (langModule) {
		SetUsingNoLibrary(langModule);		// Clear its used libraries as they are reset.
		SetIsUsingStdLibrary(langModule);	// But still uses the std.
	}

	for (SigList::const_iterator i = defs.begin(); i != defs.end(); ++i) {
		std::string def = StripPreceedingAndTrailingBlanks(*i);
		if (IsLibNameDefinition(def))
			if (!(lib = GetAutoCompleteDefValue(def, DELTA_LIBRARYNAMESPACE_AUTOCOMP_LIBNAME)).empty()) {
				if (IsExistingLibrary(lib))
					i = SkipTillNextLibraryName(defs, i);
				else
					AddNewLibrary(lib);
				ReferToLibrary(lib, DNULLCHECK(langModule));
				if (i == defs.end())
					break;
			}
			else	// In case of libname error everything is added in the shared std library
				SetCurrentProcessedLibrary(lib = STD_LIB_INTERNAL_NAME);
		else
		if (lib.empty())
			continue;
		else
		if (IsComment(def)) {
			bool isGroup = IsGroupDefinition(def), isGroupEx = IsGroupExDefinition(def);
			if (isGroup || isGroupEx) {
				std::string group =	GetAutoCompleteDefValue(
										def, 
										isGroupEx ? DELTA_LIBRARYNAMESPACE_AUTOCOMP_GROUPEX : 
													DELTA_LIBRARYNAMESPACE_AUTOCOMP_GROUP
									);
				if (!group.empty())
					if (group == DELTA_LIBRARYNAMESPACE_AUTOCOMP_NOGROUP) {
						if (!groups.empty())		// To avoid errors in definitions.
							groups.pop_back();		// Exiting top group.
					}
					else {
						if (!isGroupEx)				// Implicit exit from all other groups (including extra).
							groups.clear();			
						groups.push_back(group);	// Entering new (top) group.

						// Only user-defined auto-completion groups inside namespaces 
						// need to be aware of their enclosing namespace.

						if (!nsPath.empty())
							(*groupToNamespace)[group].insert(MakeNamespacePathPrefix(nsPath));
						groupMinSize = umin(groupMinSize, (util_ui32) group.length());
					}
			}
		}
		else
		if (def == DELTA_LIBRARYNAMESPACE_EXIT) {
			groups.clear();		// Exiting all groups of inner namespace.
			if (!nsPath.empty()) 
				nsPath.pop_back(); 
		}
		else
		if (def == DELTA_LIBRARYNAMESPACE_ENTER) { 
			groups.clear();		// Exiting all groups of enclosing namespace.
			if (!ulistiteratback(i, defs)) {
				def = StripPreceedingAndTrailingBlanks(*++i);
				AddSigToAllNamespacesAcrossThePath(nsPath, MakeNamespacePathPrefix(nsPath) + def + TYPE_NAMESPACE);
				nsPath.push_back(def);
			}
		}
		else
		if (def == DELTA_LIBRARYNAMESPACE_TYPE) 
			++i;
		else {

			// We handle consts here due to the similar manner we treat them.

			SigList		sigs;
			bool		isFunction = true;

			if (def == DELTA_LIBRARYNAMESPACE_CONST)
				if (!ulistiteratback(i, defs)) {
					def = StripPreceedingAndTrailingBlanks(*++i);
					isFunction = false;
					sigs.push_back(ExtractConstIdent(def.c_str()));
				}
				else
					return;
			else
				sigs = SplitOverloadedSigs(def);

			for (SigList::iterator j = sigs.begin(); j != sigs.end(); ++j) {

				std::string	sig	= MakeNamespacePathPrefix(nsPath);
				if (isFunction)
					sig += TRANSFORM_LIBFUNC_ARGUMENTS(*j) +  TYPE_FUNCTION;
				else
					sig += *j + TYPE_CONST;

				AddSigToAllNamespacesAcrossThePath(nsPath, sig);

				// Sig added in all current groups (including main and extra).
				for (NameList::iterator k = groups.begin(); k != groups.end(); ++k)
					(*sigGroups)[*k].push_back(sig);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::AddNewLibrary (const std::string& lib) {
	LibMap::iterator i = DPTR(libs)->find(lib);
	DASSERT(i == DPTR(libs)->end());
	(*DPTR(libs))[lib];	// Inserts.
	SetCurrentProcessedLibrary(lib);
}

//**************************

void DeltaAutoCompletion::SetCurrentProcessedLibrary (const std::string& lib) {
	LibMap::iterator i = DPTR(libs)->find(lib);
	DASSERT(i != DPTR(libs)->end());
	sigGroups			= &(i->second.first);
	groupToNamespace	= &(i->second.second);
}

//**************************

bool DeltaAutoCompletion::IsExistingLibrary (const std::string& lib)
	{ return DPTR(libs)->find(lib) != DPTR(libs)->end(); } 

//**************************

void DeltaAutoCompletion::ReferToLibrary (const std::string& lib, void* langModule) {
	if (!langModule)
		DASSERT(lib == STD_LIB_INTERNAL_NAME);
	else {
		LibMap::iterator i = DPTR(libs)->find(lib);
		DASSERT(i != DPTR(libs)->end());
		++(i->second.third);
		(*DPTR(langModules))[langModule].push_back(lib);
	}
}

//**************************

void DeltaAutoCompletion::UnreferToLibrary (const std::string& lib, void* langModule) {

	LibMap::iterator i = DPTR(libs)->find(lib);
	DASSERT(i != DPTR(libs)->end());

	DASSERT(i->second.third);
	if (!--(i->second.third))	// None refers to this lib
		DPTR(libs)->erase(i);	// Remove it.

	DASSERT(IsAlreadyUsingLibrary(lib, langModule));
	(*DPTR(langModules))[langModule].remove(lib);
}


//**************************

bool DeltaAutoCompletion::IsAlreadyUsingLibrary (const std::string& lib, void* langModule) {
	NameList& l = (*DPTR(langModules))[langModule];
	return std::find(l.begin(), l.end(), lib) != l.end();
}

//**************************

void DeltaAutoCompletion::SetIsUsingStdLibrary (void* langModule)
	{ (*DPTR(langModules))[langModule].push_back(STD_LIB_INTERNAL_NAME); }

//**************************

void DeltaAutoCompletion::SetUsingNoLibrary (void* langModule)
	{ (*DPTR(langModules))[langModule].clear(); }

//**************************

void DeltaAutoCompletion::HandleLanguageModuleInstantiation (void* langModule) {
	SetIsUsingStdLibrary(langModule);
	CreateByteCodeLibrariesRegistry(langModule);
}

//**************************

void DeltaAutoCompletion::HandleLanguageModuleDestruction (void* langModule) {
	DestroyByteCodeLibrariesRegistry(langModule);
	DPTR(langModules)->erase(langModule);
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::IncludeStdLibrarySigs (void) {
	AddNewLibrary(STD_LIB_INTERNAL_NAME);
	SigList sigs;
	for (UPTR(const char*) token = DeltaStdLib_FuncNames(); *token; ++token)
		sigs.push_back(*token);
	SetLibraryFunctions(sigs);
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::IsGroupSuggestionPrefixAndHasGroupSuggestions (const std::string& prefix) {
	for (LibMap::iterator i = DPTR(libs)->begin(); i != DPTR(libs)->end(); ++i) {
		SigGroups::iterator j = i->second.first.find(prefix);
		if (j != i->second.first.end() && j->second.size())
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Populates list 'l' with the names of all opened namespaces in 
// which the group was also defined.

void DeltaAutoCompletion::GetAllOpenedNamespacesHavingThisGroup (NameList* l, const std::string& group, int editorPos) {

	GroupNsMap::iterator i = groupToNamespace->find(group);

	if (i != groupToNamespace->end())	{ // It is a user-defined auto-completion group.
		Namespace::NamespaceList nsList = Namespace::GetOpenedNamespacesAtPos(editorPos);
		if (!nsList.empty())
			for (NameBag::iterator j = i->second.begin(); j != i->second.end(); ++j)
				if (Namespace::IsAnyNamespace(nsList, RemoveScopeResolutionSuffix(j->first)))	// Group namespace is opened.
					l->push_back(j->first);
	}
}

//******************************

const std::string GetFirstNamespace (const std::string& path) {
	for (util_ui32 i = 0; i < path.length(); ++i)
		if (path[i] == ':')
			return path.substr(0, i);
	DASSERT(false);
	return path;
}

//******************************

const DeltaAutoCompletion::NameList DeltaAutoCompletion::GetFullPathsOfAllMatchingOpenedNamespaces (
		const std::string&	path, 
		int					editorPos
	) {

	NameList paths;
	typedef Namespace::NamespaceList NamespaceList;
	NamespaceList nsList = Namespace::GetOpenedNamespacesAtPos(editorPos);

	if (!nsList.empty()) {
		std::string	pathCopy(path);
		do {
			std::string	id = GetFirstNamespace(pathCopy);
			for (NamespaceList::iterator i = nsList.begin(); i != nsList.end(); ) {
				if (const Namespace* ns = (*i)->LookupNamespace(id))
					nsList.push_front(const_cast<Namespace*>(ns));
				nsList.erase(i++);
			}
			if (nsList.empty())
				return paths;
			pathCopy = pathCopy.substr(id.length() + strlen(DELTA_LIBRARYNAMESPACE_SEPARATOR));
		} while (!pathCopy.empty());

		DASSERT(!nsList.empty());
		for (NamespaceList::iterator i = nsList.begin(); i != nsList.end(); ++i)
			paths.push_back((*i)->GetAbsId() + DELTA_LIBRARYNAMESPACE_SEPARATOR); 
	}

	return paths;
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::GetGroupSuggestions (
		const std::string&	group, 
		int					editorPos, 
		util_ui32*			total, 
		const NameList&		currLibs,
		NameList*			l
	) {
	for (NameList::const_iterator j = currLibs.begin(); j != currLibs.end(); ++j) {

		SetCurrentProcessedLibrary(*j);
		SigGroups::iterator i = sigGroups->find(group);	// No matching group in this lib.
		if (i == sigGroups->end())
			continue;
		DASSERT(!i->second.empty());	// Can't have groups without lib functions.

		NameList nsList;
		GetAllOpenedNamespacesHavingThisGroup(&nsList, group, editorPos);

		for (SigList::iterator j = i->second.begin(); j != i->second.end(); ++j)
			if (nsList.empty())
				l->push_back(*j);
			else {
				
				// Foreach ns of the group, if the ns is opened and the current sig 
				// has it as a prefix, then remove the prefix (function can be accessed normally).

				std::string sig = *j;
				for (NameList::iterator k = nsList.begin(); k != nsList.end(); ++k)
					if (ustrprefix(*k, *j))  // Drop the ns prefix as ns is used.
						{ sig = sig.substr(k->length()); break; }
				l->push_back(sig);
			}
	}
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::GetOpenedNamespaceSuggestions (
		const std::string&	path, 
		util_ui32			cropChars, 
		int					editorPos,
		util_ui32*			total,
		NameList*			l
	) {

	SigGroups::iterator i = sigGroups->find(path);
	if (i != sigGroups->end()) {
		*total += (util_ui32) i->second.size();
		for (SigList::iterator j = i->second.begin(); j != i->second.end(); ++j)
			l->push_back(j->substr(cropChars));
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::ParseBack_Failed (int startPos, int endPos)
	{ return startPos == PARSEBACK_EOF || startPos == endPos - 1; }

//******************************

int DeltaAutoCompletion::ParseBack_SkipBlanks (EditorWindow* editor, int pos) {
	while (pos != PARSEBACK_EOF && isspace(editor->GetCharAt(pos)))
		--pos;
	return pos;
}

//******************************

int	DeltaAutoCompletion::ParseBack_NamespaceAccess (EditorWindow* editor, int pos) {
	int newPos;
	if ((newPos = ParseBack_SkipBlanks(editor, pos)) == PARSEBACK_EOF)
		return pos;
	else
	if (newPos > 1 && editor->GetCharAt(newPos) == ':' && editor->GetCharAt(newPos - 1) == ':')
		if (newPos > 2 && isalnum(editor->GetCharAt(newPos-2))) {
			int wordPos = editor->WordStartPosition(newPos - 2, true);
			newPos = ParseBack_NamespaceAccess(editor, wordPos -1);
			return newPos == wordPos - 1 || newPos == PARSEBACK_EOF ? wordPos : newPos;
		}
		else
			return newPos - 1;
	else
		return pos;	// as it is, nothing to read
}

//******************************

int DeltaAutoCompletion::ParseBack_SelfAccess (EditorWindow* editor, int pos) {
	DASSERT(editor->GetCharAt(pos) == '.');
	int originalPos = pos;
	if (pos > 0 && editor->GetCharAt(pos - 1) == '.')	// double dot
		--pos;
	if (pos < (int) strlen(DELTA_SELF_POINTER_ID))
		return PARSEBACK_EOF;
	if (editor->GetCharAt(pos - 1)	== 'f' &&
		editor->GetCharAt(pos - 2)	== 'l' &&
		editor->GetCharAt(pos - 3)	== 'e' &&
		editor->GetCharAt(pos - 4)	== 's')
		return pos - 4;
	else
		return originalPos;
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::Handle (void* langModule, EditorWindow* editor, ProgramDescription& progDesc, int keyCode) {

	if (!keyCode)	// Accelerator mode.
		if (editor->AutoCompActive() || !editor->GetCurrentPos())	// No char at left.
			return false;
		else
			keyCode = editor->GetCharAt(editor->GetCurrentPos() - 1);

	// Hack to handle transition from 'self.' to 'self..' gracefully.
	if (editor->AutoCompActive() && selfAccess) {	// Was auto-complete for 'self.'
		if (keyCode == '.')	// Now it is 'self..'
			editor->AutoCompCancel();
		selfAccess = false;
	}

	if (!editor->AutoCompActive() && !InsideComments(progDesc, editor->GetCurrentPos(), editor->GetCurrentPos())) {

		bool		prefixBased			= true;
		bool		isNamespace			= false;
		int			endPos				= editor->GetCurrentPos();	// After the inserted character
		int			startPos			= endPos - 1;
		util_ui32	totalSuggestions	= 0;
		String		suggestions;
		std::string	prefix;
		
		selfAccess = false;

		if (isalnum(keyCode))
			startPos = editor->WordStartPosition(endPos, true);
		else 
		if (keyCode == ':') {
			if (ParseBack_Failed(startPos = ParseBack_NamespaceAccess(editor, endPos - 1), endPos))
				return false;	// Failed while parsing.
			DASSERT((startPos + (int) strlen(DELTA_LIBRARYNAMESPACE_SEPARATOR)) <= endPos);
			isNamespace = true;
		}
		else
		if (keyCode == '.' || keyCode == '@') {
			if (keyCode == '.') {
				if (ParseBack_Failed(startPos = ParseBack_SelfAccess(editor, endPos - 1), endPos))
					return false;	// Failed while parsing.
				DASSERT((startPos + (int) strlen(DELTA_SELF_POINTER_ID)) <= endPos);
			}
			suggestions =	GetSelfKeySuggestions(
								editor, 
								progDesc, 
								keyCode == '@', 
								prefix = util::str2std(editor->GetTextRange(startPos, endPos)), 
								&totalSuggestions
							);
			prefixBased = false;
			selfAccess	= true;
		}
		
		if (prefixBased && ((util_ui32) (endPos - startPos)) >= GetGroupMinSize()) {

			prefix = util::str2std(editor->GetTextRange(startPos, endPos));

			const NameList& currLibs		= (*DPTR(langModules))[langModule];
			DASSERT(!currLibs.empty() && currLibs.front() == STD_LIB_INTERNAL_NAME);
			bool			hasLibraryGroup	= IsGroupSuggestionPrefixAndHasGroupSuggestions(prefix);
			PredefinedGroup	predefinedGroup	= GetPredefinedGroup(prefix);

			if (hasLibraryGroup || predefinedGroup != PredefinedGroupNone) {
				NameList l;
				if (predefinedGroup != PredefinedGroupNone)
					ExtractGlobalFunctionsAndVariables(editor, progDesc.GetAST(), predefinedGroup, &l);
				if (hasLibraryGroup)
					GetGroupSuggestions(prefix, endPos, &totalSuggestions, currLibs, &l);
				suggestions = MakeSuggestions(l);
			}
			else 
			if (isNamespace) {
				NameList tokens = utokenizestr(prefix, ":");
				if (tokens.size() == 1 && HasByteCodeLibrary(langModule, tokens.front()))	// <id>::, check if bytecode library
					{ suggestions = GetByteCodeLibrarySuggestions(langModule, tokens.front()); prefix.clear(); }	// Clear prefix to retain it in auto completion.
				else {
					NameList l;
					GetNamespaceSuggestions(editor, progDesc, endPos, prefix, &totalSuggestions, currLibs, &l);
					suggestions = MakeSuggestions(l);
				}
			}
		}

		if (!suggestions.empty()) {
			editor->AutoCompSetMaxHeight((int) umin(totalSuggestions, (util_ui32) LISTBOX_MAX_SIZE));
			editor->AutoCompSetAutoHide(true);
			editor->AutoCompShow((int) prefix.size(), suggestions);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////

DeltaASTNode* DeltaAutoCompletion::FindParentMethodFromItsStmt (DeltaASTNode* node) {
	while (node = node->GetParent())
		switch (node->GetType()) {
			case FunctionASTNodeType:		return node;
			case TableConstructASTNodeType: return (DeltaASTNode*) 0;
			default: continue;
		}
	return (DeltaASTNode*) 0;
}

///////////////////////////////////////////////////////////////////////////////

DeltaASTNode* DeltaAutoCompletion::FindParentObjectConstructorFromItsMethodPosition (ProgramDescription& progDesc, int pos) {
	if (DeltaASTNode* node = progDesc.GetNode(pos))
		if (node = FindParentMethodFromItsStmt(node))
			while (node = node->GetParent())
				switch (node->GetType()) {
					case StmtsASTNodeType: case FunctionASTNodeType: return (DeltaASTNode*) 0;
					case TableConstructASTNodeType: return node;
					default: continue;
				}

	return (DeltaASTNode*) 0;
}

///////////////////////////////////////////////////////////////////////////////

const std::string DeltaAutoCompletion::GetText (EditorWindow* editor, DeltaASTNode* node) {
	const DeltaASTNode::Range& range = node->GetRange();
	return util::str2std(editor->GetTextRange(range.left, range.right));
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::IsIdent (const std::string& text) {
	if (!isalpha(text[0]))
		return false;
	for (util_ui32 i = 1; i < text.length(); ++i)
		if (!IsIdentChar(text[i]))
			return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::ExtractSlotKey (
		EditorWindow*		editor,
		DeltaASTNode*		slotDef,
		NameList*			at,
		bool				attrAccess
	) {
	switch (slotDef->GetType()) {

		case TableIndexListASTNodeType : {

			DeltaASTNodeList l;
			slotDef->GetChildren(l);

			for (DeltaASTNodeList::iterator i = l.begin(); i != l.end(); ++i) 
				if ((*i)->GetType() == TableConstKeyASTNodeType)
					ExtractSlotKey(editor, *i, at, attrAccess);
			break;
		}

		//*********************************

		case TableConstKeyASTNodeType : {

			std::string text = GetText(editor, slotDef);
			TableConstKeyASTNode* node = (TableConstKeyASTNode*) slotDef;

			switch (node->GetValue()) {

				case CONST_KEY_DOTID	: DASSERT(text[0] == '.'); at->push_back(text.substr(1) + TYPE_INDEX);	break;
				case CONST_KEY_IDENT	: at->push_back(text + TYPE_INDEX); break;

				case CONST_KEY_STRING	: {
					if (!attrAccess)
						at->push_back(text);
					else {
						DASSERT(text[0] == '"' && text[text.length() - 1] == '"' );
						if (IsIdent(text = text.substr(1, text.length() - 2)))
							at->push_back(text + TYPE_INDEX);
					}
					break;
				}

				case CONST_KEY_STRINGIFIED_ID	: {
					DASSERT(text[0] == '#');
					std::string id = text.substr(1);
					if (IsIdent(id))	// if still id without # then strip it
						at->push_back(id + TYPE_INDEX);
					else
					if (!attrAccess)
						at->push_back(text + TYPE_INDEX);
					break;
				}

				case CONST_KEY_OPSTRING	: { 
					if (!attrAccess) 
						at->push_back(text + TYPE_INDEX); 
					break;
				}
				default: break;
			}

			break;
		}

		//*********************************

		case AttributeASTNodeType : {
			std::string text = GetText(editor, slotDef);
			DASSERT(text[0] == '@');
			at->push_back(text.substr(1) + TYPE_ATTR);
			break;
		}

		//*********************************

		case NewAttributeASTNodeType : {
			DeltaASTNodeList l;
			slotDef->GetChildren(l);
			DASSERT(l.size() == 3 && l.front()->GetType() ==  AttributeASTNodeType); // attr set get
			ExtractSlotKey(editor, l.front(), at, attrAccess);
		}

		//*********************************

		case ConstASTNodeType : {

			ConstASTNode* node = (ConstASTNode*) slotDef;

			if (node->GetValue() == CONST_STRING) {
				
				std::string text			= GetText(editor, node);
				bool		isIdentIndex	= slotDef->GetParent()->GetType() == TableElemASTNodeType;

				if (isIdentIndex) 
					{ DASSERT(text[0] == '@'); text = text.substr(1); DASSERT(IsIdent(text)); }

				if (!attrAccess || isIdentIndex)
					at->push_back(text + TYPE_INDEX);
				else {
					DASSERT(text[0] == '"' && text[text.length() - 1] == '"');
					if (IsIdent(text = text.substr(1, text.length() - 2)))
						at->push_back(text + TYPE_INDEX);
				}
			}
			break;
		}
		
		//*********************************

		case FunctionASTNodeType : {
			FunctionASTNode* node = (FunctionASTNode*) slotDef;
			if (node->GetValue() == "method" && node->GetChild<0>() && node->GetChild<0>()->GetType() == FunctionNameASTNodeType) {
				std::string name = GetText(editor, node->GetChild<0>());
				std::string args = node->GetChild<1>()? GetText(editor, node->GetChild<1>()) : "";
				if (name[0] != '@')	// No operator method
					at->push_back(name + "(" + args + ")" + TYPE_FUNCTION);
			}
			break;
		}

		default: break;
	}
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::CollectAllObjectConstructorIdentSlotKeys (
		EditorWindow*		editor,
		ProgramDescription& progDesc, 
		int					pos, 
		NameList*			at,
		bool				attrAccess
	) {

	if (DeltaASTNode* node = FindParentObjectConstructorFromItsMethodPosition(progDesc, pos)) {

		DeltaASTNodeList children;
		node->GetChildren(children);
		DASSERT(children.size() == 1);

		node = children.front();
		DASSERT(node->GetType() == TableElemsASTNodeType);

		children.clear();
		node->GetChildren(children);
		for (DeltaASTNodeList::iterator i = children.begin(); i != children.end(); ++i) {

			DASSERT((*i)->GetType() == TableElemASTNodeType);
			DeltaASTNodeList elemNodes;

			(*i)->GetChildren(elemNodes);
			DASSERT(elemNodes.size() == 1 || elemNodes.size() == 2);

			ExtractSlotKey(editor, elemNodes.front(), at, attrAccess);
		}
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////

const String DeltaAutoCompletion::MakeSuggestions (const NameVec& v, const std::string& prefix) {
	String s;
	for (NameVec::const_iterator i = v.begin(); i != v.end(); ++i) {
		if (!s.empty())
			s.append(_T('\a'));
		s.Append(util::std2str(prefix + *i));
	}
	return s;
}

const String DeltaAutoCompletion::MakeSuggestions (const NameList& l) {
	NameVec v;
	ulist2vector(l, v);
	std::sort(v.begin(), v.end(), StringCaseInsensitiveLessThan);
	return MakeSuggestions(v);
}

const DeltaAutoCompletion::NameList& DeltaAutoCompletion::RemoveCommonPrefix (NameList* l, const std::string& prefix) {
	DASSERT(!prefix.empty());
	util_ui32 len = (util_ui32) prefix.length();
	for (NameList::iterator i = l->begin(); i != l->end(); ++i)
		if (ustrprefix(prefix, *i))
			*i = i->substr(len);
	return *l;
}

///////////////////////////////////////////////////////////////////////////////

const String DeltaAutoCompletion::GetSelfKeySuggestions (
		EditorWindow*		editor,
		ProgramDescription& progDesc, 
		bool				attrAccess,
		const std::string&	prefix,
		util_ui32*			total
	) {
	String suggestions;
	NameList l;

	if (CollectAllObjectConstructorIdentSlotKeys(editor, progDesc, editor->GetCurrentPos(), &l, attrAccess)) {

		NameVec v;
		ulist2vector(l, v);
		std::sort(v.begin(), v.end(), StringCaseInsensitiveLessThan);

		suggestions = MakeSuggestions(v, prefix);
	}

	*total += (util_ui32) l.size();
	return suggestions;
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::GetNamespaceSuggestions(
			EditorWindow*		editor,
			ProgramDescription& progDesc, 
			int					pos, 
			const std::string&	namespaceId,
			util_ui32*			total,
			const NameList&		currLibs,
			NameList*			l
		) {

	for (NameList::const_iterator j = currLibs.begin(); j != currLibs.end(); ++j) {

		SetCurrentProcessedLibrary(*j);
		NameList paths = GetFullPathsOfAllMatchingOpenedNamespaces(namespaceId, pos);

		for (NameList::iterator i = paths.begin(); i != paths.end(); ++i)
			GetOpenedNamespaceSuggestions(
				*i, 
				(util_ui32) (i->length() - namespaceId.length()), // crop the ns prefix since ns is opened
				pos,
				total,
				l
			);
	}
}

//---------------------------------------------------------------------------//
// BYTE CODE LIBRARIES

static const std::string GetInitialByteCodePath (bool addSemi = false) {
	std::string path = ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH);
	if (!path.empty() && addSemi)
		path += ";";
	return path;
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::CreateByteCodeLibrariesRegistry (void* langModule) {
	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) == DPTR(byteCodeLibs)->end());
	DASSERT(byteCodePaths && DPTR(byteCodePaths)->find(langModule) == DPTR(byteCodePaths)->end());
	(*DPTR(byteCodePaths))[langModule] = GetInitialByteCodePath();
	(*DPTR(byteCodeLibs))[langModule].clear();
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::ClearByteCodeLibraries (void* langModule) {
	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());
	(*DPTR(byteCodeLibs))[langModule].clear();
}

void DeltaAutoCompletion::DestroyByteCodeLibrariesRegistry (void* langModule) {
	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());
	DASSERT(byteCodePaths && DPTR(byteCodePaths)->find(langModule) != DPTR(byteCodePaths)->end());
	DPTR(byteCodeLibs)->erase(langModule);
	DPTR(byteCodePaths)->erase(langModule);
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::SetByteCodeLoadingPath (void* langModule, EditorWindow* editor, DeltaASTNode* node, const std::string& path) {
	DASSERT(byteCodePaths && DPTR(byteCodePaths)->find(langModule) != DPTR(byteCodePaths)->end());
	(*DPTR(byteCodePaths))[langModule] = GetInitialByteCodePath(true) + path;
	ClearByteCodeLibraries (langModule);
	HandleSourceCodeUpdate(langModule, editor, node);	// i-sense call
}

///////////////////////////////////////////////////////////////////////////////

static const std::string MakeSignature (DeltaStdFuncInfo* func) {
	std::string sig;
	for (util_ui16 i = 0; i < func->GetTotalFormals(); ++i) {
		std::string arg = func->GetFormalName(i);
		if (arg != DELTA_SELF_POINTER_ID && arg != DELTA_ARGUMENTS_POINTER_ID)
			sig += std::string(sig.empty() ? "" : ",") +  arg;
	}
	return std::string("(") + sig + ")";
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::AddByteCodeLibrary (void* langModule, const std::string& file, const std::string& id) {

	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());
	DASSERT(byteCodePaths && DPTR(byteCodePaths)->find(langModule) != DPTR(byteCodePaths)->end());

	std::string fullPath = ufileresolvewithpaths(file, utokenizestr((*DPTR(byteCodePaths))[langModule], ";,"));
	if (fullPath.empty())
		return;
	
	FILE* fp = ubinaryfileopen(fullPath, "r");
	if (!fp)
		return;

	BinFileReader reader(fp);
	std::string sourceFile;
	if (!DeltaReadVersionInformation(reader) || !reader.read(sourceFile, false))
		{ UERROR_CLEAR(); fclose(fp); return; }

	util_ui16			total	= 0;
	DeltaStdFuncInfo*	funcs	= DeltaStdFuncInfo::ReadFunctionTable(reader, &total);
	if (!funcs)
		{ UERROR_CLEAR(); fclose(fp); return; }

	fclose(fp);
	ByteCodeLibInfo libInfo;
	libInfo.first = id, libInfo.second = file;
	for (DeltaStdFuncInfo* p = funcs; total; --total, ++p)
		if (p->IsExported()) {
			DASSERT(p->IsNamedFunction() && p->IsGlobal());
			libInfo.third.push_back(p->GetName() + MakeSignature(p) + TYPE_FUNCTION);
		}
	DDELARR(funcs);

	(*DPTR(byteCodeLibs))[langModule].push_back(libInfo);
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::RemoveByteCodeLibrary (void* langModule, const std::string& id) {

	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());

	ByteCodeLibList& libs = (*DPTR(byteCodeLibs))[langModule];
	ByteCodeLibList::iterator i =	std::find_if(
										libs.begin(),
										libs.end(),
										std::bind2nd(ByteCodeLibMatchPred(), id)
									);
	DASSERT(i != libs.end());	// Must be found.
	libs.erase(i);
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::HasByteCodeLibrary (void* langModule, const std::string& libName) {

	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());

	ByteCodeLibList& libs = (*DPTR(byteCodeLibs))[langModule];
	return	std::find_if(
				libs.begin(),
				libs.end(),
				std::bind2nd(ByteCodeLibMatchPred(), libName)
			) != libs.end();
}

///////////////////////////////////////////////////////////////////////////////

bool DeltaAutoCompletion::HasByteCodeLibraryFunction (void* langModule, const std::string& libName, const std::string& funcName) {

	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());

	ByteCodeLibList& libs = (*DPTR(byteCodeLibs))[langModule];
	ByteCodeLibList::iterator i =	std::find_if(
										libs.begin(),
										libs.end(),
										std::bind2nd(ByteCodeLibMatchPred(), libName)
									);
	return	i != libs.end() && 
			std::find_if(
				i->third.begin(), 
				i->third.end(), 
				std::bind2nd(ByteCodeLibFuncSigMatchPred(), funcName)
			) != i->third.end();
}

///////////////////////////////////////////////////////////////////////////////

const String DeltaAutoCompletion::GetByteCodeLibrarySuggestions (void* langModule, const std::string libName) {

	DASSERT(byteCodeLibs && DPTR(byteCodeLibs)->find(langModule) != DPTR(byteCodeLibs)->end());

	ByteCodeLibList& libs = (*DPTR(byteCodeLibs))[langModule];
	ByteCodeLibList::iterator i =	std::find_if(
										libs.begin(),
										libs.end(),
										std::bind2nd(ByteCodeLibMatchPred(), libName)
									);
	DASSERT (i != libs.end());

	NameVec v;
	ulist2vector(i->third, v);
	std::sort(v.begin(), v.end(), StringCaseInsensitiveLessThan);

	return MakeSuggestions(v);
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::ExtractUsedByteCodeLibaries (
		EditorWindow*		editor, 
		DeltaASTNode*		root,
		NamePairList*		byteCodeLibs
	) {

	if (!root)
		return;

	DASSERT(root->GetType() == StmtsASTNodeType);
	DeltaASTNodeList l;
	root->GetChildren(l);

	for (DeltaASTNodeList::iterator i = l.begin(); i != l.end() && (*i)->GetType() == UsingASTNodeType; ++i) {

		UsingASTNode* node = static_cast<UsingASTNode*>((*i));

		if (node->GetChild<0>() && !node->GetChild<1>())			// it is 'using <namespace>' form
			;
		else {
			DASSERT(!node->GetChild<0>() && node->GetChild<1>());	// it is 'using #<ident>' form
			DASSERT(node->GetChild<1>()->GetType() == StringifiedIdASTNodeType);

			std::string libName = GetText(editor, node->GetChild<1>());
			DASSERT(libName[0] == '#');
			libName = libName.substr(1);
			byteCodeLibs->push_back(
				NamePair(
					libName + ".dbc",
					libName
				)
			);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::ExtractGlobalFunctionsAndVariables (
		EditorWindow*		editor, 
		DeltaASTNode*		node,
		PredefinedGroup		group,
		NameList*			funcsAndVars
	) {
	DASSERT(group != PredefinedGroupNone);
	ubag<std::string> vars, funcs;
	ExtractGlobalFunctionsAndVariablesRecursion(editor, node, group, funcsAndVars, &vars, &funcs);
}
									
///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::ExtractGlobalFunctionsAndVariablesRecursion (
		EditorWindow*		editor, 
		DeltaASTNode*		node,
		PredefinedGroup		group,
		NameList*			funcsAndVars,
		ubag<std::string>*	vars,
		ubag<std::string>*	funcs
	) {

	if (node->GetRange().IsRightOf(editor->GetCurrentPos()))
		return;

	switch (node->GetType()) {
		case FunctionASTNodeType : {

			if (group != PredefinedGroupGlobalFuncs && group != PredefinedGroupGlobals)
				return;

			FunctionASTNode* func = (FunctionASTNode*) node;
			if (func->GetValue() == "function" && func->GetChild<0>() && func->GetChild<0>()->GetType() == FunctionNameASTNodeType) {

				std::string name = GetText(editor, func->GetChild<0>());
				std::string args = func->GetChild<1>() ? GetText(editor, func->GetChild<1>()) : "";

				DASSERT(!vars->in(name) && !funcs->in(name));
				funcsAndVars->push_back(std::string(DELTA_LIBRARYNAMESPACE_SEPARATOR) + name + "(" + args + ")" + TYPE_FUNCTION);
				funcs->insert(name);
			}
			break;
		}

		case VariableASTNodeType: {

			if (group != PredefinedGroupGlobalVars && group != PredefinedGroupGlobals)
				return;

			DeltaASTNode* parent = node->GetParent();
			DASSERT(parent);
			if (parent->GetType() != FunctionNameASTNodeType && parent->GetType() != UsingASTNodeType) {

				std::string name = GetText(editor, node);

				if (!vars->in(name) && !funcs->in(name)) {
					funcsAndVars->push_back(std::string(DELTA_LIBRARYNAMESPACE_SEPARATOR) + name + TYPE_GLOBAL_VAR);
					vars->insert(name);
				}
			}
			break;
		}

		case CompoundASTNodeType: break;

		default: {
			DeltaASTNodeList l;
			node->GetChildren(l);
			for (DeltaASTNodeList::iterator i = l.begin(); i != l.end(); ++i)
				ExtractGlobalFunctionsAndVariablesRecursion(editor, *i, group, funcsAndVars, vars, funcs);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void DeltaAutoCompletion::HandleSourceCodeUpdate (
		void*				langModule,
		EditorWindow*		editor, 
		DeltaASTNode*		node
	) {

	// Extract currently used byte code libraries.
	NamePairList currLibs;
	ExtractUsedByteCodeLibaries(editor, node, &currLibs);

	if (currLibs.empty())	// None used, clear all.
		(*DPTR(byteCodeLibs))[langModule].clear();
	else {

		// Add new used libs and also identify those that are used no more, 
		// thus should be removed.

		NameList removeList;
		ByteCodeLibList& libs = (*DPTR(byteCodeLibs))[langModule];
		for (ByteCodeLibList::iterator i = libs.begin(); i != libs.end(); ++i)
			removeList.push_back(i->first);

		for (NamePairList::iterator i = currLibs.begin(); i != currLibs.end(); ++i)
			if (!HasByteCodeLibrary(langModule, i->second))				// Its a new lib
				AddByteCodeLibrary(langModule, i->first, i->second);
			else														// Its a retained old lib
				removeList.remove(i->second);

		for (NameList::iterator i = removeList.begin(); i != removeList.end(); ++i)
			RemoveByteCodeLibrary(langModule, *i);
	}
}

//---------------------------------------------------------------------------//
