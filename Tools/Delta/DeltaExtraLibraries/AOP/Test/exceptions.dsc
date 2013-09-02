std::dllimportdeltalib(aop::DLL);

function AllMethodsInClass(class)	//helper to create pointcut expressions
	{ return "execution(method *(..)) and descendant(class(" + class + "))"; }

function transform (ast) {
	aop::aspect(ast, AllMethodsInClass("RemoteObject"), aop::AROUND, 
		<<try { ~~proceed; } trap Exception { std::print(Exception, "\n"); }>>
	); //log and ignore any exception regarding remote object invocations
	aop::aspect(ast, AllMethodsInClass("StackWithDbyC"), aop::AROUND,
		<<try { ~~proceed; } trap ContractException { assert false; }>>
	); //ensure no contract exceptions thrown by a class with Design by Contract
	aop::aspect(ast, AllMethodsInClass("ConfigurationManager"), aop::AROUND,
		<<try { ~~proceed; } trap IOException
			 { throw [@class:"ConfigException", @source:IOException]; } >>
	); //hide low level IOExceptions and raise higher level ones
	return ast;
}
