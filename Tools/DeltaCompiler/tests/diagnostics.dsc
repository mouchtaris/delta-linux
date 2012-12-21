&{
	&function add_diagnostic_code(func, enter, exit) {
		func = func.get_child(0);
		local compound = func.get_child("body");
		local stmts = compound.get_child("stmts");
		for (local i = enter.get_total_children(); i; --i) {
			local child = enter.get_child(i - 1);
			local stmt = child.copy();
			stmts.push_front(stmt);
		}
		
		local returns = std::list_new();
		local visitor = std::astvisitor_new();
		visitor.set_handler("return", function(node, id, entering){ if (not entering) returns.push_back(node); });
		func.accept_preorder(visitor);
		
		foreach(local ret, returns) {
			local replacement = nil;
			if (ret.get_total_children()) {	//return expr;
				local retval = ret.get_child("expr");
				replacement = <<{ local retval = ~retval; ~exit; return retval; }>>;
			}
			else	//return;
				replacement = <<{ ~exit; return; }>>;
			local parent = ret.get_parent();
			parent.get_parent().replace(parent, replacement.get_child(0).get_child(0));
		}
	}

	&func = <<
		function pow(x, n) {
			if (n == 0)
				return 1;
			else
				return x * pow(x, n - 1);
		}
	>>;
	&add_diagnostic_code(
		func,
		<<std::print("Entering pow: x = ", x, ", n = ", n, "\n");>>,
		<<std::print("Exiting  pow: x = ", x, ", n = ", n, "\n");>>
	);
	!(func);
	std::print(pow(2, 3), "\n");
}