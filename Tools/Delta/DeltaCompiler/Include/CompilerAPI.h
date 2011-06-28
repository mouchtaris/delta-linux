// CompilerAPI.h
// Delta compiler API.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	COMPILERAPI_H
#define	COMPILERAPI_H

#include "DDebug.h"
#include "utypes.h"
#include "DeltaCompErrorMsg.h"
#include "GenericWriter.h"
#include "DeltaCompilerDefs.h"
#include "ASTNode.h"
#include <list>
#include <string>
#include <stack>

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaCompiler {

	private:

	///////////////////////////////////////////////////////////////

	static bool									phaseCleaned;
	static std::list<UPTR(const char*)>*		externFuncs;
	static std::string*							byteCodePath;
	static std::string*							srcFile;
	static std::stack<DeltaCompErrorCallback>*	errorCallbacks;
	static bool									isCompiling;		// Reetrancy check.
	static bool									isDynamicCode;		// If compiling from text string.
	static std::string*							dynamicCode;
	static util_ui32							currLine;
	static AST::Node*							ast;

	struct DCOMPLIB_CLASS CompileFlags {
		bool		productionMode;					// FALSE
	};

	static CompileFlags	compileFlags;

	static bool					InitialiseForFile (
									FILE*&		fp, 
									void		(*reset)(FILE*),
									const char* inFile, 
									const char* optSymbolicName
								);

	static bool					InitialiseForFileAST (const char* inFile, const char* optSymbolicName);
	static bool					InitialiseForTextAST (const char* text);
	static void					CleanUpForTextAST (void);

	static bool					InitialiseForFile (const char* inFile, const char* optSymbolicName);
	static bool					InitialiseForText (const char* text);
	static void					CleanUpForText (void);

	static void					PreInitialise (void);
	static void					Initialise (void);

	static bool					SyntaxAnalysis (void);
	static void					IntermediateCode (void);

	static bool					PerformFirstPass (void);
	static void					SyntaxAnlysisAndIntermediateCode (void);
	static void					OptimizationAndTargetCode (void);
	static void					InformAboutErrors (void);

	///////////////////////////////////////////////////////////////

	public:
	static void 				SetErrorCallback (void (*callback)(const char*));
	static void 				ResetErrorCallback (void);

	static util_ui32			GetLine (void);
	static util_ui32			SetLine (util_ui32 line);
	static void					NextLine (void);

	static const char*			GetSourceFile (void);
	static bool					IsCompiling (void);
	static bool					IsDynamicCode (void);
	static const std::string&	GetDynamicCode (void);
	static void					SetByteCodePath (const std::string& path);	// Must be set with every compilation.
	static const std::string&	GetByteCodePath (void);

	// Compilation.
	static void					AddExternFuncs (UPTR(const char*) funcs);
	static void					ClearExternFuncs (void);
	static bool					Compile (const char* inFile, const char* optSymbolicName = (char*) 0);
	static bool					CompileText (const char* text);
	static AST::Node*			GetSyntaxTree (void);

	// Compile flags.
	static void					SetProductionMode (bool val);
	static bool					GetProductionMode (void);

	// Code emission.
	static void 				DumpUnparsed (const char* file);
	static void 				DumpAST (const char* file);
	static void 				DumpInterCode (const char* file);
	static void 				DumpTextCode (const char* file);
	static void					DumpBinaryCode (GenericWriter& writer);
	static void 				DumpBinaryCode (const char*	file);		// To secondary storage directly.
	static void* 				DumpBinaryCode (util_ui32* size);		// To a dynamic (util_ui8 array) buffer.
	
	static void 				CleanUp (void);
	static void 				SingletonCreate (void);
	static void 				SingletonDestroy (void);
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.