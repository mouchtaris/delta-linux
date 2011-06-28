// An example showing how to use delegation in Delta (prototype-based programming).
// AS, 2009.

using std;
const nl = "\n";
function Point {
	if (isundefined(static proto))
		proto = [
			@class  : #Point,
			@Point : @self,
			method	@				{ return self.class + "(" + self.x + "," + self.y + ")"; },
			method	move(dx,dy)		{ self.x += dx; self.y += dy; },
			method	@operator+(a,b)	{ return Point().new(a.x + b.x, a.y + b.y); },
			method	@operator+_(a,b){ return a + self.x + self.y; },
			method	@operator()		{ print(self, nl); }
		];
	if (isundefined(static factory))
		factory = [
			{ #x, #y	: 0 },
			@prototype	: proto,
			method new(x,y) {
				local pt = tabcopy(self);
				pt.x = x; pt.y = y;
				delegate(pt, proto);
				return pt;
			}
		];
	return factory;
}

p1 = Point().new(1, 2);
p2 = Point().new(3, 4);
p3 = Point().new(5, 6);
p1.move(10,10);
p2.move(10,10);
(m = p3.move)(10,10);
(p2+p3)();
print(p1, nl, p2, nl, p3, nl);
print(1 + p1, nl);

// A different way of making simple prototypes.

Circle = [
	@prototype : [
			@pi : 3.141628,
			method area 		{ return self.pi * self.r * self.r; },
			method perimeter 	{ return 2*self.pi*self.r; }
		], 
	method @operator()(r, x, y) {
		delegate(
			local c = [ 
				@r : r, @x : x, @y : y,
				method destroy { undelegate(self, getdelegates(self)[0]); }
			],
			self.prototype
		);
		return c;
	},
	function GlobalInsideTable { print("GlobalInsideTable\n"); }
];
c1 = Circle(10, 0, 0);
c2 = Circle(20, 10, -10);
print(c1.area(), nl, c2.perimeter(), nl);

// We can inspect all Circle instances via its prototype.

for (t=[], i = 0; i < 20; ++i)
	t[i] = Circle(i+1, i, i);
print("Done!\n");	// Shift-F9 (quick watch) for 'c1' and inspect.
t[7].destroy();

// Lib func binder.

l = libfuncbind(print, "hello,", "world", nl);
l();
ll  = libfuncgetboundargs(l);
ll.pop_back();
ll.push_back(", and goodbye", nl);
ll.push = getdelegates(externiduserdata(ll))[0].push_back;
ll.push("****", nl);
l();

// Massive delegation tester.
(function {
	
	const N = 10000;
	const M = 500;
	
	print("massive delegation started...", nl);
	local t = [], local ts = tt = currenttime();
	
	for (local i = 0; i < N; delegate([], t), ++i)
		if (i and not (i % M)) {
			print(M, " objects created and delegated in ", currenttime() - ts, " msecs", nl);
			ts = currenttime();
		}
		
	print("massive delegation of ", N, " objects finished in ", (currenttime() - tt) / 1000, " secs", nl);
})();
