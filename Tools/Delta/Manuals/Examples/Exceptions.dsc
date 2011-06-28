// Exception handling, reflection (compiling code at runtime), 
// cross-vm invocations and debugging of dynamic code.
// AS, 2009.

using std;

const TEXT = "
using std;
function g { throw \"hello\\n\"; } 
function f(b) { 
	if (b) 
		vmcall(vmid(vmthis())+\"v\", \"f\", --b); 
	else 
		vmcall(\"v\", \"g\"); 
}";
const nl   = "\n";
buffer = inputbuffer_new(vmcompstringtooutputbuffer(TEXT, (function(s){ print(s, nl); }), false));
reader = reader_frominputbuffer(buffer);

(v 		= vmloadfromreader(reader, "v")).run();

buffer.rewind();
(vv		= vmloadfromreader(reader, "vv")).run();

buffer.rewind();
(vvv	= vmloadfromreader(reader, "vvv")).run();

try 	{ vmcall(v, "f", 2); }	// Simply step-in (F11) to the end.
trap e 	{ print(e, nl); }

//////////////////////////////////////////////////////

t1 = [
	method @operator.(t,i) 
			{ throw "no dot(.) for you\n"; }
];

t2 = [
	method @operator+(t, other)
		{ throw "no plus(+) for you\n"; }
];

try 	t1(); 
trap e 	{ print(e); }

try 	t2 += 5; 
trap e 	{ print(e); }

//////////////////////////////////////////////////////
