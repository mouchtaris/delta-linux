using std;
function transform(ast) {
	local stmts = ast.get_child("stmts");
	stmts.push_front((<<using #main;>>).get_child(0).copy());
	return ast;
}
