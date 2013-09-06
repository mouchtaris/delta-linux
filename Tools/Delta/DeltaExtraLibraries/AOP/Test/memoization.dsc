using aop;

std::dllimportdeltalib(aop::DLL);	//import AOP library

function transform (ast) {
	local pointcut = "execution(function fibonacci(n))";
	local beforeAdvice = <<
		static memoizer = [];
		if (memoizer[n] != nil) return memoizer[n];
	>>;
	local afterAdvice = <<memoizer[n] = ~~retval;>>;
	aspect(ast, pointcut, AFTER,  afterAdvice);
	//put the before advice second to avoid advising the return present in it
	aspect(ast, pointcut, BEFORE, beforeAdvice);
	return ast;
}
