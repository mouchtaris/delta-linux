std::dllimportdeltalib(aop::DLL);
function transform (ast) {
	local pointcut = "execution(function fibonacci(n))";
	local beforeAdvice = <<
		static memoizer = [];
		if (memoizer[n] != nil) return memoizer[n];
	>>;
	local afterAdvice =  <<memoizer[n] = ~~retval;>>;
	aop::aspect(ast, pointcut, aop::BEFORE, beforeAdvice);
	aop::aspect(ast, pointcut, aop::AFTER,  afterAdvice);
	return ast;
}
