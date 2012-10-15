using std;

n = (not shexists("Closures") ? 0 : shread("Closures") + 1);
shwrite("Closures", n);
l = vmload("Closures.dbc", "Closures" + n);
vmrun(l);

libs::registercopied(#delegation, 		"Delegation.dbc");
libs::registercopied(#overloading, 		"OperatorOverloading.dbc");
libs::registershared(#sh_overloading, 	"OperatorOverloading.dbc");

libs::registercopied(
	#dyn_foo,
	std::inputbuffer_new(					// Turn it to an tab input buffer
		std::vmcompstringtooutputbuffer(	// Compile to byte code buffer
				"function foo { throw \"foo\"; }", 
				(function(err){ print(err,"\n"); }), 			
				false
		)
	)
);  

d1 = libs::import(#delegation);
o1 = libs::import(#overloading);

d2 = libs::import(#delegation);
o2 = libs::import(#overloading);

os1 = libs::import(#sh_overloading);
os2 = libs::import(#sh_overloading);
assert os1 == os2;

d = libs::import(#dyn_foo);
try d.foo(); trap e { print(e, "\n"); }

libs::unimport(o1);
libs::unimport(os1);
libs::unimport(os2);
