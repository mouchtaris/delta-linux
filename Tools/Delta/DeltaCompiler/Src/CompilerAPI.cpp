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

#include "CompileOptions.h"
#include "GlobalData.h"
#include "Optimizer.h"
#include "DebugNamingForStaticVars.h"
#include "AutoCollection.h"
#include "DescriptiveParseErrorHandler.h"
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
#include "ASTValidationVisitor.h"
#include "ASTUnparseVisitor.h"
#include "ASTUtilVisitors.h"
#include "usystem.h"

#include "DeltaScanner.h"
#include "DeltaSyntax.h"
#include "DeltaLexAnalyser.h"
#include "DeltaSyntaxParser.h"
#include "ParsingContext.h"

#include "TypeCheck.h"
#include "CompilerStringHolder.h"

#define	TRANSLATE_SYNTAX_TREE

#define AUTOCOLLECTOR	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*COMPONENT_DIRECTORY(), AutoCollector)))

////////////////////////////////////////////////////////

extern int	DeltaCompiler_yydebug;
extern int	DeltaCompiler_yyparse (ParsingContext& ctx);

extern int	DeltaSyntax_yydebug;
extern int	DeltaSyntax_yyparse (ParsingContext& ctx);

////////////////////////////////////////////////////////

static const std::string getTokenTextWrapper(void *closure)
	{ return ((DeltaSyntaxParser*)closure)->GetTokenText(); }

////////////////////////////////////////////////////////

bool DeltaCompiler::IsCompiling (void) const
	{ return isCompiling; }

////////////////////////////////////////////////////////

void DeltaCompiler::SetByteCodePath (const std::string& path)
	{ COMPOPTIONS.SetByteCodePath(path); }

const std::string& DeltaCompiler::GetByteCodePath (void) const
	{ return COMPOPTIONS.GetByteCodePath(); }

////////////////////////////////////////////////////////

void DeltaCompiler::SetProductionMode (bool val)
	{ COMPOPTIONS.SetProductionMode(val); }

bool DeltaCompiler::GetProductionMode (void) const
	{ return COMPOPTIONS.GetProductionMode(); }

////////////////////////////////////////////////////////

void DeltaCompiler::AddExternFuncs (const UPTR(const char*) funcs) {
	if (!externFuncs)
		externFuncs = DNEW(FuncList);
	externFuncs->push_back(funcs);
}

DeltaCompiler::FuncList* DeltaCompiler::GetExternFuncs (void) const
	{ return externFuncs; }

void DeltaCompiler::ClearExternFuncs (void) {
	if (externFuncs) {
		externFuncs->clear();
		DDELETE(externFuncs);
		unullify(externFuncs);
	}
}

////////////////////////////////////////////////////////

static util_ui32 ValidationGetLine (TreeNode* node)
	{ return static_cast<AST::Node*>(node)->GetStartLine(); }

static void ValidationSetLine (util_ui32 line, void *closure)
	{ ((ParseParms*) closure)->SetLine(line); }

AST::ValidationVisitor* DeltaCompiler::NewValidator (bool allowEmptyInlines) const {
	return DNEWCLASS(AST::ValidationVisitor, (
		allowEmptyInlines,
		&ValidationGetLine,
		umakecallback(&ValidationSetLine, &PARSEPARMS)
	));
}

////////////////////////////////////////////////////////

void DeltaCompiler::Initialise (void) {

	DeltaCompiler_yydebug	= 0;
	DeltaSyntax_yydebug		= 0;

	PARSEPARMS.Initialise();
	SELECTIVESTEPIN.Initialise();
	TRANSLATOR.Initialise();

	COMPMESSENGER.ResetErrors();
	COMPMESSENGER.ResetWarnings();
	COMPMESSENGER.SetCurrentFile(COMPOPTIONS.GetSourceFile());

	DELTANAMESPACES.Initialise();
	DELTARETURNTYPES.Initialise();
	if (externFuncs)
		DELTANAMESPACES.Install(*DPTR(externFuncs));
}

////////////////////////////////////////////////////////

void DeltaCompiler::OnParseStarted (bool success, void* closure) {
	DeltaCompiler* compiler = (DeltaCompiler*) closure;
	CompilerComponentDirectory* directory = compiler->COMPONENT_DIRECTORY();
	const char* file = ucstringarg(COMPOPTIONS_EX(directory).GetSourceFile());
	if (!compiler->parseOnly)
		if (success)
			COMPMESSENGER_EX(directory).Msg(DELTA_COMPILER_STARTED_PREFIX " compiling '%s'.\n", file);
		else
			COMPMESSENGER_EX(directory).Msg(DELTA_COMPILER_FAILED_PREFIX " to open file '%s' for compiling.\n", file);
	compiler->sourceSuccessfullyOpened = success;
}

////////////////////////////////////////////////////////

void DeltaCompiler::PreInitialise (void) {
	isCompiling = true;
	if (!phaseCleaned)	
		CleanUp();
	phaseCleaned = false;
}

////////////////////////////////////////////////////////

bool DeltaCompiler::PureSyntaxAnalysis (DeltaSyntaxParser& parser) {
	parser.SetParseStartCallback(umakecallback(&DeltaCompiler::OnParseStarted, this));
	DESCRIPTIVE_ERROR_HANDLER.SetGetTokenText(umakecallback(&getTokenTextWrapper, &parser));

	if (COMPOPTIONS.IsDynamicCode())
		return parser.ParseText(COMPOPTIONS.GetDynamicCode());
	else
		return parser.ParseFile(COMPOPTIONS.GetOriginalSourceFile());
}

////////////////////////////////////////////////////////

bool DeltaCompiler::SyntaxAnalysisAndIntermediateCode (void) {

	GLOBALDATA.Start();

	DeltaLexAnalyserFlexLexer lexer;
	ParsingContext context(lexer);
	context.Register("DeltaCompilerMessenger",			&COMPMESSENGER);
	context.Register("ParseParms",						&PARSEPARMS);
	context.Register("CompilerStringHolder",			&STRINGHOLDER);
	context.Register("DescriptiveParseErrorHandler",	&DESCRIPTIVE_ERROR_HANDLER);
	context.Register("DeltaSymbolTable",				&DELTASYMBOLS);
	context.Register("Translator",						&TRANSLATOR);
	context.Register("DeltaQuadManager",				&QUADS);
	context.Register("DeltaStmtFactory",				&STMTFACTORY);	
	
	if (!PureSyntaxAnalysis(DeltaSyntaxParser(lexer, context, &DeltaCompiler_yyparse)))
		return false;

	if (!COMPMESSENGER.ErrorsExist())
		GLOBALDATA.End();
	return true;
}

////////////////////////////////////////////////////////

class Lexer : public DeltaScannerFlexLexer {
	private:
	std::list<int> pseudoTokens;

	public:
	virtual int	yylex (YYSTYPE* yylval, YYLTYPE* yylloc) {
		if (pseudoTokens.empty())
			return DeltaScannerFlexLexer::yylex(yylval, yylloc);
		else 
			{ int t = pseudoTokens.front(); pseudoTokens.pop_front(); return t; }
	}

	Lexer (const std::list<int>& tokens) : pseudoTokens(tokens){}
};

bool DeltaCompiler::SyntaxAnalysis (const std::list<int>& tokens) {

	Lexer lexer(tokens);
	ParsingContext context(lexer);
	context.Register("DeltaCompilerMessenger",			&COMPMESSENGER);
	context.Register("ParseParms",						&PARSEPARMS);
	context.Register("CompilerStringHolder",			&STRINGHOLDER);
	context.Register("DescriptiveParseErrorHandler",	&DESCRIPTIVE_ERROR_HANDLER);
	context.Register("AST::Creator",					&ASTCREATOR);

	if (!PureSyntaxAnalysis(DeltaSyntaxParser(lexer, context, &DeltaSyntax_yyparse)))
		return false;

	PARSEPARMS.SetLine(1);
	if (!COMPMESSENGER.ErrorsExist()) {
		AST::ValidationVisitor* validator = NewValidator();
		if (!tokens.empty())	//quotedElements
			validator->EnterQuotes();
		bool retval;
		if (!(*validator)(ASTCREATOR.GetSyntaxTree())) {
			COMPMESSENGER.Error("%s.\n", validator->GetValidationError().c_str());
			retval = false;
		}
		else {
			PARSEPARMS.SetLine(1);
			ast = ASTCREATOR.GetSyntaxTree();
			AST::SerialProducer()(ast);
			if (onParse)
				onParse(ast);
			retval = true;
		}
		DDELETE(validator);
		return retval;
	}
	else
		return false;
}

AST::Node* DeltaCompiler::GetSyntaxTree (void) 
	{ return DNULLCHECK(ast); }

////////////////////////////////////////////////////////

bool DeltaCompiler::IntermediateCode (void) {
	if (!COMPMESSENGER.ErrorsExist()) {
		AST::TranslationVisitor visitor;
		INIT_COMPILER_COMPONENT_DIRECTORY(&visitor, COMPONENT_DIRECTORY());
		visitor(GetSyntaxTree());
		return true;
	}
	else
		return false;
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

void DeltaCompiler::InformAboutErrors (void) const {
	const char* srcFile = ucstringarg(COMPOPTIONS.GetSourceFile());

	util_ui32 numErrors = COMPMESSENGER.ErrorsExist();
	if (!numErrors)
		COMPMESSENGER.Msg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', NO ERRORS detected.\n", srcFile);
	else
		COMPMESSENGER.Msg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', %d ERRORS detected.\n", srcFile, numErrors);

	util_ui32 numWarnings = COMPMESSENGER.WarningsExist();
	if (!numWarnings)
		COMPMESSENGER.Msg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', NO WARNINGS reported.\n", srcFile);
	else
		COMPMESSENGER.Msg(DELTA_COMPILER_FINISHED_PREFIX " compiling '%s', %d WARNINGS reported.\n", srcFile, numWarnings);
}

////////////////////////////////////////////////////////

bool DeltaCompiler::PerformFirstPass (void) {
	
	Initialise();

	bool result;
#ifdef	TRANSLATE_SYNTAX_TREE
	result = (!!ast || SyntaxAnalysis()) && IntermediateCode();
#else
	result = SyntaxAnalysisAndIntermediateCode();
#endif

	DASSERT(sourceSuccessfullyOpened || !result);
	if (sourceSuccessfullyOpened)
		InformAboutErrors();

	if (result && !COMPMESSENGER.ErrorsExist()) {
		OptimizationAndTargetCode(); 
		return true;  
	}
	else
		return false;
}

////////////////////////////////////////////////////////

#define COMPILE_CODE						\
	bool result = PerformFirstPass();		\
	isCompiling = false;					\
	return result

#define PARSE_CODE(EXTRA_TOKENS)						\
	Initialise();										\
	parseOnly = true;									\
	bool result = SyntaxAnalysis(EXTRA_TOKENS);			\
	parseOnly = false;									\
	isCompiling = false;								\
	return result ? GetSyntaxTree() : (AST::Node*) 0

#define COMPILE_FUNC_IMPL(INIT_CODE, CODE)	\
	DASSERT(!IsCompiling());				\
	PreInitialise();						\
	INIT_CODE;								\
	CODE

bool DeltaCompiler::Compile (const char* inFile, const char* optSymbolicName)
	{ COMPILE_FUNC_IMPL(COMPOPTIONS.InitialiseForFile(inFile, optSymbolicName), COMPILE_CODE); }

bool DeltaCompiler::CompileText (const char* text)
	{ COMPILE_FUNC_IMPL(COMPOPTIONS.InitialiseForText(text), COMPILE_CODE); }

bool DeltaCompiler::Translate (const TreeNode* ast) {
	DASSERT(!IsCompiling());

	AST::Node* node = (AST::Node*) ast->Clone(ubind1st(umemberfunctionpointer(&AST::Factory::NewNode), &ASTFACTORY));
	AST::ValidationVisitor* validator = NewValidator();
	bool result;
	if (!(*validator)(node)) {
		COMPMESSENGER.Error("%s.\n", validator->GetValidationError().c_str());
		result = false;
	}
	else {
		this->ast = node;
		AST::SerialProducer()(this->ast);

		PreInitialise();
		COMPOPTIONS.InitialiseForText(AST::UnparseVisitor()(this->ast).c_str());
		sourceSuccessfullyOpened = true;
		result = PerformFirstPass();
		isCompiling = false;
	}
	DDELETE(validator);
	return result;
}

AST::Node* DeltaCompiler::Parse (const char* inFile)
	{ COMPILE_FUNC_IMPL(COMPOPTIONS.InitialiseForFile(inFile), PARSE_CODE(UEMPTY)); }

AST::Node* DeltaCompiler::ParseText (const char* text)
	{ COMPILE_FUNC_IMPL(COMPOPTIONS.InitialiseForText(text), PARSE_CODE(UEMPTY)); }

AST::Node* DeltaCompiler::ParseQuotedElements (const char* text)
	{ COMPILE_FUNC_IMPL(COMPOPTIONS.InitialiseForText(text), PARSE_CODE(std::list<int>(1, PARSE_QUOTED_ELEMENTS))); }

const std::string DeltaCompiler::Unparse (const TreeNode* ast) const { return AST::UnparseVisitor().Unparse(ast); }

////////////////////////////////////////////////////////

void DeltaCompiler::DumpUnparsed (const char* file) const {
	if (ast) {
		std::string text = AST::UnparseVisitor()(ast);
		if (FILE* fp = fopen(file, "wt")) {
			fprintf(fp, "%s", text.c_str());
			fclose(fp);
		}		
	}
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpAST (const char* file) const {
	if (ast) {
		std::string text = TreeToStringVisitor(AST::AttributeStringifier())(ast);
		if (FILE* fp = fopen(file, "wt")) {
			fprintf(fp, "%s", text.c_str());
			fclose(fp);
		}		
	}
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpInterCode (const char* file) const {
	DASSERT(!COMPMESSENGER.ErrorsExist());
	QUADS.WriteText(file); 
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpTextCode (const char* file) const {
	DASSERT(!COMPMESSENGER.ErrorsExist());
	CODEGENERATOR.WriteTextCode(file); 
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpBinaryCode (GenericWriter& writer) const {
	DASSERT(!COMPMESSENGER.ErrorsExist());
	CODEGENERATOR.WriteBinaryCode(writer, !COMPOPTIONS.GetProductionMode());
}

////////////////////////////////////////////////////////

void DeltaCompiler::DumpBinaryCode (const char* file) const {

	DASSERT(!COMPMESSENGER.ErrorsExist());
	if (FILE* fp = ubinaryfileopen(file, "w")) {
		CODEGENERATOR.WriteBinaryCode(
			utempobj(PortableBinFileWriter(fp)), 
			!COMPOPTIONS.GetProductionMode()
		);
		fclose(fp);	
	} 
	else
		COMPMESSENGER.Msg(
			DELTA_COMPILER_FAILED_PREFIX " to open file '%s' to write target code (%s)", 
			file, 
			strerror(errno)
		);
}

////////////////////////////////////////////////////////

void* DeltaCompiler::DumpBinaryCode (util_ui32* size) const {

	DASSERT(!COMPMESSENGER.ErrorsExist());
	ubinaryio::OutputBuffer output;
	CODEGENERATOR.WriteBinaryCode(
		utempobj(PortableBufferWriter(output)), 
		!COMPOPTIONS.GetProductionMode()
	);
	return output.MakeBuffer(size);
}

////////////////////////////////////////////////////////

void DeltaCompiler::SetErrorCallback (ErrorCallback callback) {
	DPTR(errorCallbacks)->push(callback);
	COMPMESSENGER.SetErrorCallback(callback);
}

DeltaCompiler::ErrorCallback DeltaCompiler::GetErrorCallback (void) const {
	DASSERT(!DPTR(errorCallbacks)->empty());
	return DPTR(errorCallbacks)->top();
}

void DeltaCompiler::ResetErrorCallback (void) {
	DASSERT(!DPTR(errorCallbacks)->empty());
	DPTR(errorCallbacks)->pop();
	if (DPTR(errorCallbacks)->empty())
		COMPMESSENGER.SetErrorCallback((ErrorCallback) 0);
	else
		COMPMESSENGER.SetErrorCallback(DPTR(errorCallbacks)->top());
}

bool DeltaCompiler::ErrorsExist (void) const { return COMPMESSENGER.ErrorsExist(); }

////////////////////////////////////////////////////////

void DeltaCompiler::SetParseCallback (ParseCallback callback) { onParse = callback; }

DeltaCompiler::ParseCallback DeltaCompiler::GetParseCallback (void) const { return onParse; }

////////////////////////////////////////////////////////

void DeltaCompiler::CleanUp (void) {

	if (phaseCleaned)
		return;

	phaseCleaned				= true;
	sourceSuccessfullyOpened	= false;
	parseOnly					= false;
	ast							= (AST::Node*) 0;

	STRINGHOLDER.commit();
	COMPOPTIONS.Clear();
	TRANSLATOR.CleanUp();
	PARSEPARMS.CleanUp();
	DELTASYMBOLS.CleanUp();
	DELTANAMESPACES.CleanUp();
	DELTARETURNTYPES.CleanUp();
	AUTOCOLLECTOR.CleanUp();
	QUADS.CleanUp();
	CODEGENERATOR.CleanUp();
	DEBUGSTATICS.CleanUp();
	DESCRIPTIVE_ERROR_HANDLER.Clear();
	SELECTIVESTEPIN.CleanUp();
}

////////////////////////////////////////////////////////

DeltaCompiler::DeltaCompiler (void) :
	phaseCleaned			(true),
	sourceSuccessfullyOpened(false),
	parseOnly				(false),
	externFuncs				((DeltaCompiler::FuncList*) 0),
	errorCallbacks			((std::stack<ErrorCallback>*) 0),
	isCompiling				(false),
	ast						((AST::Node*) 0)
{

	INIT_COMPILER_COMPONENT_DIRECTORY(this, DNEW(CompilerComponentDirectory));

#define CREATE_COMPONENT(type)	\
	COMPONENT_DIRECTORY()->Register(#type, DNEW(type));
	CREATE_COMPONENT(CompileOptions);
	CREATE_COMPONENT(CompilerStringHolder);
	CREATE_COMPONENT(LocalDataHandler);
	CREATE_COMPONENT(DebugNamingForStaticVars);
	CREATE_COMPONENT(GlobalData);
	CREATE_COMPONENT(DeltaSymbolTable);
	CREATE_COMPONENT(Optimizer);
	CREATE_COMPONENT(DeltaCodeGenerator);
	CREATE_COMPONENT(DeltaQuadManager);
	CREATE_COMPONENT(DeltaLibraryNamespaceHolder);
	CREATE_COMPONENT(DeltaFunctionReturnTypesManager);
	CREATE_COMPONENT(AutoCollector);
	CREATE_COMPONENT(ParseParms);
	CREATE_COMPONENT(DeltaExprFactory);
	CREATE_COMPONENT(DeltaStmtFactory);
	CREATE_COMPONENT(AST::Factory);
	CREATE_COMPONENT(Translator);
	CREATE_COMPONENT(TypeChecker);
	CREATE_COMPONENT(FunctionValueReturnChecker);
	CREATE_COMPONENT(SelectiveStepInPreparator);
	CREATE_COMPONENT(AST::Creator);
	CREATE_COMPONENT(DescriptiveParseErrorHandler);
	CREATE_COMPONENT(DeltaCompilerMessenger);

#define INIT_COMPONENT(c)	\
	INIT_COMPILER_COMPONENT_DIRECTORY(c, COMPONENT_DIRECTORY());

	INIT_COMPONENT(&LOCALDATA);
	INIT_COMPONENT(&GLOBALDATA);
	INIT_COMPONENT(&DELTASYMBOLS);
	INIT_COMPONENT(&OPTIMIZER);
	INIT_COMPONENT(&CODEGENERATOR);
	INIT_COMPONENT(&QUADS);
	INIT_COMPONENT(&DELTANAMESPACES);
	INIT_COMPONENT(&DELTARETURNTYPES);
	INIT_COMPONENT(&EXPRFACTORY);
	INIT_COMPONENT(&TYPECHECKER);
	INIT_COMPONENT(&TRANSLATOR);
	INIT_COMPONENT(&RETVALCHECKER);
	INIT_COMPONENT(&SELECTIVESTEPIN);
	INIT_COMPONENT(&ASTCREATOR);
	INIT_COMPONENT(&DESCRIPTIVE_ERROR_HANDLER);
	INIT_COMPONENT(&COMPMESSENGER);
	
	AutoCollector* autoCollector = &AUTOCOLLECTOR;
	EXPRFACTORY.SetAutoCollector(autoCollector);
	STMTFACTORY.SetAutoCollector(autoCollector);
	ASTFACTORY.SetAutoCollector(autoCollector);

	unew(errorCallbacks);
	DESCRIPTIVE_ERROR_HANDLER.Initialise();
}

////////////////////////////////////////////////////////

DeltaCompiler::~DeltaCompiler () {

	CleanUp();
	ClearExternFuncs();
	DESCRIPTIVE_ERROR_HANDLER.CleanUp();
	udelete(errorCallbacks);

	DDELETE(&COMPOPTIONS);
	DDELETE(&STRINGHOLDER);
	DDELETE(&LOCALDATA);
	DDELETE(&DEBUGSTATICS);
	DDELETE(&GLOBALDATA);
	DDELETE(&DELTASYMBOLS);
	DDELETE(&OPTIMIZER);
	DDELETE(&CODEGENERATOR);
	DDELETE(&QUADS);
	DDELETE(&DELTANAMESPACES);
	DDELETE(&DELTARETURNTYPES);
	DDELETE(&AUTOCOLLECTOR);
	DDELETE(&PARSEPARMS);
	DDELETE(&EXPRFACTORY);
	DDELETE(&STMTFACTORY);
	DDELETE(&ASTFACTORY);
	DDELETE(&TYPECHECKER);
	DDELETE(&TRANSLATOR);
	DDELETE(&RETVALCHECKER);
	DDELETE(&SELECTIVESTEPIN);
	DDELETE(&ASTCREATOR);
	DDELETE(&DESCRIPTIVE_ERROR_HANDLER);
	DDELETE(&COMPMESSENGER);
	DDELETE(COMPONENT_DIRECTORY());
}