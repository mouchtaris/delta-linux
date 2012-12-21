using std;

function insideFunc(node, name) {
	if (not node)
		return false;
	else if (	node.get_tag() == "Function" and
				(local child = node.get_child("name")) and
				child.get_tag() == "Name" and child.get_attribute("name") == name
	)
		return true;
	else
		return insideFunc(node.get_parent(), name);
}

function stage(expr) {
	local tag = expr.get_tag();
	if (tag == "MulOp") {
		local left = expr.get_child(0);
		local replacement = << ~~(~left) >>;
		expr.replace(left, replacement.get_child(0).copy());
		
		local right = expr.get_child(1);
		replacement = << ~~(~right) >>;
		expr.replace(right, replacement.get_child(0).copy());
	}
	//TODO: handle the rest of the cases as well
	return (<< <<~expr>> >>).get_child(0).copy();
}

function transform(ast) {
	local returnExprs = list_new();
	local visitor = astvisitor_new();
	visitor.set_context_handler("return", "expr", function(node, id, entering){
		if (not entering and insideFunc(node, "power")) returnExprs.push_back(node);
	});
	ast.accept_preorder(visitor);
	foreach(local expr, returnExprs)
		expr.get_parent().replace(expr, stage(expr));
		
	local calls = list_new();
	local visitor = astvisitor_new();
	visitor.set_handler("FunctionCall", function(node, id, entering){
		if (not entering and not insideFunc(node, "power")) {
			local child = node.get_child("function");
			if (child.get_tag() == "LvalueIdent") {
				child = child.get_child("name");
				if (child.get_tag() == "Name" and child.get_attribute("name") == "power")
					calls.push_back(node);
			}
		}
	});
	ast.accept_preorder(visitor);
	foreach(local call, calls) {
		local firstArg = call.get_child("actuals").get_child(0).get_child(0).get_child(0);
		firstArg.get_parent().replace(firstArg, stage(firstArg));
		local replacement = <<!(~call)>>;
		call = call.get_parent();	//to skip the primary expr
		call.get_parent().replace(call, replacement.get_child(0).copy());
	}
	return ast;
}

/*
//The following uses the AOP library
function transform (ast) {
	local toAST = << << ~~around >> >>;
	local addEscape = <<~~(~~around)>>; 
	local inPower = "descendant(function power(*))";	
	aop::aspect(ast, "child(multiply) and " + inPower, "around", addEscape);
	aop::aspect(ast, "child(return) and " + inPower, "around", toAST);
	local calls = aop::match(ast, "call(power) and not " + inPower);
	foreach(local call, calls) {
		aop::advice(call.GetActual(0), "around", toAST);
		aop::advice(call, "around", << !(~~around) >>);
	}
}
*/