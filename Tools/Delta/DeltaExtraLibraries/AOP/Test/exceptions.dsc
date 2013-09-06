using aop;

std::dllimportdeltalib(aop::DLL);	//import AOP library

function AllMethodsInClass(class)	//helper to create pointcut expressions
	{ return "execution(method *(..)) and descendant(class(" + class + "))"; }

function transform (ast) {
	aspect(ast, AllMethodsInClass("RemoteObject"), AROUND, 
		<<try { ~~proceed; } trap Exception { std::print(Exception, "\n"); }>>
	); //log and ignore any exception regarding remote object invocations
	aspect(ast, AllMethodsInClass("StackWithDbyC"), AROUND,
		<<try { ~~proceed; } trap ContractException { assert false; }>>
	); //ensure no contract exceptions thrown by a class with Design by Contract
	aspect(ast, AllMethodsInClass("ConfigurationManager"), AROUND,
		<<try { ~~proceed; } trap IOException
			 { throw [@class:"ConfigException", @source:IOException]; } >>
	); //hide low level IOExceptions and raise higher level ones
	return ast;
}
