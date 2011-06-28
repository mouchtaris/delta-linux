//////////////////////////////////////////
// Simple inheritance tests.
//
const nl = "\n";
meta = [ {"x":0}, {"y":1}, {"z":2}, {"w":3}, {"q":4}];
base1 = [{"x" : "hello"}, {"y" : ",world"} ];
base2 = [{"z" : "good"}, {"w" : ",day"} ];
derived = [];

inherit(base1, meta);
inherit(derived, base1);
inherit(derived, base2);

print("***Access via 'derived'\n");
print(derived.x, derived.y, nl);
print(derived.z, derived.w, nl);
print(derived.q, nl);

print("***Access via 'base1'\n");
print(base1.x, base1.y, nl);
print(base1.z, base1.w, nl);
print(base1.q, nl);

print("***Access via 'base2'\n");
print(base2.x, base2.y, nl);
print(base2.z, base2.w, nl);
print(base2.q, nl);

print(meta, nl, base1, nl, base2, nl, derived, nl);
print(isderived(base1, meta), nl);
print(isderived(derived, base1), nl);
print(isderived(derived, base2), nl);

//////////////////////////////////////////
// Check the access times. Inheritance based and bounded.
//
t = currenttime();
print("Checking bounded access.\n");
for (i=0; i<1000; ++i)
	meta..x;
print("Elapsed time =", (currenttime() - t) / 1000, nl);

print("Checking un-bounded access.\n");
for (i=0; i<1000; ++i)
	derived.x;
print("Elapsed time =", (currenttime() - t) / 1000, nl);

t = currenttime();
print("Checking average loop (10000) ccess.\n");
for (i=0; i<10000; ++i)
	;
print("Elapsed time =", (currenttime() - t) / 1000, nl);

print("Uninheriting 'base1'\n");
uninherit(derived, base1);
print("***Access via 'derived'\n");
print(derived.x, derived.y, nl);
print(derived.z, derived.w, nl);
print(derived.q, nl);

print("***Access via 'base1'\n");
print(base1.x, base1.y, nl);
print(base1.z, base1.w, nl);
print(base1.q, nl);

print("***Access via 'base2'\n");
print(base2.x, base2.y, nl);
print(base2.z, base2.w, nl);
print(base2.q, nl);

print(base1, nl, derived, nl);
print(isderived(base1, meta), nl);
print(isderived(derived, base1), nl);

print("Uninheriting 'base2'\n");
uninherit(derived, base2);
print("***Access via 'derived'\n");
print(derived.x, derived.y, nl);
print(derived.z, derived.w, nl);
print(derived.q, nl);

print("***Access via 'base1'\n");
print(base1.x, base1.y, nl);
print(base1.z, base1.w, nl);

print("***Access via 'base2'\n");
print(base2.x, base2.y, nl);
print(base2.z, base2.w, nl);

print("***Bounded access via 'meta'\n");
print(meta..x, meta..y, nl);
print(boundedtabget(meta,"x"), boundedtabget(meta,"y"), nl);
print(meta..z, meta..w, nl);
print(boundedtabget(meta,"z"), boundedtabget(meta,"w"), nl);
print(meta..q, nl);

print(base2, nl, derived, nl);
print(isderived(derived, base2), nl);

base = derived = 0;

//////////////////////////////////////////
// Prototype tests.
//

function PointProto() {		// Prototype extraction function
	static proto;
	if (isundefined(proto))
		proto = [
			{"x", "y"	: 0},
			{"class"	: "Point"},
			{"new"		: (member(x,y) {	// Constructor member
								p = tabcopy(self); 
					    		p.x = x; 
								p.y = y; 
				           		return p; 
							})},
			{"print"	: (member(){ 
								print("Point(x=", self.x, ",y=", self.y, ")");
							})}
		];

	return proto;
}

/*
p1 = PointProto().new(30, 40);  // Instance construction via prototype
p1.print();

p2 = PointProto().new(60, 70);	// Same as before
p2.print();
*/

//////////////////////////////////////////

function sig(t) {
	for (local s = "", local n = tablength(t), local i = 0; i < n; ++i)
		if (typeof(t[i]) == "Table" and t[i]..class != nil)
			s += t[i]..class;	// Always bounded access!
		else
			s += typeof(t[i]);
	if (s == "")
		s = "void";
	return s;
}

function metaconstructor() {
	return [ {"construct" : (member() {
							print("Calling metaconstructor.\n");
							f = self.constructors[sig(arguments)];
							return f(|arguments|);
						})}
			];
}

function CircleProto() {

	static proto;

	if (isundefined(proto)) {

		proto = [	{"radius"	: 0},
					{"class"	: "Circle"},
					{"clone"	: (member(){ return  proto.new(self); })},
		    		{ "area"	: (member(){ return pi() * sqr(self.radious); })},
					{ "print"	: (member() { 
									print("Circle(R=", self.radius,",C=");
									self.Point..print();
									print(")");
									})}
		];

		proto.constructors = [
			{"NumberNumberNumber": ( function (x,y,r) {
									local circle = tabcopy(proto);
									inherit(circle, circle.Point = PointProto().new(x,y));
									circle.radius = r;
									return circle;
								})},
			{"PointNumber"		: ( function(p,r){ return proto.construct(p.x, p.y,r); })},
			{"Circle"			: ( function(c){ return proto.construct(c.Point, c.radius); }) }
		];

		inherit(proto, metaconstructor());
	}

	return proto;
}

c1 = CircleProto().construct(-3,-4, 23);
c1.print();

c2 = CircleProto().construct(c1);
c2.x += 17;
c2.y += 34;
c2.print();

//////////////////////////////////////////
// Checking dynamic set of member functions.
//
t1 = ["hello"];
t2 = [	"world", {"f1" : (member(){ print(self, "f1", nl); })},
				 {"f2" : (member(){ print(self, "f2", nl); })}
	];

setmemberfunc(t1, t2, "f1");
setmemberfunc(t1, t2, "f2");
t1.f1();
t2.f1();
t1.f2();
t2.f2();

print("Testing 'owner' handling functions.", nl);
t3=[];
t3.f1 = t1.f1;	// Member owner not yet changed.
print(t1.f1.owner, nl);	// Should print t1
print(t3.f1.owner, nl);	// Should print t1 as well!
t3.f1.owner = t3;	// Should not work!

mf = t3.f1;
mf.owner = t3;
t3.f1 = mf;

print(t3.f1.owner, nl);	// Should print t3 now!

function bindnth (f, x) {
	local t = [	{"x" : x}, 
				{"f" : f},
				{"binder" : (member(){ return self.f(|arguments|, self.x); })}];
	return t.binder;
}

// Testing binders, freezing last argument value.
// Can be easily transformed to binder for any argument.
//
f = bindnth(bindnth((function(x,y){ return x+y; }), 7), 9);
print(f(), nl);


