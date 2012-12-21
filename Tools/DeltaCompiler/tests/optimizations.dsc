&function fibonacci(n) {
	if (n == 0 or n == 1)
		return 1;
	else
		return fibonacci(n - 1) + fibonacci(n - 2);
}

&function GenerateFibonnaciTable(upperBound) {
	local numbers = nil;
	for (local i = 0; i < upperBound; ++i)
		numbers = <<~numbers, ~(fibonacci(i))>>;
	return <<[~numbers]>>;
}

function fib(n) {
	if (std::isundefined(static table))
		table = !(GenerateFibonnaciTable(20));
	assert n < std::tablength(table);
	return table[n];
}

std::print("fib(15) = ", fib(15));

/*
function power(x, n) {
	if (n == 0)
		return <<1>>;
	else if (n == 1)
		return <<~x>>;
	else
		return <<~x * ~(power(x, n - 1))>>;
}
&std::print("2^3 = ", !(power(<<2>>, 3)), "\n");

/////////////////////////////////////////////////////////

function fibonacci(n) {
	if (n == 0 or n == 1)
		return <<1>>;
	else
		return <<!(~(fibonacci(n - 1))) + !(~(fibonacci(n - 2)))>>;
}
&std::print("fib(5) = ", !(fibonacci(5)), "\n");
*/
