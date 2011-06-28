// Checking function calls.
//
x = 17;
myprint = print;

function f(x) {

	myprint("f(x):", ++x,"\n");

	function g(x) {

		myprint("g(x):", ++x,"\n");

		function h(x) {
			myprint("h(x):", ++x,"\n");
			return x;
		}

		return h(x);
	}

	myprint(g(x),"\n");	
	return ( function(x) { myprint("anonymous(x):", x, "\n"); } );
}

f(x)(23);

// Checking arithemetic.
//
myprint((x = (15+15)) * (y=(1.5)));


// Checking bools.
//
flag=true;
if (flag) 
	myprint("True.\n");
else
	myprint("False.\n");

// Prints arguments, separated with blanks, ending with new line.
//
function println() { 
	for (i=0; i<args(); i++)
		myprint(args(i), " ");
	print("\n");
}

// Checking arithmetic.
//
println("-----------");
x++;
println(x);
x=(y=20)+x;
println(x);
println(flag * y + not flag * 45.56);

// Checking arithmetic: more.
//
println("-----------");
z=((((x=10.5)+(y=20.5))*31)/31-1)*true;	// z is 30 
println(z);
z-=((10*true) % 7);	// z is now 27
println(z);

// Checking string conversion.
//
println("-----------");
println(""+z+true+nil+w);	// 27.0000trueNilUndefined

// Checking some statements.
//
println("-----------");
for (i=0, j="Hello"; i<5; i++,j++) {
	print(i,"-",j,"\n");
	for (q=i; q and q<10000000; q*=100)
		print((q%500 ? ">>>>>" : "<<<<<"));
}

// Checking some recursive functions.
//

function factorial(n) { 
	return (n==1 ? 1 : n*factorial(n-1)); 
}
println(factorial(5));	// 1*2*3*4*5=120


// Testing variable number of arguments.
//
function G() {
	println(args());
	for (i=0; i<args(); i++)
		print("Arg(",i,")=", args(i),"\n");
}

G();	// 0
G("Hello"); // 1 Arg(0)=Hello

// 8 Arg(0)=1 Arg(1)=2 Arg(2)=3 Arg(3)=AS 
// Arg(4)=Nil Arg(5)=false Arg(6)=is Arg(7)=Here
//
G(1,2,3,"AS",nil,false,"is","Here");

// Testing tables.
//
t=[10,20,30,"Hello",[],{"x","y":315},{println:println}];
t.pt_x=[15,20];
t.pt_y=[45,40];
println(t);

function tostring(x) { return ""+x; }

T=[];
for (i=0; i<15; i++)
	T[tostring(i)]=[{i:tostring}];

callglobal("println", callglobal("tostring", T));

// Testing arithmetic operators.
//
print("ARITHMETIC TEST--------\n");
println((x=true) + (s=""));
println(x+nil, x*nil, y==y, x+10, 10/false, x+(z=print), println);

// A generic swap.
//
function swap(x,y) { 
	t=valueof(x); 
	setvalue(x, valueof(y)); 
	setvalue(y, t);
}
swap(addressof(x), addressof(y)); // does the job
println("x",x,"y",y);

// Testing tolerance in tables.
//
for (table=[], i=0; i<1000; i++) {
	table[tostring(i)]=short(i);
	table[i]=byte(i);
}
print("Table done. Prepare for printing...\n");
println(table);

println(typeof(t), typeof(table), typeof(typeof), typeof(x), typeof(println), typeof(nil), typeof(xxx));

// Getting indices and then printing one by one each element
// of the original table (i.e. table iteration demo).
//
for (indices=tabindices(table), len=tablength(indices), i=int(0); i<len; i++)
	print("T[", i, "]=", table[indices[i]], "\n");

// Testing ternary expressions.
//
print( ((x=10) <= (y=20) ? (x+y >=15 ? x*y : x-y) : (x*y > 15 ? x-y : x*y)) );
