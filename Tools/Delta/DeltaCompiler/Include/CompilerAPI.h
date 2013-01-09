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
#include "DeltaCompilerDefs.h"
#include "ASTNode.h"
#include "CompilerComponentDirectory.h"
#include "ParserWrapper.h"
#include <list>
#include <string>
#include <stack>

///////////////////////////////////////////////////////////////

class GenericWriter;

namespace AST {
	class ValidationVisitor;
}

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaCompiler {

	public:
	typedef std::list<UPTR(const char*)> FuncList;
	typedef DeltaCompilerMessenger::ErrorCallback ErrorCallback;
	typedef ucallbackwithclosure<void (*)(AST::Node*, void*)> ParseCallback;

	protected:
	ucomponentdirectory*		comps;

	//Apart from owning the directory, it also a client, so emulate its get functionality.
	public:
	ucomponentdirectory*		GET_COMPONENT_DIRECTORY() { return comps; }
	const ucomponentdirectory*	GET_COMPONENT_DIRECTORY() const { return comps; }

	private:

	///////////////////////////////////////////////////////////////

	bool						phaseCleaned;
	bool						sourceSuccessfullyOpened;
	bool						parseOnly;
	FuncList*					externFuncs;
	std::stack<ErrorCallback>*	errorCallbacks;
	ParseCallback				onParse;
	bool						isCompiling;		// Reetrancy check.
	AST::Node*					ast;

	static void			OnParseStarted	(bool success, void* closure);

	void				PreInitialise (void);
	
	template<typename Lexer>
	bool				PureSyntaxAnalysis (ParserWrapper<Lexer>& parser);

	bool				SyntaxAnalysis (const std::list<int>& tokens = std::list<int>());
	bool				SyntaxAnalysisAndIntermediateCode (void);

	bool				PerformFirstPass (void);
	void				OptimizationAndTargetCode (void);
	void				InformAboutErrors (void) const;

	protected:
	virtual void		Initialise (void);
	virtual bool		IntermediateCode (void);

	AST::ValidationVisitor* NewValidator (bool allowEmptyInlines = false) const;

	///////////////////////////////////////////////////////////////

	public:
	void 				SetErrorCallback (ErrorCallback callback);
	ErrorCallback		GetErrorCallback (void) const;
	void 				ResetErrorCallback (void);
	bool				ErrorsExist (void) const;

	void				SetParseCallback (ParseCallback callback);
	ParseCallback		GetParseCallback (void) const;

	bool				IsCompiling (void) const;
	void				SetByteCodePath (const std::string& path);	// Must be set with every compilation.
	const std::string&	GetByteCodePath (void) const;

	// Compilation.
	void				AddExternFuncs (UPTR(const char*) funcs);
	FuncList*			GetExternFuncs (void) const;
	void				ClearExternFuncs (void);

	bool				Compile (const char* inFile, const char* optSymbolicName = (const char*) 0);
	bool				CompileText (const char* text);
	bool				Translate (const TreeNode* ast);

	AST::Node*			Parse (const char* inFile);
	AST::Node*			ParseText (const char* text);
	AST::Node*			ParseQuotedElements (const char* text);

	const std::string	Unparse (const TreeNode* ast) const;

	AST::Node*			GetSyntaxTree (void);

	// Compile flags.
	void				SetProductionMode (bool val);
	bool				GetProductionMode (void) const;

	// Code emission.
	void 				DumpUnparsed (const char* file) const;
	void 				DumpAST (const char* file) const;
	void 				DumpInterCode (const char* file) const;
	void 				DumpTextCode (const char* file) const;
	void				DumpBinaryCode (GenericWriter& writer) const;
	void 				DumpBinaryCode (const char*	file) const;	// To secondary storage directly.
	void* 				DumpBinaryCode (util_ui32* size) const;		// To a dynamic (util_ui8 array) buffer.
	
	void 				CleanUp (void);

	DeltaCompiler (void);
	~DeltaCompiler ();
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.