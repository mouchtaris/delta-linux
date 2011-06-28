// STL importing (class objects, iteration, algorithms).
// Member lookup cache testing.
// Mixin composition.
//
// A. Savidis, 2006-2008.
//
using std;
const nl	= "\n";
const space = ", ";
const lpar	= "(";
const rpar	= ")";

//------------------------------------------------------
// CLASS OBJECT (PROTOTYPE) GENERATION FROM LIBRARY FUNCTIONS.
// CONVERT A PROCEDURAL API TO AN OO API.
//

////////////////////////////////////////////////////////
// Polymorphic binder.
//
function bind_1st (f, x) {
	return [	
		{ "x"	: x }, 
		{ "f"	: f },
		(method(){
			return @f(@x, |arguments|); 
		})
	][0];
}

////////////////////////////////////////////////////////
// 'funcs'	: array of lib function names without the prefix.
// 'prefix'	: common prefix of lib function names.
// 'ctor'	: the external object constructor.
// 'extra'	: a functor installing extra methods
//
function Prototyper(funcs, class, prefix, ctor, more) {
	return [
		{ "funcs"	: funcs	},
		{ "prefix"	: prefix},
		{ "class"	: class	},

		{ .more		: more	},
		{ .ctor		: ctor	},

		{ .new		:(method(){
						local fs = @funcs;
						local a	 = [ 
									{ .nativeobject : @ctor(|arguments|) },
									{ .class	: @class }
								];
						for (local i = 0; i < tablength(fs); ++i)
							a[ fs[i] ] = bind_1st(@prefix + fs[i], a.nativeobject);
						if (@more) 
							@more(a);
						return a;
					})}
		];
}

////////////////////////////////////////////////////////
// Adding to library containers begin / end iterators.
//
function LibContainerMore(cont) {
	tabextend(
		cont,
		[	{ .begin	:	(method(){
								local i = @iterator();
								i.setbegin(@nativeobject);
								return i;
							})},

			{ .end		:	(method(){
								local i = @iterator();
								i.setend(@nativeobject);
								return i;
							})}
		]
	);
}

////////////////////////////////////////////////////////
// Adding to library iterators comparison and 
// assignment capabilities.
//
function LibIteratorMore(iter) {
	tabextend(
		iter,
		[	{ ==	:	(function(t, x) {
							assert t.class == x.class;
							return t.equal(x.nativeobject);
						})},

			{ !=	:	(function(t, x) {
							assert t.class == x.class;
							return not t.equal(x.nativeobject);
						})},

			{ =		:	(function(lv, x) {
							if (lv.class == x.class)
								{ lv.assign(x.nativeobject); return lv; }
							 else
								return x;
						})}
		]
	);
}

//------------------------------------------------------
// GENERIC TABLE ADAPTATION FOR ITERATION.
//

////////////////////////////////////////////////////////
// An iterator adapter for tables, as a super prototype.
// Implicit methods: table, curr, end. 
// Required 'retarget' and '='.
//
function TableIteratorSuper() {

	local  t =	[

		//******************************
		// Internal methods. Preconditions and invariant.
		//
		{ .invariant	: (method() {
								return	@table		and
										@end > 0	and
										@curr >= 0;
						})},

		{ .pre_access	: (method() {
							return	@invariant()	and
									@curr >= 0		and
									@curr < @end;
						})},

		{ .pre_checkend: (method(t) {
							return	@table == t		and
									@invariant()	and
									@curr <= @end;
						})},
						
		//******************************
		// Public basic methods.
		//
		{ .setbegin	:	(method(t) { @retarget(t); })},

		{ .checkend	:	(method(t) { 
							assert @pre_checkend(t);
							return @end == @curr;
						})},

		{ .setend	:	(method(t) {
							@retarget(t); 
							if (@end)
								@curr = @end;
						})},

		{ .fwd		:	(method(){
							assert @pre_access() and @curr != @end;
							++@curr;
						})},

		{ .bwd		:	(method(){
							assert	@pre_access() and
									@curr;
							--@curr;
						})},

		//******************************
		// Public basic overloaded operators.
		//
		{ ==	:	(function(r,i) {	// Iterator equality.
						assert	r.class == i.class and r.table == i.table;
						return	r.curr == i.curr;
					})},

		{ !=	:	(function(r,i) {	// Iterator equality.
						assert	r.class == i.class and r.table == i.table;
						return	r.curr != i.curr;
					})}
	];

	t.nativeobject = t;
	return t;
};

////////////////////////////////////////////////////////
// Augments a table with inherited methods to
// behave as an iteratable container. The interesting issue
// is that, via inheritance, iteration on the original table
// is not affected (methods are not added, but inherited).
// It accepts the container class id and the iterator factory
// function.
//
function AddTableIterationMembers(t, class, iteratorfactory) {
	
	assert typeof(t) == "Table" or typeof(t) == "Object";

	if (t.class)	// If already an augmented iterator.
		return t;
	else {
		inherit(
			t,
			[
				{ "class"	:	class },	// To avoid re-augmenting.

				{ .iterator :	iteratorfactory },

				{ .begin	:	(method(){
									local i = @iterator();
									i.setbegin(@nativeobject);
									return i;
								})},

				{ .end		:	(method(){
									local i = @iterator();
									i.setend(@nativeobject);
									return i;
								})},

				{ .total	:	(method(){
									return tablength(@nativeobject);
								})},

				{ "nativeobject" : t }
			]
		);

		return t;
	}
}
//------------------------------------------------------
// ITERATOR SPECIFICALLY FOR ASSOCIATIVE TABLES.

////////////////////////////////////////////////////////
// An iterator for general associative tables.
// Derived from TableIteratorSuper using extension
// as an implicit inheritance method.
//
function GeneralTableIterator() {
	tabextend(
		local t = TableIteratorSuper(),
		[
			{ .retarget	: (method(t) {
								@table		= t;
								@indices	= tabindices(t);
								@curr		= 0;
								@end		= tablength(@indices);
							})},

			{ "class"	:	"tableiter" },

			{ .setval	:	(method(x) {
								assert @pre_access();
								@table[ @indices[ @curr ] ] = x;
							})},

			{ .getval	:	(method() {
								assert @pre_access();
								return [
									{ .index	: local i = @indices[@curr] },
									{ .value	: @table[i] }
								];
							})},

			{ =			:	(function(lv, i) {	// Iterator assignment.
								if (i.class != "tableiter")
									return i;
								else {
									lv.table	= i.table;
									lv.indices	= i.indices;
									lv.curr		= i.curr;
									lv.end		= i.end;
									return lv;
								}
							})}
	
		]	// End of extended methods.
	);

	return t;
};

//------------------------------------------------------
// VECTOR AND ITS ITERATORS.

////////////////////////////////////////////////////////
// An iterator for vectors treating tables as arrays.
// Derived from TableIteratorSuper using extension
// as an implicit inheritance method.
//
function VectorTableIterator() {
	tabextend(
		local t = TableIteratorSuper(),
		[
			{ .retarget	: (method(t) {
								@table		= t;
								@curr		= 0;
								@end		= tablength(t);
							})},

			{ "class"	:	"vectoriter" },

			{ .setval	:	(method(x) {
								assert @pre_access();
								@table[ @curr ] = x;
							})},

			{ .getval	:	(method() {
								assert @pre_access();
								return @table[@curr];
							})},

			{ =			:	(function(lv, i) {	// Iterator assignment.
								if (i.class != "vectoriter")
									return i;
								else {
									lv.table	= i.table;
									lv.curr		= i.curr;
									lv.end		= i.end;
									return lv;
								}
							})}
		]	// End of extended methods.
	);

	return t;
};

////////////////////////////////////////////////////////

function Vector() {
	
	// Make instance and add iterator methods.
	//
	AddTableIterationMembers(
		local t = [],
		"vector", 
		VectorTableIterator
	);
	
	// Add the supplied arguments as elements.	
	foreach(i : x, arguments)
		t[i] = x;

	// Extend with vector specific methods.
	// Extend with vector specific methods.
	//
	inherit(
		t,
		[
			{.push_back	: (method(x) {
								@nativeobject[@total()] = x;
							})},

			{.pop_back	: (method() {
								@nativeobject[@total()] = nil;
							})}
		]
	);

	return t;
}

//------------------------------------------------------
// Adapting containers before iteration table.
//
function IterationContainerAdapter(t) {
	return	AddTableIterationMembers(
				t, 
				"table", 
				GeneralTableIterator
			);
}

////////////////////////////////////////////////////////

prototypes = [ 

	{ .list :	[
			{ .funcs : [
					"push_back",	"push_front",	"pop_back",
					"pop_front",	"insert",		"remove",		
					"erase",		"checkend",		"total"
				]
			},
			{ .class  : "list" },
			{ .prefix : "std::list_" },
			{ .ctor	  : list_new },
			{ .more	  : (function(l) {
							LibContainerMore(l);
							l.iterator = prototypes.listiter.new;
						})}
		]
	},

	{ .listiter:	[
			{ .funcs : [
					"setbegin",		"setend",		"fwd",
					"bwd",			"getval",		"setval",
					"checkend",		"equal",		"assign"
				]
			},
			{ .class  : "listiter" },
			{ .prefix : "std::listiter_" },
			{ .ctor	  : listiter_new },
			{ .more	  : LibIteratorMore }
		]
	}
];

////////////////////////////////////////////////////////
// Prototype maker function.
//
(function() {
	for (	local ti = tabindices(prototypes),
			local i = 0;
			i < tablength(ti);
			++i	) {

			// The prototype substitutes the prototype data.
			//
			t = prototypes[ ti[i] ];
			print("\n*****\n", t, "\n*****\n");
			prototypes[ ti[i] ] =	Prototyper(
										t.funcs,
										t.class,
										t.prefix, 
										t.ctor, 
										t.more
									);
	}
})();	// This is called only once at start-up.

prototypes.vector		=  [ {.new : Vector } ];
prototypes.vectoriter	=  [ {.new : VectorTableIterator } ];

////////////////////////////////////////////////////////
// Test program.
//
function PrintContainer (cont) {
	local t = IterationContainerAdapter(cont);
	for (	local i = t.begin();
			i != t.end(); 
			i.fwd()	)
			print(i.getval(), nl);
}
PrintContainer(prototypes.list.new(1,2,3, "hello", true, false, PrintContainer));
PrintContainer(["one"]);
PrintContainer(["one", "two", "three", { "and" : "four" }]);
PrintContainer(prototypes.vector.new(7, 15, 19, 42, [], "that"));

////////////////////////////////////////////////////////

print("******* Some tests follow *******\n");

////////////////////////////////////////////////////////
// Table access operators overloaded. A comprehensive test.
//

t = [
	{"."  : (function(t,i) { 
				if (isoverloadableoperator(i)) return nil;
				return 1969; 
			})}
];
print("Expected 1969 1969: ", t.x, space, t..y, nl);	// 1969 1969

tt = [
	{"."  : (function(t,i) { 
				if (isoverloadableoperator(i)) return nil;
				return 9; 
			})}
]; 
inherit(tt, t);
print("Expected 9 9: ", tt.x, space, tt..y, nl);	// 9 9
print("Expected 9 1969: ", t.x, space, t..y, nl);	//  9 1969

ttt = [
	{"x" : 18 },
	{"y"  : 2003 }
]; 
inherit(ttt, tt);
print("Expected 18 2003: ", ttt.x, space, ttt..y, nl);	//  18 2003
print("Expected 18 9: ", t.x, space, tt..y, nl);	//  18 9

t = [
	{"x" : 1974 },
	{"." : (method(t,i) { 
				assert equal(t, self); 
				if (isoverloadableoperator(i)) return nil;
				return tabget(t, "x"); 
			})}
];
print("Expected 1974:", t.a, nl);

////////////////////////////////////////////////////////
// DELEGATION PATTERN.
// Forwards method requests not handled at client side
// to server, without inheriting from server directly.
// The server may be changed.
//
function setdelegate(client, server) {
	inherit(
		client,
		[ 
			{"server"	: server },

			{"."		: (function(t, i) { 
								return tabget(t, "server")[i];
							})},

			{".="		: (function(t, i, c) { 
									tabget(t, "server")[i] = c;
								})}
		]
	);
}

server = [
	{ "x" : 101 },
	{ "y" : 202 },
	{"server": (method() { return self; }) }
];

client = [];
setdelegate(client, server);
print("Expected 101, 202: ", client.x, space, client.y, nl);
print("Server:", space, client.server(), nl);

////////////////////////////////////////////////////////
// The followinhg shows that 'self' is invariant to the message
// receiver, always bound to the method owner subobject.
//
a = [ {.f : (method() { return self; })} ];
b = [];
inherit(b, a);
c = [];
inherit(c, b);
print("All three expected to be the same object", nl, a.f(), nl, b.f(), nl, c.f(), nl);

////////////////////////////////////////////////////////
// Cache memory test.
//
a = [{"y" : 7}];
b = [{"x" : 9}];
c = [{"z" : 11}];
inherit(c,b);
inherit(c,a);

d = [{"q" : 13}];
e = [{"w" : 15}];
inherit(e,c);
inherit(e,d);

print("Loop 1", nl);
print("Please wait...", nl);
elapsed = 0;
const NLOOPS = 500;
for (i=0; i<NLOOPS; ++i) {
	local start = currenttime();
	local x = e.x;
	local y = e.y;
	local z = e.z;
	local q = e.q;
	local w = e.w;
	elapsed += currenttime() - start;
	if (i == NLOOPS-1)
		print("Expected 9, 7, 11, 13, 15: ", x, space, y, space, z, space, q, space, w, nl);
}

print("Elapsed = ", elapsed, nl);

print("Loop 2", nl);
print("Expected ", e.x+NLOOPS, space, e.y+NLOOPS, space, e.z+NLOOPS, space, e.q+NLOOPS, space, e.w+NLOOPS, " :", nl);
print("Please wait...", nl);
elapsed = 0;
for (i=0; i<NLOOPS; ++i) {
	local start = currenttime();
	++e.x;
	++e.y;
	++e.z;
	++e.q;
	++e.w;
	elapsed += currenttime() - start;
	if (i == NLOOPS-1)
		print(e.x, space, e.y, space, e.z, space, e.q, space, e.w, nl);
}

print("Elapsed = ", elapsed, nl);

////////////////////////////////////////////////////////
// EXAMPLES FOR 'JOT' SUBMISSION.
////////////////////////////////////////////////////////
//

print("*****JOT EXAMPLES*****", nl);
print("Dynamic mixin inheritance:", nl);

// Assume a window manager storing references of window objects
// upon their construction.
//
function WindowManager(){
	static wm;
	if (isundefined(wm))
		wm =	[
			{ .windows	: prototypes.list.new() },
			{ .Register	: (method(win){ @windows.push_back(win); })},
			
			// Calls the display method of every registered window.
			// Stored window subobjects may well belong to a derived object
			// refining Display().
			//
			{ .Display	: (method() { 
							local t = IterationContainerAdapter(@windows);
							for (local i = t.begin(); i != t.end(); i.fwd())
								i.getval().Display();
						})}	
		];
	return wm;
}

//**********************

// Window class object.
//
function Window() {
	static proto;
	if (isundefined(proto))
		proto = [
			{.class		: "Window" },
			{.Display	: (method(){ print("Window::Display()\n"); })},
			{.Handle	: (method(event){ print("Window::Handle(", event, ")\n"); })},
			{.New 		: (method(){ 
							local win = tabcopy(self);
							WindowManager().Register(win);
							return win;
						})}
		];
	return proto;
}

//**********************
// Button class object (inherits from Window).
//
function Button() {
	static proto;
	if (isundefined(proto))
		proto = [
			{.class		: "Button" },
			{.Display	: (method(){ print("Button::Display()\n"); })},
			{.Handle	: (method(event){ print("Button::Handle(", event, ")\n"); })},
			{.New 		: (method(){ 
							local button = tabcopy(self);
							inherit(button, Window().New());
							return button;
						})}
		];
	return proto;
}

//**********************
// AnimatedButton class object producing instances that
// are set upon construction as mixins over Buttons.
//
function AnimatedButton() {
	static proto;
	if (isundefined(proto))
		proto = [
			{.class		: "AnimatedButton" },
			{.Display	: (method(){ print("AnimatedButton::Display()\n"); })},
			{.Handle	: (method(event){ print("AnimatedButton::Handle(", event, ")\n"); })},
			{.New 		: (method(button){ 
							local anim = tabcopy(self);
							inherit(anim, button);
							return anim;
						})}
		];
	return proto;
}

//**********************
// AnimatedButton class producing instances that
// are set upon construction as mixins over Buttons.
//
function AuditoryButton() {
	static proto;
	if (isundefined(proto))
		proto = [
			{.class		: "AuditoryButton" },
			{.Display	: (method(){ print("AuditoryButton::Display()\n"); })},
			{.Handle	: (method(event){ print("AuditoryButton::Handle(", event, ")\n"); })},
			{.New 		: (method(button){
							local audi = tabcopy(self);
							if (arguments[0])
								inherit(audi, button);
							return audi;
						})}
		];
	return proto;
}

//**********************
// DYNAMIC MIXIN INHERITANCE (INHERIT / UNINHERIT)

button = Button().New();
button.Handle("CLICK");						// Button::Handle(CLICK))
WindowManager().Display();					// Button::Display()

auditory = AuditoryButton().New(button);	// Dynamic mixin.
button.Handle("CLICK");						// AuditoryButton::Handle(CLICK))
WindowManager().Display();					// AuditoryButton::Display()

uninherit(auditory, button);				// Mixin removed.
button.Handle("CLICK");						// Button::Handle(CLICK))
WindowManager().Display();					// Button::Display()

animated = AnimatedButton().New(button);	// Dynamic mixin.
button.Handle("CLICK");						// AnimatedButton::Handle(CLICK))
WindowManager().Display();					// AnimatedButton::Display()

uninherit(animated, button);				// Mixin removed.
button.Handle("CLICK");						// Button::Handle(CLICK))
WindowManager().Display();					// Button::Display()

////////////////////////////////////////////////////////
// Dynamic mixin composition.
//
print("Dynamic mixin composition:", nl);

function mixin_comp {	// The composer as a generic function.
	local n = arguments.total();
	comp = [ 
		@B 	: arguments[0], 		// The B class object
		@n 	: n - 1,				// Number of base class objects
		
		method new {				// Object constructor for the composite class object
			args = [];
			argNo = 0;
			if (arguments[0] == @B.class)
	    		{ args = arguments[1]; argNo = 2; }
			B_obj = @B.new(|args|);			// Construct a B object
			B_obj.n	= @n;					// Store the number of total base objects
			for (local i=1; i <= @n; ++i) {	// Instatiate all base objects
				args = [];
				Ai	= self["A" + i];		// Get the Ai class object
				if (arguments[argNo] == Ai.class) {
					args = arguments[++argNo]; 
  					++argNo;  
 	    		}
	     		Ai_obj = Ai.new(|args|); 	// Construct an Ai object
				inherit(B_obj, Ai_obj); 	// Establish inheritance A <- Bi 
				B_obj[Ai.class] = Ai_obj;	// Store the produced base object.
			}
			return B_obj;					// Return the B (most-derived) object
		}
	];
	 
	// Store base class objects as keys  Ai:Bi
	for (local i = 1; i <= n - 1; ++i)
		comp["A" + i] = arguments[i]; 
	
	return comp;
}

//*************************
// Sample base class object.
//
function Creature(){
	if (isundefined(static proto))
		proto = [
			{"class" : "Creature"},
			{"ai" : (method(){ print(@name, " is thinking", nl); })},
			{"new" : (method(name) {
						local a	= tabcopy(self);
						a.name	= name;
						return a;
					})}
		];
	return proto;
}

//*************************
// Sample mixin class object (derived).
//
function Attacker(){
	if (isundefined(static proto))
		proto = [
			{"class" : "Attacker"},
			{"force" : "mildly" },
			{"ai" : (method(){ @Creature..ai(); print("Attacking ", @force, nl); })},
			{"new" : (method(force) {
						local a	= tabcopy(self);
						if (arguments[0])
							a.force	= force;
						return a;
					})}
		];
	return proto;
}

comp = mixin_comp(Attacker(), Creature());
print("COMP CLASS OBJECT:", nl, comp, nl);

a = comp.new("Attacker", ["hard"], "Creature", ["Dragon"]);
print(nl, "ai():", nl);
a.ai();

print(nl, "COMP PRODUCED OBJECT:\n", a, nl);

////////////////////////////////////////////////////////
// HIGHER-ORDER POLYMORPHIC FUNCTIONS, DYNAMIC FUNCTION OVERLOADING
// AND VIRTUAL CONSTRUCTORS.
//
print("****Function composition 'F o G' *****", nl);

// Returns 'F o G' composition function. If G returns multiple N values,
// the those should be collected in a table with successive natural indices
// 0...N-1.
//
function function_comp(F, G) {
	local t = [ 
		{.F    : F}, 
		{.G    : G},
		{.COMP : (method(){ 
					return @F( |@G(|arguments|) |); 
				})}
	];

   return t.COMP;
}

f_mul_2		= (function(x,y){ return x * y; });
f_sqr_2		= (function(x,y){ return [ sqr(x), sqr(y) ]; });
f_mul_sqr_2	= function_comp(f_mul_2, f_sqr_2);		// Multiplies the squares.
print("Expected 144: ", f_mul_sqr_2(3,4), nl);	// Should print 144.

//******************

print("****Deferred call 'D(F, a1,...,aN)'*****", nl);

function deferred_call(f) {

	assert arguments.front() == f;
	arguments.pop_front();
	
	 local t = [
		{ .F	: f }, 
		{ .ARGS : arguments },
		{ .D	: (method(){ @F(| @ARGS |); })}
	];
	return t.D;
}

d = deferred_call(print, "deferred hello", ", world!", nl);
d();	// Equivalent to 'print("deferred hello", ", world!", nl)'

d = deferred_call(print, function_comp(f_mul_2, f_sqr_2)(4, 5), nl);
d();	// Equivalent to 'print(function_comp(f_mul_2, f_sqr_2)(4, 5), nl)'

//******************

print("****Dynamic function overloading*****", nl);

function Sig(t) {	// Signature extraction from a table of actual arguments.
	for (local s = "", local n = t.total(), local i = 0; i < n; ++i)
		if ((typeof(t[i]) == "Table" or typeof(t[i]) == "Object") and t[i]..class != nil)
			s += t[i]..class;
		else
			s += typeof(t[i]);
	if (s == "") 
		s = "void";
	return s;
}

// Returns an overloadable function object. The scheme used for
// signature matching is based on precise matching of argument types.
// In case of objects more sophisticated schemes to emulate subsumption
// can be easily implemented.
//
function Overloadable(){
	return [
		{"()"	: (method() {	// The functor method with signature-based dispatching.
					local g = @Dispatch[ Sig(arguments) ];
					if (not g)
						g =  @Dispatch["..."];	// The default function in case of no matching.
					return g(|arguments|);
				})},
		{.set		: (method(sig, f){ @Dispatch[sig] = f; })},
		{.get		: (method(sig, f){ return @Dispatch[sig]; })},
		{.Dispatch	: [] }	// Dispatch table per signature.
	];
}

// This is called with arguments as a sequence of pairs <sig, func>.
//
function overloaded_function {
	local f = Overloadable();
	for (local i = 0; i < arguments.total(); i += 2)
		f.set(arguments[i], arguments[i + 1]);	// <sig, func>
	return f;
}

f = overloaded_function(
		"NumberNumber",				(function(x, y){ return x + y; }),
		"BoolBool",					(function(a, b){ return a or b; }),
		"ProgramFuncProgramFunc",	function_comp
	);
	
print(
	"Expected 3, true, 4: ",
	f(1,2),								// 3
	space, f(true, false),				// true
	space, f(f_mul_2, f_sqr_2)(1,2),	// 4
	nl
);

////////////////////////////////////////////////////////
// Showing how . respects inheritance and can be used to
// produce class objects setting / getting attributes via library
// functions.
//

t1=[ {.x: -1}, {.y: -2} ];

t2=[
	{ "." : (method(t, i)		{ return tabget(self, i); })},
	{".=" : (method(t, i, c)	{ tabset(self, i, c); })}
];

t2.z = "hello";
t3 = [ {.w : "world" } ];

inherit(t2, t1);
inherit(t3, t2);
print(
	"Expected -1, -2, hello, world:", 
	t3.x, space, t3.y, space, t3.z, space, t3.w, nl
);

print("******Testing to string overloading**********", nl);

function tabletostring(t) {
	local s = "";
	for (	local ti = tabindices(t),
			local i = 0;
			i < tablength(ti);
			++i	) {
			s += "<" + tostring(ti[i]) + ":";
			s += tostring(tabget(t, ti[i])) + ">";
	}
	return s;	
}

uninherit(t3, t2);
print("t2 before overloading tostring:", nl, t2, nl);
tabset(t2, "tostring()", tabletostring);
print("t2 after overloading tostring:", nl, t2, nl);

////////////////////////////////////////////////////////

print("******Dump std API**********", nl);

function dump(api, prefix) {
	if (isundefined(static m))
		m = 0;
	for (	local t = tabindices(api), local i = 0, local n = tablength(t);
			i < n; 
			++i	) {
			
			local x = api[ t[i] ];
			local s = prefix + "." + t[i];
			if (typeof(x) == "Table" or typeof(x) == "Object")
				dump(x, s);
			else
			if (typeof(x) == "LibraryFunc")
				{ ++m; print(s, nl); }
			else
				print(s, ":", typeof(x), nl);
	}
	return m;
}
print("Total std functions:", dump(std_getapi(), "std"), nl);

////////////////////////////////////////////////////////
// GENERIC PROGRAMMING.

/*
Requirements: 
Iterator<I>, 
Addable<value_type>, 
Assignable<value_type>,
CopyConstructible<value_type>

template<typename I>
typename iterator_traits<T>::value_type 
sum(I start, I end, typename iterator_traits<T>::value_type init) {
  for (I current = start; current != end; ++current))
    init = init + *current;
  return init;
}
*/

// In Delta, as in most untyped dynamic languages,
// we do not need the associated types. But we can't have retroactive 
// modelling with traits (templates) - neither we can in Java or C# - so
// to retrofit the original model to the generic algorithm we may only
// pass extra parameters for the type decorators (next / get). 
//
function sum(start, end, init, next, get) {
	for (local current = start; current != end; current = next(current))
		init = init + get(current);
	return init;
}

t = IterationContainerAdapter( [ 1, 2, 3, 4, 5, 6, 7] );
print(
	sum(
		t.begin(), 
		t.end(), 
		0, 
		(function(i){ i.fwd(); return i; }), 
		(function(i){ return i.getval().value; })
	),
	nl
);

 t = [];
 if (not t[0] and typeof(t[0]) == "Nil")
	print("Not found (correct)!\n");
else
	assert false;
print(|list_new(1, 3, 5, 7, 9, 10, nil, 20, nl)|);