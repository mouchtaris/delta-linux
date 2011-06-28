
// Test-1.
nl="\n";
x=5;
y="hello";
z=true;
w=nil;
x=y;
print(x,y,z,w,nl);


// Test-2.
y=10;
z=3.141528;
w=12345678910;
x=y+z+w;
print(x,y,z,w,nl);



// Test-3.
x=(y- z++ + --w)-10;
print(x,y,z,w,nl);



// Test 4.
x=16;
x-= x+= x/= x*= x%= 23;
print(x,y,z,w,nl);



// Test-5.
function add(x,y) { return x+y; }
add(x=10,y=x+20);
w=add(add(x, add(x,y+x)));
print(x,y,z,w,nl);



// Test-6.
x=(function swap(x,y) {tmp=x; x=y; y=tmp; });
x(y=10, z=23);
print(x,y,z,w,nl);



// Test-7.
t=[10,20,30,[],[{10:"Antonis"},{"x","y","z":"empty"},false]];
t[x]=10;
t.x = (function g(x) { return x(g); });
t[[[],[]]]=[];
print(t,nl);


// Test-8.
a=b or c;
a=b and c;
x=a and ( b or c) or (true and not not false);
print(x,a,nl);


// Test-9.
x=(a>=15 or b<=10) and (w*z-=10)==37;
print(x,y,z,w,nl);

// Test-10.
x=10;
if (x++ or (y and z>=a+b)) x=10; else x=20;

// Test-11.
while (a>b or c) {
	i++;
}


// Test-12.
for (i=0,j=[]; a[i]<=b and (c>=a[i+10]); i++, a[i]=[i]) {
	j=strlen("hello");
	function swap(x,y) { t=x; x=y; y=t; }
	swap(addressof(j), addressof(j));
}

//Test-14.
x=((done or y>=x+w) and (z>3.14) ? 23 : 39);

// Test-15.
x++ + --y;
y=-z;

// Test-16.
onevent LButtonDown (actor, x, y) {
	if (sqr(x,y)==sqrt(x+y, x-y))
		return true;
	else
		return false;
}

// Test-17.
z=10;
w=20;
z=not not not not f(x,y);

// Test-18.
z=10;
s="hello"+"world"+"bye"+"bye"+"darling"+"world";
w=20;
function myadd(x,y) { return x+y; }
(function (x,y) { tmp=x; x=y; y=tmp; })(addressof(z), addressof(w));
x = sqrt;
print(x,nl);