using std;
function add_diagnostic_code(func, enter, exit) {
	local compound = func.get_child("body");
	local stmts = compound.get_child("stmts");
	for (local i = enter.get_total_children(); i; --i) {
		local child = enter.get_child(i - 1);
		local stmt = child.copy();
		stmts.push_front(stmt);
	}
	
	local returns = list_new();
	local visitor = astvisitor_new();
	visitor.set_handler("return", function(node, id, entering){ if (not entering) returns.push_back(node); });
	func.accept_preorder(visitor);
	
	foreach(local ret, returns) {
		local replacement = nil;
		if (ret.get_total_children()) {	//return expr;
			local retval = ret.get_child("expr");
			//TODO: fix the alpha_renaming to work if this aspect is applied multiple times
			replacement = <<{ local $retval = ~retval; ~exit; return $retval; }>>;
		}
		else	//return;
			replacement = <<{ ~exit; return; }>>;
		local parent = ret.get_parent();
		parent.get_parent().replace(parent, replacement.get_child(0).get_child(0));
	}
}


function transform_logging(ast) {
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

function transform_add_staging(ast) {
	local stmts = ast.get_child("stmts");
	
	local e = (<<!(<<std::print("staged print statement");>>)>>).get_child(0).copy();
	local expr = ast_new("Expr");
	expr.push_back(e, "expr");
	
	local exprListStmt = ast_new("ExprListStmt");
	exprListStmt.push_back(expr);
	
	local basicStmt = ast_new("BasicStmt");
	basicStmt.push_back(exprListStmt, "stmt");
	
	local stmt = ast_new("Stmt");
	stmt.push_back(basicStmt, "stmt");
	
	stmts.push_back(stmt);
	return ast;
}

function transform(ast) { return transform_logging(ast); }

/*runtime = "Runtime";
&compile_time = <<std::print(runtime)>>;
&function stage1() { return compile_time; }
&&function stage2() { return <<"Compilation">>; }
!(compile_time);
&std::print(!(stage2()), "\n");
!(stage1());
*/
/*
function f(ast, num) {
	std::print(num);
	return ast;
}

<<~(!(f(<<1>>, 1)))>>;
*/
//!(f(!(f(<< <<std::print("1")>> >>, 2)), 3));
//!(f(!(f(<< <<std::print("4")>> >>, 5)), 6));

//////////////////////////////////////////////////////////////////////////////////
// Specification based function generation and usage.
//
/*&function GenerateFunctionSpec(arg) {
	if (arg)
		return [
			@decl : <<function add(x, y) { return x + y; }>>,
			@call : <<add(1, 2)>>
		];
	else
		return [
			@decl : <<function ten { return 10; }>>,
			@call : <<ten()>>
		];
}

&func1 = GenerateFunctionSpec(true);
&func2 = GenerateFunctionSpec(false);

!(func1.decl);
!(func2.decl);

//probably have application code here

std::print(!(func1.call), "\n");
std::print(!(func2.call), "\n");

//////////////////////////////////////////////////////////////////////////////////
// Generated function names and corresponding usages.
//
&function id_new(name) { return std::ast_new("Name", [@name : name]); }
&name = id_new("foo");
t = [ method !(name) { std::print("method called\n"); } ];
t[!(name.get_attribute("name"))]();
*/
//////////////////////////////////////////////////////////////////////////////////

/*
//Aspects
//file Logging.dsc

//joinpoints:	i) 	function execution
//				ii)	attribute access (set/get)
				iii)trap
				iv)	table constructor
				v)	
function transformation(ast) {

	local pointcut = "execution(method *(*))";
	
	//e.g. "onevent *(event)" 	-> all onevent functions with a single arg named event
	//e.g. "* foo(*)" 			-> all types of functions named foo (with any number of args)
	
	local advice = [
		@pointcut	: pointcut,
		@type		: "before",
		@code		: <<print("before")>>
	];
	
	//aspect(target:ast,pointcut:String,advice_type:String,advice:ast)
	aspect(ast, pointcut, "before", <<print("before")>>);
	aspect(ast, pointcut, "after", <<print("after")>>);
	//the around form uses a delayed escape that is substituted by the matched ast
	aspect(ast, pointcut, "around", <<print("before"); ~~around; print("after");>>);
	
	//////////////////////////////////////////////////////////////////////
	
	//joinpoints(target:ast, pointcut:String)
	local matches = joinpoints(ast, pointcut);
	foreach(local match, matches) {
		//advice(target:ast,advice_type:String,advice:ast)
		advice(match, "before", <<print("before")>>);
		advice(match, "after", <<print("after")>>);
		advice(match, "around", <<print("before"); ~~around; print("after");>>);
		advice(match, "around", <<print("before"); ~match; print("after");>>);	//same as above
	}
	
	//////////////////////////////////////////////////////////////////////
	
	
	//public aspect Logging {
	//	pointcut method() :  execution(* *(..));

	//	before(): method() {
	//		System.out.println("before");
	//	}
	//	after(): method() {
	//		System.out.println("after");
	//	}

	//}

	
	
/*	
	id = asp:aspect( {name = 'logaspect'}, {pointcutname = 'logdeposit', designator = 'call',
	list = {'Bank.deposit'}},{type = 'before', action = logfunction} )
*/
}
*/

/*
function f() {
	function g() {
		function h() {
			function i() { f(); g(); h(); }
			i();
		}
		h();
	}
	g();
}
*/

/*
function i() { f(); }	//the one with the least dependencies (the inner-most if tied)

function f_g_h_i() { f(); g(); h(); }

function f(x) {
	function g() { f(x); }
	g();
}

function g() { f(); };
function f() { g(); }
*/

//print(g());

/*!(<<function f(x) { if (x) return true; }>>);

function func_generator(name) { return <<function ~name(x, y) { print("foo\n");}>>; }

!(func_generator(<<foo>>));
!(<<!(<<&foo(a, 1);>>);>>);
*/
/*

&{
	&x = <<1>>;
	&y = <<~x + 2>>;
	std::print(!y);
}
*/

//&function foo() { return <<using std;>>; }
//!foo();			// using std;


/*function printer(x) { 
	function bind(f, x) { return (function { f(x, ...); }); }
	return bind(std::print, x);
}

//&& p = printer(10);
//&std::print(123);
//&& p();
function foo() {
	function f() { std::print(1); }
	&f();
}

x = 1;

function bind(f, x) {
	return (function { f(x, ...); });
	&function foo(){}
	!<<3>>;
}
function lala(){}

function impure() { x = 2;
	&bind(1, 2);
}

&function foo() {
	foreach(local var, arguments)
		(function printer() { std::print(var); })();
}

function f() {
	function g() { return <<1>>; }
	&x = g();
	function h() { return !x; }
	function h2() {
		function h3(){ &x = h3();}
		&x = h2();
	}
	return h;
}
print(f()());



x = 0;
std::print(x);

&x = 1;
function f(){ return <<function foo(){}>>;}
&x = 2;
function a() {
	&x = 3;
	function b() {
		&x = 4;
		!f();
	}
	b();
	&x = 5;
}

&x = 6;
!f();
&x = 7;
&foo();
function g(){ !f(); }
&x = 8;
function h() { g(); }

&function foo2() { foo(); }
*/
/*x = <<
	function lala(){
		print(~(local z = <<1>>));
	}
>>;
i = std::print;
function h(x) { x;}
function f() {
	function g() { f(!1);}
	function h() { g(); }
	h();
}
&{
	z = 1;
	&{z = 2;}
}

function recursive() { recursive(); }

function foo() {
	&!a;
	try {}
	trap x1 {}
}*/