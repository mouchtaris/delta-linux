&function fibonacci(n) {	//compile time version using normal computation 
	if (n == 0 or n == 1)
		return 1;
	else
		return fibonacci(n - 1) + fibonacci(n - 2);
}

&function GenerateFibonacciTable(upperBound) {
	local numbers = nil;
	for (local i = 0; i < upperBound; ++i)
		numbers = <<~numbers, ~(fibonacci(i))>>;	//merge computed values
	return <<[~numbers]>>;	//generate const table with the resulting values 
}
function fib(n) {			//runtime version using the generated const table
	static table=!(GenerateFibonacciTable(20));	//inline the const table here
	return table[n];
}
std::print("fib(15) = ", fib(15), "\n");  //call involves no runtime overhead
