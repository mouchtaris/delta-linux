print("VM global initialization!\n");

function f() {
	throw (function(){ print("Other VM exception functor!\n"); });
}

f();