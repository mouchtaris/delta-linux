function add(x,y) {
	return x+y;
}

function f(x,y) {
	return callglobal("add",x,y);
}

print(f(10,20),"\n");
