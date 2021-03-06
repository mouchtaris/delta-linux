using aop;

std::dllimportdeltalib(DLL);	//import AOP library

function transform(ast) {
	local validator = << function validate(ast) { std::print("Validating ast\n");/*validation checks go here*/ return ast; } >>;
	advise(ast, BEFORE, validator); //introduce the validate function

	local advice = <<validate(~~proceed)>>;
	//the following statements turn <<... ~x ...>> into validate(<<... ~(validate(x)) ...>>)
	aspect(ast, "ast(\"QuasiQuotes\")",	AROUND, advice);
	aspect(ast, "child(ast(\"Escape\"))",AROUND, advice);

	//turn !(...) into !(validate(...))
	aspect(ast,"child(ast(\"Inline\"))", AROUND, advice);

	//validate all functions (except validate) whose first arg is an ast
	aspect(ast, "execution(function *(ast ..)) and not execution(function validate(..))", BEFORE, <<validate(ast);>>); // validate AST arguments
	return ast;
}
