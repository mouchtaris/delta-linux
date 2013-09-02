using std;
const USE_AOP_LIB = true;

if (USE_AOP_LIB)
	dllimportdeltalib(aop::DLL);

//////////////////////////////////////////////////////////////////////////////////
// Transformation using only AST manipulation
//
function add_diagnostic_code(func, enter, exit) {
	local compound = func.get_child("body");
	local stmts = compound.get_child("stmts");
	for (local i = enter.get_total_children(); i; --i)
		stmts.push_front(enter.get_child(i - 1).copy());
	for (local i = 0; i < exit.get_total_children(); ++i)
		stmts.push_back(exit.get_child(i).copy());
	
	local returns = list_new();
	local visitor = astvisitor_new();
	visitor.set_handler("return", function(node, id, entering){ if (not entering) returns.push_back(node); });
	func.accept_preorder(visitor);
	
	foreach(local ret, returns) {
		local replacement = nil;
		if (ret.get_total_children()) {	//return expr;
			local retval = ret.get_child("expr");
			replacement = <<{ local $retval = ~retval; ~exit; return $retval; }>>;
		}
		else	//return;
			replacement = <<{ ~exit; return; }>>;
		local parent = ret.get_parent();
		parent.get_parent().replace(parent, replacement.get_child(0).get_child(0));
	}
}

function transform_with_ast_library(ast) {
	local funcs = list_new();
	local visitor = astvisitor_new();
	visitor.set_handler("Function", function(node, id, entering){
		if (not entering) funcs.push_back(node);
	});
	ast.accept_preorder(visitor);
	foreach(local func, funcs) {
		local name = func.get_child("name").get_attribute("name");
		add_diagnostic_code(
			func,
			<<std::print("Entering " + ~name + "\n");>>,
			<<std::print("Exiting " + ~name + "\n");>>
		);
	}
	return ast;
}

//////////////////////////////////////////////////////////////////////////////////
// AOP transformation
//
function transform_with_aop_library(ast) {
	local funcs = aop::match(ast, "execution(function *(..))");
	foreach(local f, funcs) {
		local name = f.get_child("name").get_attribute("name");
		aop::advise(f, aop::BEFORE, <<std::print("Entering " + ~name + "\n");>>);
		aop::advise(f, aop::AFTER,  <<std::print("Exiting "  + ~name + "\n");>>);
	}
	return ast;	
}

//////////////////////////////////////////////////////////////////////////////////
// Aspect entry function
//
function transform(ast) {
	local transformer = (USE_AOP_LIB ? transform_with_aop_library : transform_with_ast_library);
	return transformer(ast);
}