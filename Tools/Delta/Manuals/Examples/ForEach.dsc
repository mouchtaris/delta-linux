// Example regarding the use of foreach stmt.
// A. Savidis, July 2009.

using std;
const nl = "\n";

// Object iteration.
foreach (i:x, [0, 1, 2, 3, 4, 5, 6])
	print(x + ":" + i, "\n");

// List iteration.
foreach (x, l = list_new("hello", "world"))
	{ l.remove(x); print(x,"\n"); }
	
l.push_back(1, 2, 3, 4);
j = (i = l.iterator()).setbegin(l).copy();
print(i, j, nl);

x=10, t=[12], y="this";
// Vector object iteration.
v = vector_new(3);
v.setval(0, "a");
v.setval(1, "b");
v.setval(2, "c");
v.resize(5); 
v.setval(3, "d");
v.setval(4, "e");

foreach (i:x, v)  
	print(i + ":" + x,"\n");

// Object iteration with user-defined iterators.
t = [
	method iterator {
		return [
			@i 	: listiter_new(),
			@pos: 0,
			method setbegin(t)	{ @i.setbegin(t.l); @pos = 0; },
			method checkend(t)	{ return @i.checkend(t.l); },
			method getval		{ return @i.getval(); },
			method getindex		{ return @pos; },
			method fwd 			{ @i.fwd(); ++@pos;}
		];
	},
	function f{},
	@l : list_new("a list", "inside", "a table")
];
foreach (i:x, t)
	print(i + ":" + x,"\n");

// Vectors and iteration.
v = vector_new(10);
for (i = 0; i < 10; ++i)	// style 1 with lib methods
	v.setval(i, i);
v.resize(12);
v.setval(10, 10);
v.setval(11, 11);
for (i = 0; i < v.total(); ++i)
	print(v.getval(i));
print(nl);

v.push_front("hello");
v.push_back("world");
print(v.front(), v.back());	
v.pop_front();
v.pop_back();
print(v.front(), v.back());	

print(nl, |v|, nl);

foreach(i:x, v)
	print(i + ":" + x + " ");
print(nl);
	
// Access via subscript.
v[0] = "FIRST", v[v.total()-1] = "LAST";
v.size = v.total();
for (i = 0; i < v.total(); ++i)	
	print(v[i]);
print(nl);

function ff(...) {
	foreach (x, arguments)
		print(x, nl);
}

ff(11,22,33,44);
