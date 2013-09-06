using aop;

std::dllimportdeltalib(aop::DLL);	//import AOP library

function transform (ast) {
	local class = "class(SharedObject)"; //class for synchronization
	local pointcut = "descendant(" + class + ") and execution(method *(..))";
	aspect(ast, pointcut,	BEFORE, <<@mutex.lock()>>);
	aspect(ast, pointcut,	AFTER,	 <<@mutex.unlock()>>);
	aspect(ast, class,		BEFORE, <<@mutex : mutex_new()>>); //insert mutex member
	advise(ast, BEFORE, <<function mutex_new() { return []; }>>);
	return ast;
}
