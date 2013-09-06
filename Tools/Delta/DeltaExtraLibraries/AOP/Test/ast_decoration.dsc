using aop;

std::dllimportdeltalib(aop::DLL);

function transform (ast) {
	local quasiquotes = match(ast,"ast(\"QuasiQuotes\")"); //find all AST creations
	foreach(local quote, quasiquotes) {
		local tag = quote.get_child("expr").get_child(0).get_tag();
		if (tag == "Function")
			advise(quote, AROUND, <<[
					@ast : ~~proceed,
					method GetName	() { return @ast.get_child(0).get_child("name").get_attribute("name"); },
					//the remaining weaved methods have little actual functionality; they just print for demonstraction purposes
					method GetActual(n){ std::print("Function::GetActuals\n");},
					method GetLocals() { std::print("Function::GetLocals\n"); }
			]>>);
		//else if (...) perform similar handling for other quoted language elements
		else
			advise(quote, AROUND, <<[@ast : ~~proceed]>>);
	}
	aspect(ast, "child(ast(\"Escape\"))", AROUND, <<~~proceed.ast>>);
	aspect(ast, "ast(\"PrimaryExpression\") and descendant(call(std::inline(*)))", AROUND, <<~~proceed.ast>>);
	return ast;
}
