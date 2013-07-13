// Generators are a technique commonly related to continuations (green threads), 
// enabling functions yielding output with successive invocations ('yield' operator,
// as opposed to 'return'). Such functions can be used in foreach.
// Their advantage is memory efficiency since output is produced on demand, 
// rather than being entirely stored in a container.
// Here they are implemented easily by combining the extensible API of foreach and
// closures for functions with state (could use functor objects too).
// A. Savidis, July 2013

const nl = "\n";
UNDEF 	 = std::undefined();

////////////////////////////////////////////////////////////
// Turning callables to container-like adapters that allow their
// use directly in foreach

function Gen(f) {	// f is callable sith sig (void):Value
	function Iterator {
		return	[
			method setbegin (c) { @func = c.func; },
			method checkend (c) { return std::isundefined(@val = @func()); },
			method fwd			{},
			method getval		{ return @val; }
		];
	}

	function Container (f) {
		return [
			@func : f,
			method iterator { return Iterator(); }
		];
	}

	return Container(f);
}

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
foreach (x, ::Gen(range(0, 23)))
	std::print(x, nl);

std::print("Factorial test", nl);
foreach (x, ::Gen(factorial(9)))
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
