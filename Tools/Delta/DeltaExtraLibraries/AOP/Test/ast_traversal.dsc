std::dllimportdeltalib(aop::DLL);

function transform(ast) {
	local visitors = "ast(\"AssignOp\") and parent(call(astvisitor_new()), \"rvalue\")";
	foreach(local visitor, aop::match(ast, visitors)) {
		local id = visitor.get_child("lvalue").copy();
		local handlers = "execution(function (node, id, entering)) and descendant(call("+id.GetName()+".set_handler(..))"; //match handlers 
		aop::aspect(visitor.Block(), handlers, aop::AROUND, <<if(nesting==0)~~proceed;>>);
		aop::advise(visitor, aop::AFTER, <<  //introduce nesting and increase and decrease
			local nesting = 0;    			 //it as needed in quasi-quote and escape handlers
			~id.set_handler("QuasiQuotes", function(node, id, entering) {
				if (entering) ++nesting; else --nesting;
			});
			~id.set_handler("Escape", function(node, id, entering) {
				if (entering) --nesting; else ++nesting;
			});
		>>); 
	}
	return ast;
}
