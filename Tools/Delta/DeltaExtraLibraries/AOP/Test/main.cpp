#include "AOPLibrary.h"
#include "CompilerAPI.h"
#include "DeltaCompilerInit.h"
#include "ASTUnparseVisitor.h"
#include "ASTValidationVisitor.h"

static void onerror (const char* error, void*) {
	std::cout << error;
	std::cout.flush();
}

int main(int argc, char** argv) {
	//const char* text = "function f(x) { std::print(x); return; }\nf(1);";
	const char* text = "function add(x, y) { return x + y; }\nfunction id(x) { return x; }\nstd::print(add(1, id(2)), \"\\n\");";
	const std::string pointcut = "execution(function *(..))";
	//const std::string pointcut = "ast(\"Program\")";
	
	const char* beforeAdviceText = "std::print(\"BEFORE\")";
	const char* afterAdviceText = "std::print(\"AFTER\")";
	const char* aroundAdviceText = "std::print(\"AROUND(before)\"); ~proceed; std::print(\"AROUND(after)\");";

	AOPLibrary::AdviceType adviceType = AOPLibrary::AFTER;

	const char* adviceText;
	if (adviceType == AOPLibrary::BEFORE)
		adviceText = beforeAdviceText;
	else if (adviceType == AOPLibrary::AFTER)
		adviceText = afterAdviceText;
	else
		adviceText = aroundAdviceText;

	DeltaCompilerInit::Initialise();
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
	/*
	AOPLibrary::ASTList matches = AOPLibrary::Match(tree, pointcut);
	printf("Nodes matched: %d\n", matches.size());
	unsigned count = 0;
	for (AOPLibrary::ASTList::const_iterator i = matches.begin(); i != matches.end(); ++i, ++count)
		printf("Node %d: %s\n", count, (*i)->GetTag().c_str());
	*/
	DeltaCompilerInit::CleanUp();

	return 0;
}