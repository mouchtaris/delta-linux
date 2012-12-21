/*using std;
const asm		= "NullClass.dbc";
const nl 		= "\n";

// Produce source on-the-fly.
code = "function foo { std::print(\"foo\\n\"); }
&foo();
";
vm = nil;

// Compile dynamic code to a byte code file.
result = vmcompstring(code, asm, (function(s){ print(s, nl); }), false);
assert result;

// Load and initialise (run) the dynamic byte code.
vmrun(vm = vmload(asm, "NullClass"));
vm.foo();*/

function f(x){ return x; }
!(f(<<std::print(f(1));>>));