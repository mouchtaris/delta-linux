// Delta.
// IMPLEMENTATION OF THE EXAMPLES (A. SAVIDIS OOPSLA paper proposal).
// 03/2006

const nl	= "\n";
const nl2	= nl + nl;
const sp	= " ";
const sp2	= sp + sp;

/////////////////////////////////////////////////////
// Figure 1: PROTOTYPES.
/////////////////////////////////////////////////////

print("****PROTOTYPES***", nl);

function PointProto() {
	if (isundefined(static proto))
		proto = [
			{"x", "y" : 0 },
			{"class"  : "Point" },
			{"clone"  : (member() {
							return objcopy(self);
						})},
			{"new"    : (member(x,y) {
							p = self.clone(); 
							p.x = x; p.y = y; 
		    				return p; 
 						})},
			{"tostring" : (member() { 
								return "Point(" + 
										self.x	+
										","		+ 
										self.y	+
										")"; 
							})}
		];
   return proto;
}

p1			= PointProto().new(1,2);
clone		= p1.clone;		// 'clone' is a member value, owner is p1
p2			= clone();		// Same like p1.clone()
clone.owner = p2; 			// Changes instance owner from p1 to p2
p3			= clone();		// Same as p2.clone()

// Lets print the instances to verify the differences.
//
print(p1, nl2, p2, nl2, p3, nl2);

/////////////////////////////////////////////////////
// Figure 2: COMPOSER.
/////////////////////////////////////////////////////

print("****COMPOSER***", nl);

function compose(F,G) {
	t = [ 
		{"F"    : F}, 
		{"G"    : G},
		{"COMP" : (member(){ 
					return self.F(|self.G(|arguments|)|); 
				})}
	];

   return t.COMP;
}

mul2		= (function(x,y){ return x * y; });
sqr2		= (function(x,y){ return [ sqr(x), sqr(y) ]; });
mul2sqr2	= compose(mul2, sqr2);

x = mul2sqr2(3,7);
print(mul2, nl, sqr2, nl, mul2sqr2, nl, x, nl);

function functoriser(x) {
	return [ {"()":(member(){return self.x;})}, {"x" : x }];
}

/////////////////////////////////////////////////////
// Figure 3: ALTERING MEMBER FUNCTIONS ON THE FLY.
/////////////////////////////////////////////////////

print("****ALTERING MEMBER FUNCTIONS***", nl);

function Proto() {
	 if (isundefined(static t))
		t = [
			{"a"	: 2006 },
			{"b"	: 2007 },

			{"f1"	: (member(){ print("original f1:", self.a, nl); })},
			{"f2"	: (member(){ print("original f2:", self.b, nl); })},

			{"new"	: (member(){return objcopy(self); })}
      ];
	  return t;
}

fjvariations = [ 
       (member(){ print("updated f1:", self.b, sp, self.a, nl); }),
       (member(){ print("updated f2:", self.a, sp, self.b, nl); })
];

o1 = Proto().new();
o2 = Proto().new();

o1.f1(); o1.f2();
o2.f1(); o2.f2();

// We update f1 for o1.
//
f1			= fjvariations[0]; 
f1.owner	= o1; 
o1.f1		= f1;

// We update f2 for o2.
//
f2			= fjvariations[1]; 
f2.owner	= o2; 
o2.f2		= f2;

o1.f1(); o1.f2();
o2.f1(); o2.f2();

/////////////////////////////////////////////////////
// Figure 4: IMPLEMENTING GENERIC COLLECTIVE MANAGEMENT.
/////////////////////////////////////////////////////

print("****GENERIC COLLECTIVE MANAGEMENT***", nl);

function deferred_call(f) {
	// Copy original call arguments, except f itself (0 argument).
	//
	for (local args=[], local i=1; i<tablength(arguments); args[i-1]=arguments[i], ++i)
		;

	t = [
		{"func" : f}, 
		{"args" : args},
		{"call" : (member(){ self.func(|self.args|); })}
	];
	return t.call;
}

function collective_management() {
	if (isundefined(static t))
		t = [
			{"n"		: 0},
			{"calls"	: []},
			{"add"		: (member(c){ self.calls[self.n++] = c; })},
			{"commit"	: (member(){
							for (local i=0; i<self.n; ++i)
								{ self.calls[i](); self.calls[i] = nil; }
							self.n = 0;
							})}
		];
	return t;
}

collective_management().add(deferred_call(print, "hello", ", world!", nl));
collective_management().add(deferred_call(o1.f1));
collective_management().add(deferred_call(o2.f2));
collective_management().add(deferred_call(print, compose(mul2, sqr2)(2,4), nl));

collective_management().commit();

function bind_1st (f, x) {
	local t = [	{"x" : x}, 
				{"f" : f},
				{"binder" : (member(){ return self.f(self.x, |arguments|); })}];
	return t.binder;
}

collective_management().add(
	bind_1st(bind_1st(bind_1st(print, "hello"), ", world!"), nl)
);
collective_management().commit();

/////////////////////////////////////////////////////
// Figure 9 and 10: DYNAMIC INHERITANCE EXAMPLES.
/////////////////////////////////////////////////////

print("****DYNAMIC INHERITANCE***", nl);

function CircleProto() {
	if (isundefined(static proto))
		proto = [	{"R"		: 0},
					{"class"	: "Circle"},
					{"clone"	: (member(){
										return  proto.new(
													self.pt.x,
													self.pt.y, 
													self.R
												);
									})},
					{"new"		: (member(x,y,R) {
										p	= objcopy(self);
										p.R = R;
										p.pt = PointProto().new(x,y);
										inherit(p, p.pt);
										return p;
									})}
		];
	return proto;
}			

c = CircleProto().new(0,1,2);
print(c, nl);
print(c.class, c.pt.class, c.pt..class, nl);	// CircleCirclePoint

// Dynamic inheritance control functions.
//
c = [];
a = [{"x":1}];
inherit(c, a);
print(c.x, nl);
b = [{"x":2}, {"y":2}];
inherit(c, b); 
print(c.x, sp, c.y, sp, b..x, sp, b..y, nl);
d = [{"z":3}, {"y":3}];
inherit(b, d); 
print(c.z, sp, b.z, sp, b.y, sp, d..y, nl);

uninherit(c, a);
print(c.x, sp, isderived(c, a), nl);

/////////////////////////////////////////////////////
// Figure 12 and 32: STATE PATTERN.
/////////////////////////////////////////////////////

print("****STATE PATTERN***", nl);

// State pattern implemenetd via inheritance.
// The same API with different semantics.
//
st1 = [{"id": "A"}, {"f" : (member(){ print(self.id, nl); })}];
st2 = [{"id": "B"}, {"f" : (member(){ print("*", self.id, "*", nl); })}];

// Firstly as a function.
//
function setstate(a,s) {
	if (a.state != nil)
		uninherit(a, a.state);
	inherit(a, s.state = s);
}

a =[];
setstate(a, st1);
a.f();

setstate(a, st2);
a.f();

// Then as an inherited property!
//
function StatePattern() {
	return [
		{"setstate" : (member(s) {
			if (self.state)
				uninherit(self, self.state);
			inherit(self, self.state = s);						
		})}
	];
}

b = [];
inherit(b, StatePattern());

b.setstate(objcopy(st1));
b.f();

b.setstate(objcopy(st2));
b.f();

/////////////////////////////////////////////////////
// Figure 18: PROTOTYPE MIXER FUNCTION.
/////////////////////////////////////////////////////

print("****PROTOTYPE MIXER***", nl);

function mixer() {

	local n	= tablength(arguments)-1;

	local proto = [ 
		{"mixin"	: arguments[0]},
		{"n"		: n},
		{"new"		: (member(){

						local args	= [];
						local j		= 0;
						if (arguments[0] == self.mixin.class)
							{ args = arguments[1]; j += 2; }

						local obj = self.mixin.new(|args|);
						obj.numbases = self.n;

						for (local i=0; i < self.n; ++i) {
							args = [];
							local proto = self["super" + byte(i)];
							if (arguments[j] == proto.class)
								{ args = arguments[j+1]; j += 2;  }

							base = proto.new(|args|);
							inherit(obj, base);
							obj["base" + byte(i)] = base;
						}
						return obj;
					})}
	];

	for (local i=0; i<=n; ++i)
		proto["super" + byte(i)] = arguments[i+1];

	return proto;					
}

function ProtoMan(){
	if (isundefined(static proto))
		proto = [
			{"class" : "man"},
			{"new" : (member(s) {
						local p		= objcopy(self);
						p.name		= s;
						return p;
					})},
			{"print" :(member(){ print(self.name); })}
		];
	return proto;
}

function ProtoDoctor() {
	if (isundefined(static proto))
		proto = [
			{"class" : "doctor"},
			{ "new" :(member(s){ 
					local p		= objcopy(self); 
					p.spec		= s;
					return p; 
				})},
			{ "print" :(member(){ print("Dr " + self.name + ", " + self.spec); })}
		];	
	return proto;
}

MixedProto = functoriser(mixer(ProtoDoctor(), ProtoMan()));
dm = MixedProto().new("doctor", ["Pathologist"], "man", ["John Smith"]);
dm.print();
print(nl, MixedProto(), nl, dm, nl);

/////////////////////////////////////////////////////
// Figure 19, 20, 21: FUNCTION OVERLOADING.
/////////////////////////////////////////////////////

print("****FUNCTION OVERLOADING***", nl);

function sig(t) {
	for (local s = "", local n = tablength(t), local i = 0; i < n; ++i)
		if (typeof(t[i]) == "Table" and t[i]..class != nil)
			s += t[i]..class;
		else
			s += typeof(t[i]);
	if (s == "")
		s = "void";
	return s;
}

function overloadable(){
	return [
		{"()"	: (member(){
					local g = self.D[sig(arguments)];
					if (not g)
						g =  self.D["..."];
					return g(|arguments|);
				})},
		{"set"	: (member(s,f){ self.D[s] = f; })},
		{"get"	: (member(s,f){ return self.D[s]; })},
		{"D"	:[]}			
	];
}

function overload(){
	inherit(local f=[], overloadable());
	for (local i = 0; i < tablength(arguments); i += 2)
		f.set(arguments[i], arguments[i+1]);
	return f;
}

// Overloading non-member functions.
//
f = overload(
		"NumberNumber", (function(x,y){ return x+y; }),
		"BoolBool", (function(a,b){ return a or b; }),
		"PointPoint",	(function(p1,p2) { return PointProto().new(p1.x+p2.x, p1.y+p2.y); })
	);

print(f(3,6), nl);
print(f(true, false), nl);
print(f(p1, p2), nl);
f.set("BoolBool", (function(){ return "BoolBool not allowed!";}));
print(f(true, false), nl);

// Overloading member functions.
//
obj = [
	{"f" : overload(
				"Number",	(member(y){ return self.x + y; }),
				"Bool",		(member(b){ return self.a and b; })
			)},
	{"x" : 10},
	{"a" : false}
];

print(obj.f(7), nl);
print(obj.f(true), nl);

print2 = overload(
			"Point", (function(pt){ print("Point(", pt.x, ",", pt.y, ")", nl); }),
			"...", print
		);
print2(PointProto());
print2(1,2,3,4,5,nl);

// Updating the a function version for an overloaded
// member function.
//
function owned(m,b) { m.owner = b; return m; }

obj.f.set(
	"Number",
	owned((member(y){return self.x * y;}), obj)
);
print(obj.f(7), nl);

function overload2(){
	inherit(local f=[], overloadable());
	for (local i=0; i<tablength(arguments); ++i) {
		local g = arguments[i];
		for (local t=tabindices(g.D), local j=0; j<tablength(t); ++j)
			f.set(t[j], g.get(t[j]));
	}
	return f;
}

ff = overload2(f, obj.f);
print(ff(true, false), nl);
print(ff(7), nl);

// Constructor overloading, showing virtual constructors too.
//
function ShapeProto() {
	if (isundefined(static proto))
		proto = [
					{"class" :	"Shape"},
					{"new"	 :	overload(
									"...",
									(member(){ return objcopy(self); })
								)}							
		];
	return proto;
}

ShapeProto().new.set(
	"NumberNumber", 
	PointProto().new
);
ShapeProto().new.set(
	"NumberNumberNumber", 
	CircleProto().new
);

sh1 = ShapeProto().new(10, 20);
print(sh1.class, nl);
sh2 = ShapeProto().new(6, 5, 12);
print(sh2.class, nl);
sh3 = ShapeProto().new();
print(sh3.class, nl);

function PointProto2() {
	if (isundefined(static proto))
		proto = [
			{"x", "y" : 0 },
			{"class"  : "Point" },
			{"clone"  : (member() {
							return objcopy(self);
						})},
			{"new"    : overload(
							"NumberNumber",
							(member(x,y) {
								p = self.clone(); 
								p.x = x; p.y = y; 
		    					return p; 
 							}),
							"Point",
							(member(pt) {
								return self.new(pt.x, pt.y); 
 							})
						)}
		];
   return proto;
}

// Multi-methods.
//

a = [	{"class" : "A"}, 
		{"f" : overload("AA", (member(a1,a2){ print(self.x, a1.x, a2.x, nl); }))},
		{"x" : 10}	];

aa = objcopy(a);
aa.x = 20;

a.f(aa,aa);

a1 = [	{"class" : "A1"}, 
		{"y" : "hello"}	];
inherit(a1,aa);
a1.f.set("A1A", owned(
					(member(a1,a2){ print(self.y, a1.y, a2.x, nl);}),
					a1
				));

a1.f.set("AA1", owned(
					(member(a1,a2){ self.f.get("A1A")(a2,a1); }),
					a1
				));

aa = objcopy(a);
aa.x = 20;

a.f(aa,aa);


/////////////////////////////////////////////////////
// POLYMORPHIC MEMBER FUNCTIONS
/////////////////////////////////////////////////////
print("****POLYMORPHIC MEMBERS***", nl);

t0 =[ (member(){print(self.x, nl);}), (member(){print(self.y, nl);}) ];
t1 =[ {"x" : "hello"}];
t2 =[ {"y" : ", world"}];

t0[0].owner = t1;
t0[1].owner = t2;

t0[0](); 
t0[1]();


/////////////////////////////////////////////////////

print("****OPERATOR OVERLOADING***", nl);

// Set an overloaded assignment of PointProto.
//
PointProto()["="] = (function(t1,t2) { t1.x = t2.x; t1.y = t2.y; });
p1 = PointProto().new(1,2);
p2 = PointProto().new(3,4);
print(p1, nl, p2, nl);
p1 = p2;
print(p1.tostring(), nl, p2.tostring(), nl);

/////////////////////////////////////////////////////

try {
	print("****LMBDA***", nl);
	factorial  = (function(n){  return (n == 1 ? 1 : lambda(n-1)*n);  });
	print(factorial(8));
} trap e {
	throw e;
}

print("After exception handling.\n");
