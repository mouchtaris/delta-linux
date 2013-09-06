#include "AOPLibrary.h"
#include "CompilerAPI.h"
#include "DeltaCompilerInit.h"
#include "ASTUnparseVisitor.h"
#include "ASTValidationVisitor.h"

static void onerror (const char* error, void*) {
	std::cout << error;
	std::cout.flush();
}

#define FUNCTION			1
#define OBJECT_CONSTRUCTOR	2
#define CLASS1				3
#define CLASS2				4
#define PROGRAM				5

#define TEST PROGRAM

int main(int argc, char** argv) {
#if TEST == FUNCTION
	const char* text = "function add(x, y) { return x + y; }\nfunction id(x) { return x; }\nstd::print(add(1, id(2)), \"\\n\");";
	const std::string pointcut = "execution(function *(*))";
	const char* beforeAdviceText = "std::print(\"BEFORE\")";
	const char* afterAdviceText = "std::print(\"AFTER\")";
	const char* aroundAdviceText = "std::print(\"AROUND(before)\"); ~proceed; std::print(\"AROUND(after)\");";
#elif TEST == OBJECT_CONSTRUCTOR
	const char* text = "function Point(x, y) { return [ @x : x, @y : y ]; }";
	const std::string pointcut = "ast(\"TableConstructor\")";
	const char* beforeAdviceText = "method before{}";
	const char* afterAdviceText = "method after{}";
	const char* aroundAdviceText = "[]";
#elif TEST == CLASS1
	const char* text = "function RemoteObject() { return [ method m() { throw 123; } ]; }";
	const std::string pointcut = "execution(method *(..)) and descendant(class(RemoteObject))";
	const char* beforeAdviceText = "";
	const char* afterAdviceText = "";
	const char* aroundAdviceText = "try { ~proceed; } trap e { std::print(e); }";
#elif TEST == CLASS2
	const char* text = "function SharedObject() { return [ method m() { std::print(123); } ]; }";
	const std::string pointcut = "descendant(class(SharedObject)) and execution(method *(..))";
	const char* beforeAdviceText = "@mutex.lock();";
	const char* afterAdviceText = "@mutex.unlock();";
	const char* aroundAdviceText = "@mutex.lock(); ~proceed; @mutex.unlock();";
#elif TEST == PROGRAM
	const char* text = "function f() {}";
	const std::string pointcut = "ast(\"FunctionStmt\")";
	const char* beforeAdviceText = "std::print(123);";
	const char* afterAdviceText = "std::print(~retval)";
	const char* aroundAdviceText = "&~proceed;";
#endif

	AOPLibrary::AdviceType adviceType = AOPLibrary::AROUND;

	const char* adviceText;
	if (adviceType == AOPLibrary::BEFORE)
		adviceText = beforeAdviceText;
	else if (adviceType == AOPLibrary::AFTER)
		adviceText = afterAdviceText;
	else
		adviceText = aroundAdviceText;

	DeltaCompilerInit::Initialise();
	AOPLibrary::Initialise();
	DeltaCompiler* compiler = DNEW(DeltaCompiler);
	DPTR(compiler)->SetErrorCallback(onerror);
	TreeNode* ast = ((TreeNode*)DPTR(compiler)->ParseText(text))->Clone(TreeNode::DefaultCreator());
	DPTR(compiler)->CleanUp();
	TreeNode* advice = ((TreeNode*)DPTR(compiler)->ParseQuotedElements(adviceText))->Clone(TreeNode::DefaultCreator());

	AOPLibrary::Aspect(ast, pointcut, adviceType, advice);
	TreeNode::Delete(advice);

	AST::ValidationVisitor validator;
	if (!validator(ast))
		printf("Validation error: %s\n", validator.GetValidationError().c_str());
	else {
		const std::string result = DPTR(compiler)->Unparse(ast);
		printf("%s\n", result.c_str());
	}
	TreeNode::Delete(ast);
	DDELETE(compiler);

	AOPLibrary::CleanUp();
	DeltaCompilerInit::CleanUp();

	return 0;
}