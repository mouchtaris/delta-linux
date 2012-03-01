// CompilerAPI.cpp
// Delta compiler API.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
// Most recent update, September 2007, removed C-preprocessor
// support, added compile flags.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "CompilerAPI.h"
#include "Symbol.h"
#include "ParseParms.h"
#include "InterCode.h"
#include "TargetCode.h"
#include "DeltaCompErrorMsg.h"

#include "GlobalData.h"
#include "Optimizer.h"
#include "DebugNamingForStaticVars.h"
#include "AutoCollection.h"
#include "DescriptiveParseErrorHandler.h"
#include "Unparse.h"
#include "ufiles.h"
#include "SelectiveStepInPreparator.h"
#include "GenericWriter.h"
#include "FunctionValueReturnChecker.h"
#include "LibraryNamespaceHolder.h"
#include "FunctionReturnTypesManager.h"
#include "ParseActions.h"
#include "ASTCreationActions.h"
#include "TreeToStringVisitor.h"
#include "ASTAttributeStringifier.h"
#include "ASTTranslationVisitor.h"
#include "ASTUnparseVisitor.h"
#include "usystem.h"

#define	TRANSLATE_SYNTAX_TREE

////////////////////////////////////////////////////////

extern int							DeltaCompiler_yydebug;
extern FILE*						DeltaCompiler_yyin;
extern void							DeltaCompiler_ResetLex (FILE* fp);
extern int							DeltaCompiler_yyparse (void);

extern int							DeltaSyntax_yydebug;
extern FILE*						DeltaSyntax_yyin;
extern void							DeltaSyntax_ResetLex (FILE* fp);
extern int							DeltaSyntax_yyparse (void);

////////////////////////////////////////////////////////

bool								DeltaCompiler::phaseCleaned		= true;
std::list<UPTR(const char*)>*		DeltaCompiler::externFuncs		= (std::list<UPTR(const char*)>*) 0;
std::string*						DeltaCompiler::byteCodePath		= (std::string*) 0;
std::string*						DeltaCompiler::srcFile			= (std::string*) 0;
std::stack<DeltaCompErrorCallback>*	DeltaCompiler::errorCallbacks	= (std::stack<DeltaCompErrorCallback>*) 0;
bool								DeltaCompiler::isCompiling		= false;
bool								DeltaCompiler::isDynamicCode	= false;
std::string*						DeltaCompiler::dynamicCode		= (std::string*) 0;
util_ui32							DeltaCompiler::currLine			= 1;
AST::Node*							DeltaCompiler::ast				= (AST::Node*) 0;

DeltaCompiler::CompileFlags	DeltaCompiler::compileFlags = { false };

////////////////////////////////////////////////////////

util_ui32 DeltaCompiler::GetLine (void) 
	{ return currLine; }

util_ui32 DeltaCompiler::SetLine (util_ui32 line) 
	{ return currLine = line; }

void DeltaCompiler::NextLine (void) 
	{ ++currLine; }

const char*	DeltaCompiler::GetSourceFile (void) 
	{ DASSERT(srcFile); return DPTR(srcFile)->c_str(); }

void DeltaCompiler::SetByteCodePath (const std::string& path) {
	*DPTR(byteCodePath) =  ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH);
	if (!DPTR(byteCodePath)->empty())
		DPTR(byteCodePath)->append(";");
	DPTR(byteCodePath)->append(path);
}

const std::string& DeltaCompiler::GetByteCodePath (void)
	{ return *DPTR(byteCodePath); }

bool DeltaCompiler::IsCompiling (void) 
	{ return isCompiling; }

bool DeltaCompiler::IsDynamicCode (void) 
	{ return isDynamicCode; }

const std::string& DeltaCompiler::GetDynamicCode (void)
	{ return *DPTR(dynamicCode); }

void DeltaCompiler::SetProductionMode (bool val)	
	{ compileFlags.productionMode = val; }

bool DeltaCompiler::GetProductionMode (void)		
	{ return compileFlags.productionMode; }

////////////////////////////////////////////////////////

void  DeltaCompiler::AddExternFuncs (const UPTR(const char*) funcs) {
	if (!externFuncs)
		externFuncs = DNEW(std::list<UPTR(const char*)>);
	externFuncs->push_back(funcs);
}

void DeltaCompiler::ClearExternFuncs (void) {
	if (externFuncs) {
		externFuncs->clear();
		DDELETE(externFuncs);
		unullify(externFuncs);
	}
}

////////////////////////////////////////////////////////

bool DeltaCompiler::InitialiseForFile (
		FILE*&		fp, 
		void		(*reset)(FILE*),
		const char* inFile, 
		const char* optSymbolicName
	) {
	if ((fp = fopen(inFile, "rt"))) {
		DPTR(dynamicCode)->clear();
		isDynamicCode = false;
		*DPTR(srcFile) = !optSymbolicName? inFile : optSymbolicName;
		(*reset)(fp);
		return true;
	}
	else {
		DeltaCompMsg(DELTA_COMPILER_FAILED_PREFIX " to open file '%s' for compiling.\n", inFile);
		return false;
	}
}

bool DeltaCompiler::InitialiseForFile (const char* inFile, const char* optSymbolicName) 
	{ return InitialiseForFile(DeltaCompiler_yyin, &DeltaCompiler_ResetLex, inFile, optSymbolicName); }

bool DeltaCompiler::InitialiseForFileAST (const char* inFile, const char* optSymbolicName) 
	{ return InitialiseForFile(DeltaSyntax_yyin, &DeltaSyntax_ResetLex, inFile, optSymbolicName); }

////////////////////////////////////////////////////////

void DeltaCompiler::Initialise (void) {

	DeltaCompiler_yydebug	= 1;
	DeltaSyntax_yydebug		= 0;
	currLine = 1;	// Set first file line.

	ParseParms::Initialise();
	SelectiveStepInPreparator::Initialise();
	ParseParms::SetInProductionMode(compileFlags.productionMode);

	ParseActions_Initialise();

	DeltaCompResetErrors();
	DeltaCompResetWarnings();
	DeltaCompSetCurrentFile(DPTR(srcFile)->c_str());

	DELTANAMESPACES.Initialise();
	DELTARETURNTYPES.Initialise();
	DELTANAMESPACES.Install(*DPTR(externFuncs));
	AST::Initialise();
}

////////////////////////////////////////////////////////

void DeltaCompiler::InformAboutErrors (void) {
	util_ui32 numErrors = DeltaCompErrorsExist();
	if (!numErrors)
		DeltaCompMsg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', NO ERRORS detected.\n", DPTR(srcFile)->c_str());
	else
		DeltaCompMsg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', %d ERRORS detected.\n", DPTR(srcFile)->c_str(), numErrors);

	util_ui32 numWarnings = DeltaCompWarningsExist();
	if (!numWarnings)
		DeltaCompMsg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', NO WARNINGS reported.\n", DPTR(srcFile)->c_str());
	else
		DeltaCompMsg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', %d WARNINGS reported.\n", DPTR(srcFile)->c_str(), numWarnings);
}

////////////////////////////////////////////////////////

void DeltaCompiler::SyntaxAnlysisAndIntermediateCode (void) {

	DeltaCompMsg(DELTA_COMPILER_STARTED_PREFIX " compiling '%s'.\n", DPTR(srcFile)->c_str());

	GlobalData::Start();
	
	DeltaCompiler_yyparse();

	if (!IsDynamicCode()) {
		DASSERT(DeltaCompiler_yyin);
		fclose(DeltaCompiler_yyin);
		unullify(DeltaCompiler_yyin);
	}
	if (!DeltaCompErrorsExist())
		GlobalData::End();

	InformAboutErrors();
}

////////////////////////////////////////////////////////

void DeltaCompiler::IntermediateCode (void) {
	if (!DeltaCompErrorsExist())
		AST::TranslationVisitor()(GetSyntaxTree());
}

////////////////////////////////////////////////////////

void DeltaCompiler::OptimizationAndTargetCode (void) {

	QUADS.BeforeOptimizationValidation();
	OPTIMIZER.Optimize();
	QUADS.AfterOptimizationValidation();

	DEBUGSTATICS.ProduceDebugNamesForStaticVars();
	CODEGENERATOR.GenerateCode();
}

////////////////////////////////////////////////////////

bool DeltaCompiler::SyntaxAnalysis (void) {

	DeltaCompMsg(DELTA_COMPILER_STARTED_PREFIX " compiling '%s'.\n", DPTR(srcFile)->c_str());

	DeltaSyntax_yyparse();

	if (!IsDynamicCode()) {
		DASSERT(DeltaSyntax_yyin);
		fclose(DeltaSyntax_yyin);
		unullify(DeltaSyntax_yyin);
	}

	currLine = 1;
	if (!DeltaCompErrorsExist())
		{ ast = AST::GetSyntaxTree(); return true; }
	else
		return false;
}

AST::Node* DeltaCompiler::GetSyntaxTree (void) 
	{ return DNULLCHECK(ast); }

////////////////////////////////////////////////////////

bool DeltaCompiler::PerformFirstPass (void) {

	Initialise();

#ifdef	TRANSLATE_SYNTAX_TREE
	if (SyntaxAnalysis())
		IntermediateCode();
	InformAboutErrors();
#else
	SyntaxAnlysisAndIntermediateCode();
#endif

	if (!DeltaCompErrorsExist()) {
		OptimizationAndTargetCode(); 
		return true;  
	}
	else
		return false;
}

////////////////////////////////////////////////////////

void DeltaCompiler::PreInitialise (void) {
	isCompiling = true;
	if (!phaseCleaned)	
		CleanUp();
	phaseCleaned = false;
}

////////////////////////////////////////////////////////

bool DeltaCompiler::CompileText (const char* text) {

	DASSERT(!IsCompiling());
	
	bool (*init)(const char*);
	unullify(init);

#ifdef	TRANSLATE_SYNTAX_TREE
	init = &InitialiseForTextAST;
#else
	init = &InitialiseForText;
#endif

	PreInitialise();
	bool result = (*init)(text) &&  PerformFirstPass();
	isCompiling = false;
	return result;
}

////////////////////////////////////////////////////////

bool DeltaCompiler::Compile (const char* inFile, const char* optSymbolicName) {

	DASSERT(!IsCompiling());

	bool (*init)(const char*, const char*);
	unullify(init);

#ifdef	TRANSLATE_SYNTAX_TREE
	init = &InitialiseForFileAST;
#else
	init = &InitialiseForFile;
#endif

	PreInitialise();
	bool result = (*init)(inFile, optSymbolicName) && PerformFirstPass();
	isCompiling = false;
	return result;
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpUnparsed (const char* file) {
	if (ast) {
		std::string text = AST::UnparseVisitor()(ast);
		if (FILE* fp = fopen(file, "wt")) {
			fprintf(fp, "%s", text.c_str());
			fclose(fp);
		}		
	}
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpAST (const char* file) {
	if (ast) {
		std::string text = TreeToStringVisitor(AST::AttributeStringifier())(ast);
		if (FILE* fp = fopen(file, "wt")) {
			fprintf(fp, "%s", text.c_str());
			fclose(fp);
		}		
	}
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpInterCode (const char* file) {
	DASSERT(!DeltaCompErrorsExist());
	QUADS.WriteText(file); 
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpTextCode (const char* file) {
	DASSERT(!DeltaCompErrorsExist());
	CODEGENERATOR.WriteTextCode(file); 
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpBinaryCode (GenericWriter& writer) {
	DASSERT(!DeltaCompErrorsExist());
	CODEGENERATOR.WriteBinaryCode(writer, !compileFlags.productionMode);
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpBinaryCode (const char* file) {

	DASSERT(!DeltaCompErrorsExist());
	if (FILE* fp = ubinaryfileopen(file, "w")) {
		CODEGENERATOR.WriteBinaryCode(
			utempobj(PortableBinFileWriter(fp)), 
			!compileFlags.productionMode
		);
		fclose(fp);	
	} 
	else
		DeltaCompMsg(
			DELTA_COMPILER_FAILED_PREFIX " to open file '%s' to write target code (%s)", 
			file, 
			strerror(errno)
		);
}

////////////////////////////////////////////////////////

void* DeltaCompiler::DumpBinaryCode (util_ui32* size) {

	DASSERT(!DeltaCompErrorsExist());
	ubinaryio::OutputBuffer output;
	CODEGENERATOR.WriteBinaryCode(
		utempobj(PortableBufferWriter(output)), 
		!compileFlags.productionMode
	);
	return output.MakeBuffer(size);
}

////////////////////////////////////////////////////////

void DeltaCompiler::SetErrorCallback (void (*callback)(const char*)) {
	DPTR(errorCallbacks)->push(callback);
	DeltaCompSetErrorCallback(callback);
}

void DeltaCompiler::ResetErrorCallback (void) {
	DASSERT(!DPTR(errorCallbacks)->empty());
	DPTR(errorCallbacks)->pop();
	if (DPTR(errorCallbacks)->empty())
		DeltaCompSetErrorCallback((DeltaCompErrorCallback) 0);
	else
		DeltaCompSetErrorCallback(DPTR(errorCallbacks)->top());
}

////////////////////////////////////////////////////////

extern const std::string DeltaCompiler_GetText (void);
extern const std::string DeltaSyntax_GetText (void);

void DeltaCompiler::SingletonCreate (void) {

	DescriptiveParseErrorHandler::Initialise();

#ifdef	TRANSLATE_SYNTAX_TREE
	DescriptiveParseErrorHandler::SetGetTokenText(&DeltaSyntax_GetText);
#else
	DescriptiveParseErrorHandler::SetGetTokenText(&DeltaCompiler_GetText);
#endif
	
	unew(srcFile);
	unew(dynamicCode);
	unew(errorCallbacks);
	unew(byteCodePath);
	*byteCodePath = ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH);

	Unparse_SingletonCreate();
	SelectiveStepInPreparator::SingletonCreate();
	ParseActions_SingletonCreate();
	AST::SingletonCreate();
	FunctionValueReturnChecker::SingletonCreate();
}

////////////////////////////////////////////////////////

void DeltaCompiler::SingletonDestroy (void) {

	CleanUp();
	ClearExternFuncs();
	DescriptiveParseErrorHandler::CleanUp();
	udelete(srcFile);
	udelete(dynamicCode);
	udelete(errorCallbacks);
	udelete(byteCodePath);
	Unparse_SingletonDestroy();
	SelectiveStepInPreparator::SingletonDestroy();
	ParseActions_SingletonDestroy();
	AST::SingletonDestroy();
	FunctionValueReturnChecker::SingletonDestroy();
}

////////////////////////////////////////////////////////

void DeltaCompiler::CleanUp (void) {

	if (phaseCleaned)
		return;

	phaseCleaned = true;
	DPTR(byteCodePath)->clear();
	CleanUpForText();
	CleanUpForTextAST();

	ParseActions_CleanUp();
	AST::CleanUp();
	ParseParms::CleanUp();
	DELTASYMBOLS.CleanUp();
	DELTANAMESPACES.CleanUp();
	DELTARETURNTYPES.CleanUp();
	AUTOCOLLECTOR.CleanUp();
	QUADS.CleanUp();
	CODEGENERATOR.CleanUp();
	DEBUGSTATICS.CleanUp();
	DescriptiveParseErrorHandler::Clear();
	SelectiveStepInPreparator::CleanUp();
}

////////////////////////////////////////////////////////
