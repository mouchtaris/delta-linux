using #utility;

std::dllimportdeltalib(aop::DLL);

function transform (ast) {
	local funcs = aop::match(ast, "ast(\"Function\")");	//match all functions
	foreach(local func, funcs) {
		local name = utility::GetFuncName(func);	//insert staged call to analyze matched function
		aop::advise(func, aop::AFTER, <<&analyze(compiler_get_function_ast(~name));>>);
	}
	aop::advise(ast, aop::BEFORE, <<
		&function compiler_get_function_ast (func) { std::print("Retrieving ast for function ", func, "\n"); return func; }
		&function analyze (func) { std::print("Analyzing func ", func, "\n"); }
	>>);	//insert staged defs
	return ast;
}