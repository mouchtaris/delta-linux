using aop;
using #utility;

std::dllimportdeltalib(aop::DLL);	//import AOP library

function transform (ast) {
	local funcs = match(ast, "ast(\"Function\")");	//match all functions
	foreach(local func, funcs) {
		local name = utility::GetName(func);	//insert staged call to analyze matched function
		advise(func, AFTER, <<&analyze(compiler_get_function_ast(~name));>>);
	}
	advise(ast, BEFORE, <<
		//The first function is just for presentation reasons to avoid
		//compicating the example with with calls to the compiler.
		&function compiler_get_function_ast (func) { std::print("Retrieving ast for function ", func, "\n"); return func; }
		&function analyze (func) { std::print("Analyzing func ", func, "\n"); }
	>>);	//insert staged defs
	return ast;
}