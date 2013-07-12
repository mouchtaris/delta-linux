// Various examples on the use of closures.
// A. Savidis, July 2010.

using std;

/////////////////////////////////
// On block vars of global scope.
f = nil;
{
	local x = 10;
	f = (function { print(x); }); 
}
f();	// Prints 10

/////////////////////////////////
// Simple functors.
function const_maker (x) {
	return function { return x; }
}

c_hw = const_maker("hello, world");
print(c_hw());	// Prints 'hello, world'

/////////////////////////////////
// Access to outer function closure vars.

function f1 (x,y,a,t) {
	function f2 (z,w) {
		{ print(x); { print(y); } }			// Inside blocks but recorded as closure vars.
		print(a);
		t;	// just access
		function f3 { print(x, y, a, z, w); t; }	// Access f1 via f2 closure.
		return f3;
	}
	return f2; 
}

f = f1(11,22, 55, [])(33,44); // prints 112255 (2nd call).
f();	// prints =1122553344 (3rd call).
		
/////////////////////////////////
// Iteration and ability to change the closure vars.

print("\n");
function do {
	foreach(a, arguments)
		(
		function { 
			print(a, "\n"); a = "*"; }
		)(), print(a);
}
do(true, nil, 45, 21);

/////////////////////////////////
// Binders.

function bind1st(f, x) 
	{ return lambda { f(x, |arguments|) }; }

f = bind1st(bind1st(print, "this"), "that");
print(f, "\n");
f();

/////////////////////////////////
// Resursive function with closure.

f = (lambda(n) {
		(function { if (n) { print(n--), @lambda(); } })
	})(10);
f();

//////////////////////////////////
// 'self' of an outer method can be part of
// the closure too for embedded to method functions.

t = [
	method f {
		return lambda 
			{ std::print(self.x + self.y) }; // 'self' is a closure var (formal arg). 
	},
	@x : "\nhello", @y : ",world\n"
];

t.f()();

//////////////////////////////////
// Sharing of closure vars among different
// function values taken within the same call.

function shared (x,y,z, t) {
	return [
		@ set_xz : 			function(v1, v2)	{ x = v1, z = v2; },
		@ print_xy_set_y : 	function(v) 		{ std::print(x, " ", y, "\n"); y = v; },
		@ print_zy : 		function 			{ std::print(z, " ", y, "\n"); },
		@ print_x : 		function 			{ std::print(x, "\n"); },
		@ t : 				function 			{ return t; }
	];
}

t = shared(12, 23, 35, [ "sample object" ]);
t.print_zy();			// 35 23
t.print_xy_set_y(24); 	// 12 23
t.print_zy();			// 35 24
t.set_xz(13, 36);
t.print_zy();			// 36 24
t.print_x();			// 13
t.t();					// Set a bpt at @t above and inspect 't' (closure var) through i-views

//////////////////////////////////
// Test use of functions with closures by various inner 
// functions.

fv = (function(x, y) {

	const space = " ";
	const nl	= "\n";	
	local z		= "goodbye, world";
	
	function f {
		function h 
			{ return f; }					// inner 'h' accesses 'f' but don't know 'f' has a closure yet
		print(x, space, y, space, z, nl);	// here we only know 'f' has a closure
		return h;
	}
	
	function g { 
		function q { return f; }			// inner 'q' in 'g' uses 'f'
		return q();
	}
	
	++x, ++y;								// change closure
	g()();									// g() returns 'f' with closure and calls it.
	
	return f;
	
})(5, 7);

fv()()();

//////////////////////////////////

function q (a, b) {
	return function (x,y) {
		print(a,b); 
		return function { print(a); }
	}
}

q(1,2)(3,4)();	// 121
print("\n");

//////////////////////////////////

t = [
	@x : 10,
	method f {
		return	function g {
					self.x;
					return	function h {
								return self.x + 20;
							}
				}
	}
];

std::print(t.f()()());