// An example showing how to use reflection in Delta (compiler invocation at runtime).
// Code may be compiled to a buffer and load from it (completely transient, no secondary
// storage).
// AS, 2009.

using std;
const asm		= "NullClass.dbc";
const nl 		= "\n";

print(nl, vmextractbuilddeps("Closures.dsc"), nl);

// Produce source on-the-fly.
code = "function NullClass{\nreturn [ {.class : \"Null\"} , {.x : 10 } ]; \n}";
vm = nil;

function test1 {
	// Compile dynamic code to a byte code file.
	result = vmcompstring(code, asm, (function(s){ print(s, nl); }), false);
	assert result;
	
	// Load and initialise (run) the dynamic byte code.
	vmrun(vm = vmload(asm, "NullClass"));
}

function test2 {
	reader =	std::reader_frominputbuffer(						// Make binary reader
					std::inputbuffer_new(							// Turn it to an input buffer
						output = std::vmcompstringtooutputbuffer(	// Compile to byte code buffer
									code, 
									(function(s){ print(s, nl); }), 
									false
								 )
					)
				);


	vm = std::vmloadfromreader(reader, "NullClass");	// Load from reader.
	std::vmrun(vm);										// Run it.
	vm.NullClass();										// Can access global funcs directly.
	local f = vm.NullClass;
}

function test3 {
	
	// Compile dynamic code to outputbuffer, turn it to input buffer
	// load vm from it.
	vmrun(
		vm = vmloadfrominputbuffer(
				inputbuffer_new(
					vmcompstringtooutputbuffer(code, (function(s){ print(s, nl); }), false)
				), 
				"NullClass"
			)
	);
}

test2();		// Comment out any; they all have the same effect.
//	test2();
//	test3();

// Extract global methods (as object) through introspection.
funcs = vmfuncs(vm);

// Invoke a method (class constructor here).
vmcall(vm, "NullClass");
a = funcs.NullClass();
print(a, nl); // Test purposes only.

// Add at shared space the class ctor so that other VMs can use it.
shwrite("NullClass", funcs.NullClass); 

// Test deployment of dynamic class ctor via shared space.
print(shread("NullClass")(), nl);


