// Testing of static function variables, having
// local scope but global storage.
// Test of tables used as objects with members
// functions.
// Test of const qualifier.
//
function rememberarg (arg) {
	static value = arg;
	return ( function(arg) { return value * arg;} );
}

r = rememberarg(23);
print(r(10),"\n");
print(r(2),"\n");

// Object constructor function.
//
function Point (x,y, name) {

	// Sub-object constructor.
	//
	function Attributes (parent) {
		return	[ 
			{"parent" : parent }, // Explicitly supplied parent object.
			{"color" : 0 },
			{"hidden" : false },
			{"setcolor" : ( member(color) { self.color = color; } ) },
			{"hide" : (member() { self.hidden = true; } ) },
			{"show" : (member() { self.hidden = false; } ) },
			{"tostring" : (member() { return "Attributes(" + self.color + "," + self.hidden + ")"; } ) }
		];
	}

	// Object definition.
	//
	return	[
		{ "x" : x },
		{ "y" : y },
		{ "name" : name },
		{ "move" : ( member (dx, dy) { self.x += dx; self.y += dy; } ) },
		{ "tostring" : (member() { return "Point(" + self.name + "," + self.x + "," + self.y + ")"; } ) },
		{ "display" : (member() { print(self.tostring(), ",", self.attributes.tostring(), "\n"); } ) },

		{ "get" : ( member get(id){ return self[id]; } ) },
		{ "getx" : ( member(){ return self.get("x"); } ) },
		{ "gety" : ( member(){ return self.get("y"); } ) },

		// This shows how a member can be called after it is
		// defined, inside the same table element (scopewise, outside
		// this table element it is invisible).
		//
		{ "i", "j" : (member constval() { return 1969; } )(), constval() },

		// This shows how we can have objects inside objects. Notice that
		// there is no built-in way for a sub-object to know its parent
		// unless you put explicitly such an attribute. To do that, we make an explicit
		// sub-object constructor in which we pass the parent table.
		//
		{ "attributes" : (member(){ return Attributes(self); })() }
	];
}

const P1_X = 10;
const P1_Y = 20;
const P2_X = P1_X - 1;
const P2_Y = P1_Y + 12;
const P_DX = 3;
const P_DY = P_DX;
const Relation = (P1_X > P1_Y) or (not  (P2_X > P2_Y));

// Warnings for constant conditions known at compile
// time.
//
if (not Relation)
	print("True relation\n");

while (not not not not (true or false)) {
	print("Hello\n");
	break;
}

if (not not Relation or Relation)
	print("Yes\n");
else
	print("No\n");

// Object instantiation.
//
p1 = Point(P1_X, P1_Y, "point1");
p2 = Point(P2_X, P2_Y, "point2");
p1.attributes.hide();
p2.attributes.setcolor(32);

// Member call.
//
p1.move(P_DX,P_DY);
p1.display();

p2.move(1,-1);
p2.display();

// Member functions as first order objects carrying instance
// information too.
//
p1_display = p1.display;
p1_display();

// When setting the member of an object X with the member of
// another Y, it is the Y member that is called!
//
p1.display = p2.display;
p1.display();	// Displays for p2.

// More tests of member function calls.
//
print(p1.getx(),",", p1.gety(), ",", p1.i, ",", p1.j, "\n");

t1 = [10,20];
t1[2] = t1;
t2 =[10,t1];

// Static data initialisation control.
//
function TestStatic () {

	static msg;
	static call;

	if (uninitialised(msg)) {
		msg = ", Hello, everybody!\n";
		call = 1;
	}
	else
		++call;

	function message() {
		print(call, msg);
	}

	return message;
}

// Debug qualified code and assertions.
//
debug total = tablength(t1);
debug function DebugOnlyFunc() { return 1; }
assert not ++total;
assert DebugOnlyFunc()==1;

TestStatic()();
TestStatic()();
TestStatic()();

l = list(1,2,3,4,5,"hello");
list_push_back(l, l);
print(l,"\n");

print("Testing vm calls:\n");
vmcall(this(), "TestStatic")();
vmcall(this(), vmfuncaddr(this(), "TestStatic"))();

v = vector(5);
vector_setval(v, 0, v);

print(
	"Types:\n", 
	externidtype(l), "\n",
	externidtype(this()), "\n",
	externidtype(v), "\n",
	externidtype(10), "\n",
	v, "\n"
);
