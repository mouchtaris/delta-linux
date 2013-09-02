std::dllimportdeltalib(aop::DLL);

function transform (ast) {
	local quasiquotes = aop::match(ast,"ast(quasiquote)"); //find all AST creations
	foreach(local quote, quasiquotes) {
		if (quote.GetChild().GetType() == "class")
			aop::advise(quote, aop::AROUND, <<[		//AST creations are replaced with objects 
					@ast : ~~proceed,				//the original AST is stored as normal data
					method GetMethods	(){},		//custom methods added
					method GetAttributes(){},
					method BaseClasses	(){}
			]>>);
		else if (quote.GetChild().GetType() == "function")
			aop::advise(quote, AROUND, <<[
					@ast : ~~proceed,
					method GetName	() {},
					method GetActual(n){},
					method GetLocals() {}
			]>>);
		else
			;	//perform similar handling for other quoted language elements
	}
	aop::aspect(ast, "child(escape)", aop::AROUND, <<~~proceed.ast>>);
	aop::aspect(ast, "child(inline)", aop::AROUND, <<~~proceed.ast>>);
	return ast;
}
