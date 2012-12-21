&function DbyC (call, handler) {
	return <<try { ~call; } trap $ex { ~handler; }>>;
}

function Stack {
	return [
		@index : -1,
		@table : [],
		method top {
			if (@empty())
				throw "StackEmptyException";
			return @table[@index];
		},
		method pop {
			local value = @top();
			@table[@index--] = nil;
			return value;
		},
		method push(x) { @table[++@index] = x; },
		method empty { return @index == -1; }
	];
}

s = Stack();
s.push(1);
s.push(2);

assert s.top() == 2;

!(DbyC(<<s.pop()>>, <<std::print("precondition failed\n")>>));
assert s.top() == 1;

!(DbyC(<<s.pop()>>, <<std::print("precondition failed\n")>>));
assert s.empty();

!(DbyC(<<s.pop()>>, <<std::print("precondition failed\n")>>));	//raises exception