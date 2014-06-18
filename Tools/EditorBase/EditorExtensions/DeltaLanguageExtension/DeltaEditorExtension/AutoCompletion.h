// AutoCompletion.h
// Simple and straight auto-completion support for Delta.
// ScriptFigher Project.
// A. Savidis, August 2010.
//

#ifndef	DELTA_AUTOCOMPLETION_H
#define	DELTA_AUTOCOMPLETION_H

#include "StringDefs.h"
#include "Common.h"
#include "EditorMain/EditorWindow.h"
#include "ProgramDescription.h"
#include "utypes.h"
#include "DeltaStdDefs.h"
#include "ustrings.h"
#include "ufunctors.h"
#include <list>
#include <map>
#include <string>
#include <vector>
#include <functional>

using namespace editor;
using namespace delta;

///////////////////////////////////////////////////////////////////////////////

class DeltaAutoCompletion {

	friend class DeltaGotoDefinition;

	private:

	typedef std::list<std::string>						SigList;
	typedef SigList::iterator							SigListIter;
	typedef SigList::const_iterator						SigListConstIter;
	typedef std::vector<std::string>					NameVec;
	typedef std::list<std::string>						NameList;
	typedef std::list<std::string>						NsPath;
	typedef std::pair<std::string, std::string>			NamePair;
	typedef std::list<NamePair>							NamePairList;
	typedef ubag<std::string>							NameBag;
	typedef std::map<std::string, SigList>				SigGroups;	// Groups encompass all sigs they match, even across namespaces.
	typedef std::map<std::string, NameBag>				GroupNsMap;	// Auto-completion groups know they span across namespaces.

											// lib name, lib file,    funcs
	struct ByteCodeLibInfo : public utriple<std::string, std::string, NameList> {
		ByteCodeLibInfo (void) : utriple<std::string, std::string, NameList>("", "", NameList()) {}
	};

	struct ByteCodeLibMatchPred : public std::binary_function<ByteCodeLibInfo, std::string, bool> {
		bool operator()(const ByteCodeLibInfo& info, const std::string& id) const
			{ return info.first == id; }
	};

	struct ByteCodeLibFuncSigMatchPred : public std::binary_function<std::string, std::string, bool> {
		bool operator()(const std::string& sig, const std::string& name) const
			{ return ustrprefix(name, sig) && sig[name.length()] == '('; }
	};

	struct LibInfo : public utriple<SigGroups, GroupNsMap, util_ui32> {
		LibInfo (void) : utriple<SigGroups, GroupNsMap, util_ui32>(SigGroups(), GroupNsMap(), 0) {}
	};

	typedef std::map<std::string, LibInfo>				LibMap;
	typedef std::list<ByteCodeLibInfo>					ByteCodeLibList;
	typedef std::map<void*, NameList>					LangModMap;
	typedef std::map<void*, ByteCodeLibList>			ByteCodeLibs;
	typedef std::map<void*, std::string>				ByteCodePaths;

	static bool					selfAccess;
	static util_ui32			refCounter;	
	static bool					hadUserLibs;
	static util_ui32			groupMinSize;

	static SigGroups*			sigGroups;					// Current processd sigs groups.
	static GroupNsMap*			groupToNamespace;			// Current processed group to ns map.

	static LibMap*				libs;						// Mapping a library to its data.
	static LangModMap*			langModules;				// Mapping language module to its supported libs.
	static ByteCodeLibs*		byteCodeLibs;				// Mapping language module to its bytecode libraries.
	static ByteCodePaths*		byteCodePaths;				// Mapping language module to its bytecode loading path.
		
	enum PredefinedGroup {
		PredefinedGroupGlobals		= 0,
		PredefinedGroupGlobalVars	= 1,
		PredefinedGroupGlobalFuncs	= 2,
		PredefinedGroupNone			= 3	
	};

	//**************************
	// Utility functions to process sigs and namespace paths.

	static bool					IsComment (const std::string& s) 
									{ return s[0] == *DELTA_LIBRARYNAMESPACE_COMMENT; }
	static bool					IsGroupDefinition (const std::string& s) 
									{ return ustrprefix(DELTA_LIBRARYNAMESPACE_AUTOCOMP_GROUP, s); }
	static bool					IsGroupExDefinition (const std::string& s) 
									{ return ustrprefix(DELTA_LIBRARYNAMESPACE_AUTOCOMP_GROUPEX, s); }
	static bool					IsLibNameDefinition (const std::string& s) 
									{ return ustrprefix(DELTA_LIBRARYNAMESPACE_AUTOCOMP_LIBNAME, s); }
	static const std::string	GetAutoCompleteDefValue (const std::string& def, const std::string& kwd) {
									std::string s = def.substr(kwd.length());
									if (s[0] == '(' && s[s.length() - 1] == ')')
										return s.substr(1, s.length() - 2);
									else
										return "";
								}
	static PredefinedGroup		GetPredefinedGroup (const std::string& prefix);
	static const std::string	RemoveScopeResolutionSuffix (const std::string ns)
									{ return ns.substr(0, ns.length() - 2); }

	static bool					InsideComments (ProgramDescription& progDesc, int startPos, int endPos);
	static const SigList		SplitOverloadedSigs (const std::string& sig);
	static const std::string	StripPreceedingAndTrailingBlanks (const std::string& s);
	static const std::string	StripAllBlanks (const std::string& s);
	static const std::string	ExtractConstIdent (const char* s);
	static const std::string	MakeUntypedFormalArguments (const std::string& s);
	static const std::string	CommentOutFormalArguments (const std::string& s);
	static const std::string	MakeNamespacePathPrefix (const NsPath& l);
	static util_ui32			GetGroupMinSize (void) 
									{ return groupMinSize; }

	//**************************
	// Looking up in namespace libfunc sigs.

	static const NameList		GetFullPathsOfAllMatchingOpenedNamespaces (
									const std::string&	path, 
									int					editorPos
								);
	static void					AddSigToAllNamespacesAcrossThePath (
									const NsPath&		ns, 
									const std::string&	sig
								);
	static void					GetAllOpenedNamespacesHavingThisGroup (
									NameList*			l, 
									const std::string&	group, 
									int					editorPos
								);

	//**************************

	static bool					IsExistingLibrary (const std::string& lib);
	static void					AddNewLibrary (const std::string& lib);
	static void					SetCurrentProcessedLibrary (const std::string& lib);

	static void					SetUsingNoLibrary (void* langModule);
	static void					SetIsUsingStdLibrary (void* langModule);
	static void					ReferToLibrary (const std::string& lib, void* langModule);
	static void					UnreferToLibrary (const std::string& lib, void* langModule);
	static bool					IsAlreadyUsingLibrary (const std::string& lib, void* langModule);

	static void					IncludeStdLibrarySigs (void);
	static const SigListConstIter		
								SkipTillNextLibraryName (const SigList& defs, const SigListConstIter& i);

	//**************************
	// Back parsing in editor.

	static bool					ParseBack_Failed (int startPos, int endPos);
	static int					ParseBack_SkipBlanks (EditorWindow* editor, int pos);
	static int					ParseBack_NamespaceAccess (EditorWindow* editor, int pos);
	static int					ParseBack_SelfAccess (EditorWindow* editor, int pos);
	static bool					IsIdentChar (int keyCode) 
									{ return isalnum(keyCode) || keyCode == '_'; }

	//**************************
	// Slot suggestions (self).

	static const std::string	GetText (EditorWindow* editor, DeltaASTNode* node);
	static bool					IsIdent (const std::string& text);
	static DeltaASTNode*		FindParentMethodFromItsStmt (DeltaASTNode* node);
	static DeltaASTNode*		FindParentObjectConstructorFromItsMethodPosition (
									ProgramDescription& progDesc, 
									int					editorPos
								);
	static void					ExtractSlotKey (
									EditorWindow*		editor,
									DeltaASTNode*		slotDef,
									NameList*			at,
									bool				attrAccess	// via @<id>
								);
	static bool					CollectAllObjectConstructorIdentSlotKeys (
									EditorWindow*		editor,
									ProgramDescription& progDesc, 
									int					pos, 
									NameList*			at,
									bool				attrAccess	// via @<id>
								);

	//**************************
	// Suggestions.

	static bool					IsGroupSuggestionPrefixAndHasGroupSuggestions (const std::string& prefix);
	static const String			MakeSuggestions (
									const NameVec&		v, 
									const std::string&	prefix = ""
								);
	static const String			MakeSuggestions (const NameList& l);
	static const NameList&		RemoveCommonPrefix (NameList* l, const std::string& prefix);
	static void					GetOpenedNamespaceSuggestions (
									const std::string&	path, 
									util_ui32			cropChars, 
									int					editorPos, 
									util_ui32*			total,
									NameList*			l
								);
	static void					GetGroupSuggestions (
									const std::string&	prefix, 
									int					editorPos, 
									util_ui32*			total,
									const NameList&		currLibs,
									NameList*			l
								);
	static const String			GetSelfKeySuggestions (
									EditorWindow*		editor,
									ProgramDescription& progDesc, 
									bool				attrAccess,
									const std::string&	prefix,
									util_ui32*			total
								);
	static void					GetNamespaceSuggestions(
									EditorWindow*		editor,
									ProgramDescription& progDesc, 
									int					pos, 
									const std::string&	prefix,
									util_ui32*			total,
									const NameList&		currLibs,
									NameList*			l
								);

	static uint					GetStagingDepth (const DeltaASTNode* node);

	//**************************
	// Bytecode libraries.

	static void					CreateByteCodeLibrariesRegistry (void* langModule);
	static void					DestroyByteCodeLibrariesRegistry (void* langModule);
	static void					ClearByteCodeLibraries (void* langModule);
	static void					AddByteCodeLibrary (void* langModule, const std::string& file, const std::string& id);
	static void					RemoveByteCodeLibrary (void* langModule, const std::string& id);
	static bool					HasByteCodeLibrary (void* langModule, const std::string& libName);
	static const String			GetByteCodeLibrarySuggestions (
									void*				langModule,
									const std::string	libName
								);
	static void					ExtractUsedByteCodeLibaries(
									EditorWindow*		editor, 
									DeltaASTNode*		node,
									NamePairList*		byteCodeLibs
								);
	static void					ExtractGlobalFunctionsAndVariables (
									EditorWindow*		editor, 
									DeltaASTNode*		node,
									PredefinedGroup		group,
									NameList*			funcsAndVars
								);
	static void					ExtractGlobalFunctionsAndVariablesRecursion (
									EditorWindow*		editor, 
									DeltaASTNode*		node,
									PredefinedGroup		group,
									NameList*			funcsAndVars,
									ubag<std::string>*	vars,
									ubag<std::string>*	funcs
								);

	///////////////////////////////////////////////////////////

	public:
	static void 				Initialise (void);
	static void 				CleanUp (void);

	static void					SetLibraryFunctions (
									const SigList&	defs, 
									void*			langModule = (void*) 0	// null means adding stdlib
								);

	static void					HandleLanguageModuleDestruction (void* langModule);
	static void					HandleLanguageModuleInstantiation (void* langModule);
	static void					SetByteCodeLoadingPath (
									void*				langModule, 
									EditorWindow*		editor, 
									DeltaASTNode*		node,
									const std::string& path
								);
	static bool					HasByteCodeLibraryFunction (
									void*				langModule, 
									const std::string&	libName, 
									const std::string&	funcName
								);
	static void					HandleSourceCodeUpdate (	// handles editing changes
									void*				langModule,
									EditorWindow*		editor, 
									DeltaASTNode*		node
								);

	static bool					Handle (
									void*				langModule,
									EditorWindow*		editor, 
									ProgramDescription& progDesc, 
									int					keyCode = 0	// Accelerator.
								);
};

///////////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
