using aop;
using #utility;

std::dllimportdeltalib(aop::DLL);	//import AOP library

function GetEnclosingBlock(ast) {
	while(ast and ast.get_tag() != "Program" and ast.get_tag() != "Stmts")
		ast = ast.get_parent();
	return ast;
}

function GetEnclosingStmt(ast) {
	while(ast and ast.get_tag() != "Stmt")
		ast = ast.get_parent();
	return ast;	
}

function transform(ast) {
	local visitors = "ast(\"AssignOp\") and ascendant(call(std::astvisitor_new()))";
	foreach(local visitor, match(ast, visitors)) {
		local id = visitor.get_child("lvalue").copy();
		local name = utility::GetName(id);
		local handlers = "execution(function (node, id, entering))" +
			" and descendant(call(" + name + ".set_handler(..)))"; //match handlers 
		
		aspect(GetEnclosingBlock(visitor), handlers, AROUND, <<if(nesting==0)~~proceed;>>);
		
		advise(GetEnclosingStmt(visitor), AFTER, <<  //introduce nesting and increase and decrease
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
