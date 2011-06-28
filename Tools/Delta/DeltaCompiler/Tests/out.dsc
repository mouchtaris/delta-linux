# 1 "testin.dsc"
# 1 "commondefs.dsc" 1









# 1 "testin.dsc" 2



--
 
 
x=17;
printcmmd=print;
function f(x) {
	printcmmd("f(x):", ++x,"\n");
	function g(x) {
		printcmmd("g(x):", ++x,"\n");
		function h(x) {
			printcmmd("h(x):", ++x,"\n");
			return x;
		}
		return h(x);
	}
	printcmmd(g(x),"\n");	
	return ( function(x) { printcmmd("anonymous(x):", x, "\n"); } );
}
f(x)(23);

 
 
flag=true;
if (flag) 
	print("True.\n");
else
	print("False.\n");

 
 
function println() { 
	for (i=0; i<args(); i++)
		print(args(i), " ");
	print("\n");
}

 
 
println("-----------");
x++;
println(x);
x=(y=20)+x;
println(x);
println(flag * y + not flag * 45.56);

 
 
println("-----------");
z=((((x=10.5)+(y=20.5))*31)/31-1)*true;	 
println(z);
z-=((10*true) % 7);	 
println(z);

 
 
println("-----------");
println(""+z+true+nil+w);	 

 
 
println("-----------");
for (i=0, j="Hello"; i<5; i++,j++) {
	print(i,"-",j,"\n");
	for (q=i; q and q<10000000; q*=100)
		print((q%500 ? ">>>>>" : "<<<<<"));
}

 
 

function factorial(n) { 
	return (n==1 ? 1 : n*factorial(n-1)); 
}
println(factorial(5));	 


 
 
function G() {
	println(args());
	for (i=0; i<args(); i++)
		print("Arg(",i,")=", args(i),"\n");
}

G();	 
G("Hello");  

 
 
 
G(1,2,3,"AS",nil,false,"is","Here");

 
 
t=[10,20,30,"Hello",[],{"x","y":315},{println:println}];
t.pt_x=[15,20];
t.pt_y=[45,40];
println(t);

function tostring(x) { return ""+x; }

T=[];
for (i=0; i<15; i++)
	T[tostring(i)]=[{i:tostring}];

callglobal("println", callglobal("tostring", T));

 
 
print("ARITHMETIC TEST--------\n");
println((x=true) + (s=""));
println(x+nil, x*nil, x(y=[]), y==y, x+10, 10/false, x+(z=print), println);

 
 
function swap(x,y) { 
	t=valueof(x); 
	setvalue(x, valueof(y)); 
	setvalue(y, t);
}
swap(addressof(x), addressof(y));  
println("x",x,"y",y);

 
 
for (table=[], i=0; i<50; i++) {
	table[tostring(i)]=short(i);
	table[i]=byte(i);
}
print("Table done. Prepare for printing...\n");
println(table);

println(typeof(t), typeof(table), typeof(typeof), typeof(x), typeof(println), typeof(nil), typeof(xxx));

 
 
 
for (indices=tabindices(table), len=tablength(indices), i=int(0); i<len; i++)
	print("T[", i, "]=", table[indices[i]], "\n");

 
 
print( ((x=10) <= (y=20) ? (x+y >=15 ? x*y : x-y) : (x*y > 15 ? x-y : x*y)) );
