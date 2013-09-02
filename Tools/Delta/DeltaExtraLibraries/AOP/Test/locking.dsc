std::dllimportdeltalib(aop::DLL);

function transform (ast) {
	local class = "class(SharedObject)"; //class for synchronization
	local pointcut = "descendant(" + class + ") and execution(method *(..))";
	aop::aspect(ast, pointcut,	aop::BEFORE, <<@mutex.lock()>>);
	aop::aspect(ast, pointcut,	aop::AFTER,	 <<@mutex.unlock()>>);
	aop::aspect(ast, class,		aop::BEFORE, <<@mutex : mutex_new()>>); //insert mutex member
	aop::advise(ast, aop::BEFORE, <<function mutex_new() { return []; }>>);
	return ast;
}
