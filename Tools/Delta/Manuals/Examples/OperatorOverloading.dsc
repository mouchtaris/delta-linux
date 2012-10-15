// An example showing how to use operator overloading in Delta 
// (untyped object-based). To check the values carried inside
// objects, set break points (F9) and use watches or quick watches 
// (Shift-F9) for expressions. To configure the length of debugger
// to string conversions or to set whether all variables in the current
// contetxt are to be received, use menus Configure->Debugger
// AS, 2009.

using std;
const nl = "\n";

////////////////////////////////////////////////////////////
// OPERATOR OVERLOADING, sample for '+' overloading through
// instance methods (analogy to class methods).
//
print("***Simple overloading via class methods (lhs and rhs forms).", nl);

function Point { // Sample factory (returns the prototype through which objects are created).

	if (isundefined(static proto))		// First time initialization.
		proto = [ 	// Protype is a shared trait will common methods and attributes.
			@class	: #Point,
			@Point 	: @self,			// The prototype as a slot too.
			
			method	@ /* to string */	{ return self.class + "(" + self.x + "," + self.y + ")"; },
			method	move(dx,dy)			{ self.x += dx, self.y += dy; },
			
			method	@operator+(a,b)		{ return Point().new(a.x + b.x, a.y + b.y); }, // lhs form (default)
			method	@operator+_(a,b)	{ return a + self.x + self.y; }, // rhs form
			
			method	@operator()			{ print(self, nl); }	// functor, self printing
		];

	if (isundefined(static factory))	// The Point factory actually.
		factory = [
			@x			: 0,
			@y			: 0,
			@prototype	: proto,		// For convention, they refer to the prototype (need not).
			method new(x,y) {			// The constructor method.
				local pt = tabcopy(self);
				pt.x = x, pt.y = y;
				delegate(pt, proto);	// Here we use the built-in delegation inheritance.
				return pt; 
			}
		];
	return factory;
}

p1 = Point().new(1, 2);
p2 = Point().new(3, 4);
(p1+p2)();	// Here we add two points, and invoke the result as a function (to print itself).

/////////////////////////////////////////////////////////////
// OPERATOR OVERLOADING, examples shows how we may combine
// operator functions on the same object via chaining.
//
print("***Overloading via chaining and retry.", nl);

function MakeClass (class) {				// Makes a simple prototype with one method only. 
	return	[
		@class : class,
		method @operator-(x,y) { 			// Overload C-C operator, C a user-defined class.
			if (y.class == self.class) 		// Test that both arguments of the same class id.
				return x.val - y.val;
			return undefined(); 			// Post a retry ('undefined' value is retry in Delta).
		}
	];
}

function MakeInstance (proto, val) {		// Makes a primitive instance linked to its prototype.
	delegate(local a = [ @val : val ], proto);
	return a;
}

function A_minus_B(a, b) {					// The A-B operator as a function.
	if (a.class == #A and b.class == #B)	// Matched class ids.
		return a.val - b.val;
	return undefined();						// No matching, pots a retry.
}

function B_minus_A(b, a) {					// The B-A operator as a global function.
	if (b.class == #B and a.class == #A)	// Matched classes.
		return b.val - a.val;	
	return undefined();						// No matching, retry.
}

// Produces a function object chaining 'func' with the previous
// operator found at 'proto[op]'
//
function ChainOperator(proto, op, func) {
	proto[op] = [
			@f : func, 
			@g : proto[op], 
			method @operator()(x,y) {
				local r = @g(x,y);			// Call the old operator.
				if (isundefined(r))			// Dit it post retry?
					return @f(x,y);			// Then call the new operator.
				else						// The old operator matched the args.			
					return r;				// Else return whethever returned by the old operator.
			}
		];
}

A  = MakeClass(#A);
B  = MakeClass(#B);
a1 = MakeInstance(A, 1); 
a2 = MakeInstance(A, 2);
b1 = MakeInstance(B, 3);
b2 = MakeInstance(B, 4);

ChainOperator(A, "-", A_minus_B);
ChainOperator(B, "-", B_minus_A);

print(a1-a2, nl, b1-b2, nl, a1-b1, nl, b2-a2, nl);

/////////////////////////////////////////////////////////////
// DOT OVERLOADING, applied to emulate remote objects.

print("***Dot overloading showing remote objects.", nl);

function RemoteObject(addr, port, class) {	// Sample factory
	
	// Quick emulation with a local object. Remote methods assumed 'f' and 'g'.
	
	function RNew (addr, port) 		{ /* returns new link*/ return []; }
	function RSet (link, i, v) 		{ /* send set message */ link[i] = v; }
	function RGet (link, i)			{ /* blocking receive of a value */ return link[i]; }
	function RInv (link, v)			{ /* calling a method */ arguments.pop_front(); arguments.pop_front(); print(...); }
	function RIsMeth (link,v)		{ /* testing if method*/ return v == #f or v == #g or iscallable(RGet(link,v)); }
	
	function MakeInvoker (v, link) {
		return [
			@v 		: v, 
			@link 	: link,
			method @operator() 
					{ return RInv(@link, @v, ...); } 
		];
		return v;
	}

	return [
		{ #class : class 			},
		{ #addr  : addr 			},
		{ #port  : port				},
		{ #link  : RNew(addr, port) },
		
		method @ { return tabget(self, #class)+ ":" + tabget(self, #link); },
		
		method @operator.(t,i)		{ 
			if (isoverloadableoperator(i)) // To avoid inquiries of other operators.
				return (i == "tostring()" ? tabget(t, i) : nil);
			if (RIsMeth(@link, i))
				return MakeInvoker(@link, i);
			return RGet(@link, i);
		},
		
		method @operator.=(t,i,v) 	{ 
			if (isoverloadableoperator(i)) // To avoid inquiries of other operators.
				error("can't overload this object");
			RSet(@link, i, v);
		}
	];
}

a = RemoteObject("0.0.0.0", 123, "Point2d");
a.x = 10;							// Remote fields
a.y = 20;							// Remote fields
a.h = (function{ print(...); });	// A callable slot.
a.h("remote call", nl);				// Invoking a user-defined slot.
a.f("hello, remote world!", nl);	// Invoking a remote method.

/////////////////////////////////////////////////////////////
// DOT OVERLOADING, implementing delegation inheritance.
// We name it 'delegate2' since 'delegate' is already a library function
// name in Delta (Delta already supports delegation inheritance).
//

print("***Dot overloading showing delegation inheritance.", nl);

const builtin_get = tabget;
const builtin_set = tabset;

function delegate2 (child, parent) {
	
	function change_self (m, a) {
		if (typeof(m) == "MethodFunc")
			m.self = a;
		return m;
	}
	function get(t,i) { 
		if (isoverloadableoperator(i)) 
			return nil;
		if (not local x = builtin_get(t,i))
			x = builtin_get(t, #parent)[i];
		return change_self(x, t); 
    }

	function set(t,i,x) {
		local parent = builtin_get(t, #parent);
		if (not builtin_get(t,i) and parent[i])
			parent[i] = x;
		else
			builtin_set(t,i,x);
    }	

	child.parent = parent;

	if (not builtin_get(child, ".")) {
        builtin_set(child, ".",  get);
        builtin_set(child, ".=", set);
    }
}

a = [];
b = [ @x : 1, @y : 2, method f(){ return self; } ];
delegate2(a, b);
print(a.x, a.y, nl, a.f(), nl);

/////////////////////////////////////////////////////////////
// FUNCTORS, showing how to easily implement editable binders.
//
print("***Function objects showing mutable binders.", nl);

function bind_n (f...) {
	arguments.pop_front();
	return [
		{ #f, #args: f, arguments },
		method @operator()
			{ return @f(|@args|,...); }
	];
}

p_hw = bind_n(print, "hello", ", world");
p_hw();

function bind(f, x) {
	return [	
		@f : f, @x : x,
		method rebind(y) 	{@x = y; },
		method @operator()	{ return @f(@x,...); }
	];
}

f = bind(bind(print, "hello"), ",world\n");
f();
f.rebind(",everybody\n");	
f();
f.f.rebind("goodbye");	
f();

/////////////////////////////////////////////////////////////
// OPERATOR OVERLOADING with extensible operators.
//

print("***Operator overloading emulating global operator functions.", nl);

(function {
	
	function ExtensibleOperator (op) {
		return [
			@funcs 	: [],
			@op 	: op,

			method install(sig, f) 
				{ @funcs[sig] = f; return self; },
				
			method lookup (sig)
				{ return @funcs[sig]; },

			method @operator()(x,y) {
				if (local f = @funcs[x.class + @op + y.class]) // Get function by signature.
					return f(x,y);
				return undefined();
			}
		];
	}
	
	//************************************//

	function Constructor (proto) {
		return 	tabmethodonme( // Adds a method on an object dynamically.
					proto,
					[	method { 
							local a = [ { #class : @class } ]; 
							delegate(a, self);
							return a;
						}
					][0]
				);
	}
	
	//************************************//

	function BasicOperator (Tleft, Tright) { // Produces an operator function with sig (Tleft, Tright).
		return	bind(
					bind(
						function(Tleft, Tright, l, r) {
							if (l.class==Tleft and r.class==Tright) 
								{ print(Tleft + "-" +Tright, nl); return l; }
							return undefined();
						},
						Tleft
					),
					Tright
				);
	}
	
	//************************************//

	function BasicClass (T, op) { 
		return [ 
			{ #class : 	T },
			{ #new 	 : 	Constructor(@self) },
			{ op 	 : 	ExtensibleOperator(op).install(T + op + T, BasicOperator(T,T)) }
		]; 
	}

	//*************************************//

	function A { // A prototype.
		if (isundefined(static proto)) 	proto = BasicClass("A", "-"); 
		return proto;
	}
	
	function B { // B prototype.
		if (isundefined(static proto))	proto = BasicClass("B", "-");
		return proto;
	}
	
	function C { // C prototype.
		if (isundefined(static proto))
			proto = [ {#class : "C" }, { #new : Constructor(@self) } ]; 
		return proto;
	}
	
	//************************************//
	
	function Point2d (x,y) {
		return [
			{ #class, #x, #y : #Point2d , x, y },
			method @ { return "Point2d(" + @x + "," + @y + ")"; },
			{ = 	 : 	ExtensibleOperator("=").install(	// Overloading assignment
							"Point2d=Point2d",
							function(a,b) { 
								if (a.class==#Point2d and b.class==#Point2d)
									{ a.x = b.x; a.y = b.y;  return a; }
								return undefined();	// retry
							}
			) }
		];
	}
	
	function Point3d (x,y,z) {
		return [
			{ #class, #x, #y, #z  : #Point3d, x, y, z },
			{ =() 	 : 	ExtensibleOperator("=").install(	// Overloading type casting
							"Point2d=Point3d",
							function(a,b) { 
								if (a.class==#Point2d and b.class==#Point3d)
									{ a.x = b.x; a.y = b.y; return a; }
								return undefined();	// retry
							}
			) }
		];
	}

	//************************************//
	
	(function {	// Main example
		
		A(). - .install("A-B", BasicOperator(#A, #B)); 
		B(). - .install("B-A", BasicOperator(#B, #A)); 
		C(). -  = ExtensibleOperator("-").install("C-C", BasicOperator(#C,#C)).install("C-B", BasicOperator(#C,#B));
		C(). -_ = ExtensibleOperator("-").install("B-C", BasicOperator(#B,#C));

		setarithmeticoperator("-", local sub_global = ExtensibleOperator("-"));
		sub_global	.install("A-B", A().-.lookup("A-B"))
					.install("B-A", B().-.lookup("B-A"))
					.install("C-C", C().-.lookup("C-C"))
					.install("C-A", BasicOperator(#C,#A))
					.install("A-C", BasicOperator(#A,#C));

		local a = A().new();
		local x = a-a;
		
		local b = B().new();
		x = a-b;
		x = b-a;
		
		local c = C().new();
		x = c-c;
		x = b-c;
		x = c-b;
		
		x = c-a;
		x = a-c;
		
		p1 = Point2d(1,2);
		p2 = Point2d(3,4);
		print(p1 = p2, nl);
		p3 = Point3d(5,6,7); 
		print(p1 = p3, nl);
		
	})();

})();

/////////////////////////////////////////////////////////////
// OPERATOR OVERLOADING for slot get / set operations.

print("***Operator overloading on attribute objects.", nl);

t = [
	// Untyped version of the property pattern in Delta. The general syntax is:
	// @<ident> { @set <callable value> @get <callable value> }
	
	@x {	
		@set method(v) 	{ @x = v; }
		@get method 	{ return @x; } 
	},
	
	{ #x, #y : [ // The attribute 'x' will also be set as an obect overloading = and =()
			@val : 0 , 
			method @operator=(lv, rv)   
				{ print(rv, nl); return @val = rv; } ,
			method @operator=()(lv, rv) 
				{ print(rv, nl); return self; } 
	] }
];

t.x = 10;	// calls set method and then the overloaded assign op 
t 	= t.y;	// calls get method and then overloaded type casting 
t 	= 11;	// calls overloaded assign op

// Also, operators may be easily overloaded using their lexeme
// directly as a key in dot expressions.

x = [];
x.= 	= ( function { print("unused=\n");		} );
x.== 	= ( function { print("unused==\n");		} );
x.==_ 	= ( function { print("unused==_\n");	} );
x.=() 	= ( function { print("unused=()\n");	} );
print(x);

/////////////////////////////////////////////////////////////
// Attributes can be very dynamic too (not relating to 
// overloading per se).

print("\n\n*****Dynamic attributes******\n");

tt = [
	// Static definition, i.e. by syntax, of an attribute.
	@x { 
		@set method(x) { @x = x; }  // setter comes syntactically first.
		@get method { return @x; } 
	}
];
tt.x = 10;				// will set @x to 10
print("@x=", tt.x, nl);	// will print 10

tabredefineattribute(
    tt, #x, 	// Dynamic redefinition of previously static attribute.
    tabmethodonme(tt, method(x){ @x = 2 * x;   } ),	// setter
    tabmethodonme(tt, method   { return 2* @x; } )	// getter, returns twice of its val
);
tt.x = 2;				// will set @x to 4
print("@x=", tt.x, nl);	// will print 8

tabnewattribute(
    tt, #y, 	// Dynamic addition of new attribute.
    tabmethodonme(tt,  method(x){ @y = x + self.x; } ),	// setter, uses x attr
    tabmethodonme(tt,  method   { return @y;       } )	// getter   
);

tt.y = 3;				// will set @y to 11
print("@y=", tt.y, nl);	// will print 11

tabsetattribute(tt, #x, 12);	// will set directly @x to 12
print("@x=", tt.x, nl);			// will print 24
print("x=", tabgetattribute(tt, #x), nl);	// will print 12

tt.x = nil;				// remove attr
tt.x = 2;				// make it a normal slot.
tt.y = 4;				// wil set @y to 6
print("@y=", tt.y, nl);	// will print 6
