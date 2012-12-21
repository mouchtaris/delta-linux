//////////////////////////////////////////////////////////////////////////////////
// Flyweight pattern as a metaprogram
//
&using std;
using std;

//////////////////////////////////////////////////////////////////////////////////
// Utility metafunctions
//
&function id_new(name) { return std::ast_new("Name", [@name : name]); }

&function expr_new(name) {
	local lvalue = ast_new("LvalueIdent");
	lvalue.push_back(id_new(name), "name");
	local primaryExpr = ast_new("PrimaryExpression"); 
	primaryExpr.push_back(lvalue, "expr"); 
	return primaryExpr;
}

//////////////////////////////////////////////////////////////////////////////////
// Implementation of the Flyweight pattern for constructor functions 
// The code generated is based on the given flyweight members.
// (see example below for usage)
//
&function MakeFlyweight(func, members) {
	func = func.get_child(0);
	local compound = func.get_child("body");
	local stmts = compound.get_child("stmts");

	local flyweightMembers = list_new();
	local visitor = astvisitor_new();
	//TODO: only look within the function table constructor
	visitor.set_handler("IdentIndexElem", function(node, id, entering){
		if (not entering) {
			local child = node.get_child("name");
			local name = child.get_attribute("name");
			foreach(local member, members) {
				if (member == name) {
					flyweightMembers.push_back(node);
					return;
				}
			}
		}
	});
	func.accept_preorder(visitor);
	assert flyweightMembers.total() == members.total();	//found all flyweight members

	foreach(local member, flyweightMembers) {
		local name = member.get_child("name").get_attribute("name");
		local id = expr_new(name);
		local expr = member.get_child("expr");
		stmts.push_front((<<if (std::isundefined(static ~id)) ~id = ~expr;>>).get_child(0).copy());
		member.replace(expr, id);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Example using the Flyweight pattern for the soldiers of a war simulator
//
&func = <<
	function soldier(state, position, command) {
		return [
			@state		: state,
			@position	: position,
			@command	: command,
			
			@behaviour	: ["behaviour_initialization"],
			@graphics	: ["graphics_initialization"],
			@ai			: ["ai_initialization"],
			
			method act() { std::print(@behaviour, " ", @graphics, " ", @ai, "\n"); }
		];
	}
>>;
&MakeFlyweight(func, list_new("behaviour", "graphics", "ai"));	//Apply the pattern
!(func);

s1 = soldier("ok", "(0, 0)", "attack");
s2 = soldier("hurt", "(1, 1)", "defend");
s1.act();

assert s1.behaviour and s2.behaviour;
if (s1.behaviour == s2.behaviour)
	print("Same behaviour object (flyweight)\n");
else
	print("Different behaviour objects\n");
	