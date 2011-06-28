// Use of subobject trees.
// AS, 2009.

using std;

const nl = "\n";

function New(id) 
		{ return [ { #id: id } ]; }
		
inherit(b1 = New(#b1), a2 = New(#a2));
inherit(b1, a1 = New(#a1));
inherit(b2 = New(#b2), a3 = New(#a3)); 
inherit(c = New(#c), b2);
inherit(c, b1);

print(c, "\n");
b1 = b2 = a1 = a2 = c = nil;	// Collect here

function Point(x, y) {
      return [
            @x { 											// an 'x' attribute definition 
                  @set (method(v) { @x = v; @xf(self); }) 	// defining the 'x' set method, which invokes a callback too 
                  @get (method { return @x; }) 				// defining the 'x' get method 
            }, 
            @y { 											// a 'y' attribute definition 
                  @set (method(v) { @y = v; @yf(self); }) 	// defining the 'x' set method, which invokes a callback too 
                  @get (method { return @y; }) 				// defining the 'x' get method 
            }, 
            { #xf, #yf : (function(pt){}) }, 				// default empty implementations of both listener callbacks 
            { #x, #y : x, y }, 								// initial (optional, but desirable) values to the 'x' and 'y' slots 
            method set_x_listener(f){ @xf = f; }, 			// method to set the 'x' property listener callback 
            method set_y_listener(f){ @yf = f; } 			// method to set the 'y' property listener callback 
      ]; 
}

pt = Point(10,20);
pt.x = 20;	 // internally invokes the set method 
pt.set_y_listener((function(pt){ print(pt.y); })); // set a listere on 'y' to print its value 
pt.y = -10; 	// will set 'y' to '-10' and print '-10' 
tabredefineattribute(pt, #x, (function{}), (function(v) { return 0; }));
pt.x = 20;
print("\n", pt.x, "********\n");	// prints 0

A = [ @A : #A, method M { return self..A; } ];
B = [ @B : #B, method M { return self..B; } ];
C = [ @C : #C, method M { return getbase(getbase(self, 0), 0)..M(); } ];

inherit(B, A);
inherit(C, B);
print(C.M(), "\n");	// prints A

// Inheritance works correctly also among externids and objects.
inherit(x = [], l = list_new());	// x inherits from 'l' list
x.l_base = l; // keeps its base in a local slot
x..push_back = tabmethodonme(x, method(x){ self.l_base..push_back(x, x); }); // push_back twice
x.push_back(1);
print(x.l_base, nl);	// should print the list as [1,1]
x.pop_back();			// removes one elements
print(x.l_base, nl);	// should print the list as [1]

(method show { print(@push_back, nl); }).self = externiduserdata(l);
show();					// should print the push_back library method

// Here we have a combination of inheritance and delegation.
// Assuming ->(delegates) and => (donates)
// We have the schema x -> y => z.

x = [ @x : 10 ];
y = [ @y : 20 ];
z = [ @z : 30 ];

std::delegate(x, y);
std::inherit(z, y);
print(x.z, nl);			// 30
x.z = 40;
assert z..z == x.z;
print(x.z, nl);			// 40

// When an object with both delegates and derivaties is met,
// the lookup to derivatives preceeds the delegation lookup.

zz = [ @z : 50 ];
std::inherit(zz, x);	// x => zz added.
assert zz..z == x.z;
print(x.z, nl);			// 50

x..z = 60;				// Even when we have the slot in x
assert x.z == zz..z;	// The derivative slot is more recent to its base slot
print(x.z, nl);			// Thus 50 again		