function f (x,y) {
	return x+y;
}

function h(x) {
	return x * (x-1);
}

function g(x, y, f, h) {
	return h(f(x,y));
}

print(g(2, 3, f, h));