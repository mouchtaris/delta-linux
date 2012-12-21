using std;

function transform_calls(ast) {
	local cacheCode = <<cache = [];>>;
	local stmts = ast.get_child("stmts");
	stmts.push_front(cacheCode.get_child(0).copy());
	
	local callAdvice = <<
		(function {
			if (n < std::tablength(cache))
				return cache[n];
			local result = ~~call;
			if (n >= std::tablength(cache))
				cache[n] = result;
			return result;
		})()
	>>;
	
	local calls = list_new();
	local visitor = astvisitor_new();
	visitor.set_handler("FunctionCall", function(node, id, entering){
		function withinFibonacci(node) {
			if (not node)
				return false;
			else if (	node.get_tag() == "Function" and
						(local child = node.get_child("name")) and
						child.get_tag() == "Name" and child.get_attribute("name") == "fibonacci"
			)
				return true;
			else
				return withinFibonacci(node.get_parent());
		}
		if (not entering and withinFibonacci(node)) {
			local child = node.get_child("function");
			if (child.get_tag() == "LvalueIdent") {
				child = child.get_child("name");
				if (child.get_tag() == "Name" and child.get_attribute("name") == "fibonacci") {
					calls.push_back(node);
				}
			}
		}
	});
	ast.accept_preorder(visitor);
	foreach(local call, calls) {
		local newCall = callAdvice.copy();
		newCall.inject(call.copy());
		call.get_parent().replace(call, newCall.get_child(0).get_child(0).copy());
	}
		
/*	aspect(
		ast,
		"ast(call(fibonacci(n)))",
		"around",
		callAdvice
	);
*/	
	return ast;
}

function transform_funcs(ast) {
	local cacheCode = <<
		memoizer = [
			@values : [],
			method Has(name, arg) { 
				if (local t = @values[name])
					return t[arg] != nil;
				else
					return false;
			},
			method Get(name, arg) { return @values[name][arg]; },
			method Set(name, arg, value) {
				if (not @values[name])
					@values[name] = [];
				@values[name][arg] = value;
			}
		];
	>>;
	local stmts = ast.get_child("stmts");
	stmts.push_front(cacheCode.get_child(0).copy());
	
	local beforeAdvice = <<
		{
			local $name = ~~name;
			if (memoizer.Has($name, n))
				return memoizer.Get($name, n);
		}
	>>;
	
	local afterAdvice = <<
		{
			local $retval = ~~retval;
			memoizer.Set(~~name, n, $retval);
			return $retval;
		}
	>>;
	
	local funcs = list_new();
	local visitor = astvisitor_new();
	visitor.set_handler("Function", function(node, id, entering){
		if (not entering and (local formals = node.get_child("formals"))	and
			formals.get_total_children() == 1								and
			formals.get_child(0).get_attribute("name") == "n"
		)
			funcs.push_back(node);
	});
	ast.accept_preorder(visitor);
	
	
	foreach(local func, funcs) {
		local name = func.get_child("name").get_attribute("name");		
		
		local beforeCopy = beforeAdvice.copy();
		beforeCopy.inject(name);		
		func.get_child("body").get_child("stmts").push_front(beforeCopy.get_child(0).copy());
		
		local returns = list_new();
		local visitor = astvisitor_new();
		visitor.set_handler("return", function(node, id, entering){ if (not entering) returns.push_back(node); });
		func.accept_preorder(visitor);
	
		foreach(local ret, returns) {
			assert ret.get_total_children();	//return expr;
			local retval = ret.get_child("expr");
			local replacement = afterAdvice.copy();
			replacement.inject(retval);
			replacement.inject(name);
			local parent = ret.get_parent();
			parent.get_parent().replace(parent, replacement.get_child(0).get_child(0));
		}
	}
		
/*	aspect(
		ast,
		"ast(function(fibonacci(n)))",
		"before",
		<<>>
	);
	aspect(
		ast,
		"ast(function(fibonacci(n)))",
		"after",
		<<>>
	);
*/	
	return ast;
}

function transform(ast) { return transform_funcs(ast); }