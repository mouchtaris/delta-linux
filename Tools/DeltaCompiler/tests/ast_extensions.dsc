using std;

function transform(ast) {
	local quasiQuotes = list_new();
	local visitor = astvisitor_new();
	local quoteNesting = 0;
	visitor.set_handler("QuasiQuotes", function(node, id, entering){
		quoteNesting += (entering ? 1 : -1);
		if (not entering and quoteNesting == 0)
			quasiQuotes.push_back(node);
	});
	ast.accept_preorder(visitor);
	foreach(local quasi, quasiQuotes) {
		local replacement = <<[@ast : ~quasi, method test() { std::print("AST::test\n"); }]>>;
		//TODO: add appropriate methods based on the content of the AST.
		//for instance in function could add GetName(), GetTotalArgs(), etc.
		//or for table constructors modelling classes could add GetMethods(), GetAttributes(), etc.		
		local parent = quasi.get_parent();
		quasi.get_parent().replace(quasi, replacement.get_child(0).copy());
	}
	
	//TODO: similarly change escapes
	
	/*local inlineArgs = list_new();
	visitor = astvisitor_new();
	visitor.set_handler("FunctionCall", function(node, id, entering){
		if (entering) {
			local func = node.get_child("function");
			if (func.get_tag() == "LvalueNamespaceIdent"			and
				func.get_total_children() == 2						and
				func.get_child(0).get_attribute("name") == "std"	and
				func.get_child(1).get_attribute("name") == "inline"
			) {
				inlineArgs.push_back(node.get_child("actuals").get_child(0).get_child(0).get_child(0));	//get the primary expression
				//visitor.leave(); TODO: add this after the astvisitor bug has been fixed
			}
		}
	});
	ast.accept_preorder(visitor);
	foreach(local arg, inlineArgs) {
		local replacement = <<~arg.ast>>;
		arg.get_parent().replace(arg, replacement.get_child(0).copy());
	}*/
	return ast;
}