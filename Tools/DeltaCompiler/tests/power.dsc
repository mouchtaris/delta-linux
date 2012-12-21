&function ExpandPower (x, n) {
	if (n == 0)
		return <<1>>;
	else if (n == 1)
		return x;
	else
		return <<~x * ~(ExpandPower(x, n - 1))>>;
}
&function MakePower (n) {
	local multiplication = ExpandPower(<<x>>, n);
    local func = <<(function(x){ return ~multiplication; })>>;
	return func;
}

power3 = !(MakePower(3)); //power3 = (function(x) { return x*x*x; });
std::print("2^3 = ", power3(2), "\n");