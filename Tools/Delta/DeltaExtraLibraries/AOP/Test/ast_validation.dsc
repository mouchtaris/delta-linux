std::dllimportdeltalib(aop::DLL);

function transform(ast) {
	local validator = << function validate(ast) { /*validation checks go here*/ return ast; } >>;
	aop::advise(ast, BEFORE, validator); //introduce the validate function

	local advice = <<validate(~~proceed)>>;
	//the following statements turn <<... ~x ...>> into validate(<<... ~(validate(x)) ...>>)
	aop::aspect(ast, "ast(QuasiQuotes)",	aop::AROUND, advice);
	aop::aspect(ast, "child(Escape)",		aop::AROUND, advice);

	//turn !(...) into !(validate(...))
	aop::aspect(ast,"child(Inline)", aop::AROUND, advice);

	//validate all functions (except validate) whose first arg is an ast
	aop::aspect(ast, "execution(function *(ast,..)) and not execution(function validate(..)", aop::BEFORE, <<validate(ast);>>); // validate AST arguments
	return ast;
}
