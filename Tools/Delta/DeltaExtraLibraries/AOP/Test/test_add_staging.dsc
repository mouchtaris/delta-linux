function power (x, n) {
	if (n == 0)
		return 1;
	else
		return x * power(x, n - 1);
}
std::print("Power(2,4)= ", power(2, 4), "\n");
x = 3;
std::print("Power(x,3) = ", power(x, 3), "\n");

function fibonacci(n) {
	if (n == 0 or n == 1)
		return 1;
	else
		return fibonacci(n - 1) + fibonacci(n - 2);
}

std::print("Fibonacci(5) = ", fibonacci(5), "\n");

function factorial(n) {
	if (n == 0)
		return 1;
	else
		return n * factorial(n - 1);
}

std::print("Factorial(5) = ", factorial(5), "\n");