using aop;
using std;
using #utility;

std::dllimportdeltalib(aop::DLL);	//import AOP library

toAST = << << ~~proceed >> >>;
addEscape = <<~~(~~proceed)>>;
addStaging = <<&~~proceed;>>;

//AST utility functions
function GetActuals(call) {
	local actuals = list_new();
	local actualsNode = call.get_child("actuals");
	for(local i = 0, local n = actualsNode.get_total_children(); i < n; ++i)
		actuals.push_back(actualsNode.get_child(i).get_child(0).get_child(0));
	return actuals;		
}

function IsConst(node) {
	local tag = node.get_tag();
	if (tag == "NumConst" or tag == "StringConst" or tag == "BoolConst")
		return true;
	else if (tag == "PrimaryExpression")
		return IsConst(node.get_child("expr"));
	else
		return false;	
}

//Helper functions to create pointcuts
function InFunc(name) { return "descendant(execution(function " + name + "(..)))"; }
function MatchCall(name) { return "call(" + name + "(..))"; }
function OutsideRecursiveCall(funcName)
	{ return "not descendant(" + MatchCall(funcName) + ")"; }

//This is not the complete implementation, it is just for demonstration purposes
function UsesExternalState(func) { return false; }
function UsesOtherFunctions(func) { return false; }
function ModifiesAnyArgument(func) { return false; }
function IsRecursive(func) { return true; }

function IsStageableFunc(func) {
	if (UsesExternalState(func) or UsesOtherFunctions(func) or
		ModifiesAnyArgument(func) or not IsRecursive(func)
	)	//add other preconditions here
		return false;
	else
		return true;
}

function StageFunctionDefinition(func) {
	local matchId = "ast(\"LvalueIdent\")";
	local recursiveCall = MatchCall(utility::GetName(func));
	aspect(func, recursiveCall, AROUND, addEscape);
	
	local exprTargets = match(func, "child(ast(\"return\"))");
	while(exprTargets.total()) {
		local dependencies = list_new();
		foreach(local expr, exprTargets) {
			advise(expr, AROUND, toAST);
			local ids = match(expr, matchId + " and not descendant(" + recursiveCall + ")");
			foreach(local id, ids) {
				dependencies.push_back(utility::GetName(id));
				advise(id, AROUND, addEscape);
			}
		}
		exprTargets.clear();
		foreach(local dep, dependencies) {
			local assigns = match(func, "ast(\"AssignOp\") and parent(" + matchId + ", \"lvalue\")");
			foreach(local assign, assigns)
				if (utility::GetName(assign.get_child("lvalue")) == dep)
					exprTargets.push_back(assign.get_child("rvalue"));
		}
	}
	advise(func.get_parent(), AROUND, addStaging);
}

function StageFunctionCalls(ast, funcName) {
	local calls = match(ast, MatchCall(funcName) + " and not " + InFunc(funcName));
	foreach(local call, calls) {
		foreach(local actual, GetActuals(call))
			if (not IsConst(actual))
				advise(actual, AROUND, toAST);
		advise(call, AROUND, << !(~~proceed) >>);
	}
}

function transform (ast) {
	foreach(local func, match(ast, "execution(function *(..))"))
		if (IsStageableFunc(func)) {
			StageFunctionDefinition(func);
			StageFunctionCalls(ast, utility::GetName(func));
		}
	return ast;
}