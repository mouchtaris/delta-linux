&function Singleton() {
	return [
		//TODO: class is added in the AST by copy, so it singleton is applied first,
		//next transformations will operate on a copy and never be part of the main AST
		method def(class) {	//assume ctor func has the form: proto = [...]; return proto;
			local n = class;
			while (n.get_tag() != "Stmt")
				n = n.get_parent();
			local stmts = n.get_parent();
			n.remove_from_parent();
			local code = <<if (std::isundefined(static proto)) proto = ~class;>>;
			code = code.get_child(0);
			code.remove_from_parent();
			stmts.push_front(code);
			return nil;
		},
		method use(code) { return code; }
	];
}

&function DbyC() {
	return [
		method def(class) {
			//utility funcs
			function GetMethod(ast) {
				if (ast.get_tag() != "UnindexedElem")
					return nil;
				ast = ast.get_child("expr");
				if (ast.get_tag() != "FunctionExpr")
					return nil;
				ast = ast.get_child("expr");					
				if (ast.get_tag() == "Function" and ast.get_attribute("class") == "Method")
					return ast;
				else
					return nil;
			}
			function GetMethodName(m) {
				if ((local n = m.get_child("name")) and n.get_tag() == "Name")
					return n.get_attribute("name");
				else
					return "";
			}
			function HasMethod(class, name) {
				foreach(local child, class.get_children())
					if ((local m = GetMethod(child)) and GetMethodName(m) == name)
						return true;
				return false;
			}
			function AddFirstStmt(func, code) {
				local compound = func.get_child("body");
				local stmts = compound.get_child("stmts");
				if (not stmts)	//just replace the compoumd stmts with the given code stmts (TODO: separate handling for expr)
					compound.push_front(code.copy(), "stmts");
				else
					stmts.push_front(code.get_child(0).copy());	//TODO: fix issue when copy is not used
			}
			//implementation
			foreach(local child, class.get_children())
				if ((local m = GetMethod(child)) and (local name = GetMethodName(m)) != "") {
					local pre_id = "pre_" + name;
					if (HasMethod(class, pre_id))
						AddFirstStmt(
							m,
							<<
								if (not self[~pre_id]())
									throw [
										@class : "ContractException",
										@type : "Precondition",
										@method : ~(name)
									];
							>>
						);
				}
			return nil;
		},
		method use(code) {
			return <<
				try { ~code; }
				trap ContractException { std::print(ContractException); }
			>>;
		}
	];
}

&function Transformations(...) {
	local transformations = [
		@list : arguments,
		method def(class) {
			local retval = nil;
			foreach(local t, @list)
				retval = <<~retval, ~(t.def(class))>>;
			return retval;
		},
		method use(code) {
			foreach(local t, @list)
				code = t.use(code);
			return code;
		}
	];
	return transformations;
}
&t = Transformations(DbyC(), Singleton());

function Stack() {
	proto = [
		method empty() { return true; },
		method pop(){},
		method pre_pop(){ return not @empty(); },
		!(t.def(std::context("TableConstructor")))
	];
	return proto;
}
stack = Stack();
!(t.use(<<stack.pop()>>));
