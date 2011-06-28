// Delta standard run-time library test.
//

// Testing string library functions.
//
s="Hello,World,Anthony,In,Day,2000,Delta,Use";
print("S=",s," Len=", strlen(s),"\n");
print(strslice(s, 0,4, 12,18, 20,21, 32,36, 38,0));
pos=strsub(s,"Anthony");
if (pos!=-1)
	print(strslice(s,pos,pos+strlen("Anthony")-1)+"\n");
pos=strsub(s, "Not in");
print(pos,"\n");
print(strupper("this will go to uppercase...\n"));
print(strlower("AND THIS TO LOWERCASE.\n"));
print(strbyte("Savidis",3),"\n");
print(strbyte("Savidis",3.14),"\n");
print(strbyte("Savidis",-4),"\n");
print(strrep("[]",5));

// Testing mathematic functions.
//
print(pi()," ", sqr(15), " ", sqrt(169), " ", sin(pi()/6), " ", cos(pi()/3), "\n");
print(abs(-12.4355), " ", max(1,2,3,4,5,56,7,8,-89), " ", min(0.00000123, 45, 212, -76.7756),"\n");
randomise();
for (i=0; i<10; i++)
	print(random(10),(i==9 ? "\n" : " "));
print(power(2,2), " ", power(3,3),  " ", power(2,16), "\n");

function printv (x) { print(typeof(x)," ", x, "\n"); }

// Testing file I/O.
//
fp=fileopen("test.txt","wt");
filewrite(fp,"Hello,world\n", 15, " ", true);
fileclose(fp);

fp=fileopen("test.txt","rt");
printv(fileread(fp,"string"));
printv(fileread(fp,"number"));
printv(fileread(fp,"bool"));
fileclose(fp);

fp=fileopen("test.txt","rt");
while (not fileend(fp))
	print(fileread(fp,"string"));
fileclose(fp);

fp=fileopen("test.bin", "wb");
fileput(fp,"Hello",15,true,false,byte(4),byte(123));
fileclose(fp);
fp=fileopen("test.bin", "rb");
printv(fileget(fp,"string"));
printv(fileget(fp,"real"));
printv(fileget(fp,"bool"));
printv(fileget(fp,"bool"));
printv(fileget(fp,"byte"));
printv(fileget(fp,"short"));
fileclose(fp);

// Testing shared memory lib.
//
shwrite("x",10);
shwrite("y", []);
print(shread("x"),shread("y"),shexists("z"));
SHTAB=shtable();
SHTAB.x=shread("y");
SHTAB.y=shread("x");
print(shdelete("x"), shdelete("y"), shdelete("z"));
print(SHTAB);

// Testing bit operators.
//
mask=0x10;	// 16 decimal, 00010000 binary.
x=0x1;
print("\n", bitand(mask,x), " ", bitor(mask,x), " ", bitxor(mask,x), " ", byte(bitnot(x)));
print("\n", bitand(bitshleft(x,4), mask));


// Testing resource loading.
//
rc=rcload("resources.txt");
print("\nResources: ", rc);