// Generators are a technique commonly related to continuations (green threads), 
// enabling functions yielding output with successive invocations ('yield' operator,
// as opposed to 'return'). Such functions can be used in foreach.
// Their advantage is memory efficiency since output is produced on demand, 
// rather than being entirely stored in a container.
// Here they are implemented easily by combining the extensible API of foreach and
// closures for functions with state (could use functor objects too).
// A. Savidis, July 2013

using #Delta;

const nl = "\n";
UNDEF 	 = std::undefined();

////////////////////////////////////////////////////////////
// Functions signify end of output with an undefined value.
// We use closures to keep state.

function local range (a, b) {
	local i = a;
	return lambda { (i < b ? i++ : UNDEF) };
}

/*
Equivalent with yield:
function range(a,b) {
	for (local i = a; i < b; ++i)
		yield(i);
}
*/

////////////////////////////////////////////////////////////

function local factorial (n) {
	local i = local x = 1;
	return lambda { (i < n ? x *= i++ : UNDEF) };
}

/*
Equivalent with yield:
function factorial(n) {
	for (local i = local x = 1; i < n; ++i)
		yield(x *= i);
}
*/

////////////////////////////////////////////////////////////

std::print("Range test", nl);
foreach (x, Delta::Generator(range(0, 23)))
	std::print(x, nl);

std::print("Factorial test", nl);
foreach (x, Delta::Generator(factorial(9)))
	std::print(x, nl);

/*
Equivalent with yield:

foreach (x, (range(0, 23))
	std::print(x, nl);

std::print("Factorial test", nl);
foreach (x, factorial(9))
	std::print(x, nl);	
*/

////////////////////////////////////////////////////////////
// retrofitting with untyped attributes

std::print("Redtrofgitting with traits via untyped attribute pattern:\n");

// Point3d ctor; fields as x,y,z
function Point3d (x,y,z){ 
	return [ @x :x, @y : y, @z : z ];
}
// Vector3d ctor; an array of values
function Vector3d (x,y,z){ 
	return [ x, y, z];
}

// retrofitting Vector3 using untyped attribute pattern 
function Vector3d_point_traits (v) {
	return [
		@x {	
			@set method(v) 	{ v[0] = v; }
			@get method 	{ return v[0]; } 
		},
		@y {	
			@set method(v) 	{ v[1] = v; }
			@get method 	{ return v[1]; } 
		},
		@z {	
			@set method(v) 	{ v[2] = v; }
			@get method 	{ return v[2]; } 
		}
	];
}

function move (a, b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

p1 = Point3d(1,2,3);
v1 = Vector3d(-1,-2,-3);
move(
	p1, 
	Vector3d_point_traits(v1)	// retrofitting manually applied
);

std::print(p1, nl);

////////////////////////////////////////////////////////////

