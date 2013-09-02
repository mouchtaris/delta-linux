//Without the aspect, this will print Entering/Exiting messages for both functions f and g
//With the aspect it will only print the messages for function f.

&using #utility;
&{
	ast = << function f() { return << function g() {} >>; } >>;

	visitor = std::astvisitor_new();
	visitor.set_handler("Function", function(node, id, entering){
		std::print((entering ? "Entering" : "Exiting"), " node ", utility::GetFuncName(node), "\n");
	});
	ast.accept_preorder(visitor);
}