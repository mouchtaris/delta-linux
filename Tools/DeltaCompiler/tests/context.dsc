&function add_setters(ast) {
	local unindexed = std::ast_new("UnindexedElem");
	unindexed.push_back((<<3>>).get_child(0).copy(), "expr");
	ast.push_back(unindexed); 
	std::print(ast); 
	return nil;
}
&x = 1;
t = [
	@a : 1, @b : 2,
	!(add_setters(std::context("TableConstructor")))
];