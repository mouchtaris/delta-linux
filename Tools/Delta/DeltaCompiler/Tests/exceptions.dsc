// Exception handling, reflection (compiling code at runtime), 
// cross-vm invocations and debugging of dynamic code.
// AS, 2009.

const TEXT = "
function g { throw \"hello\\n\"; } 
function f(b) { 
	if (b) 
		vmcall(vmid(vmthis())+\"v\", \"f\", --b); 
	else 
		vmcall(\"v\", \"g\"); 
}";
const nl   = "\n";

buffer = inputbuffer(vmcompstringtooutputbuffer(TEXT, (function(s){ print(s, nl); }), false));
reader = reader_frominputbuffer(buffer);

(v 		= vmloadfromreader(reader, "v")).run();

buffer.rewind();
(vv		= vmloadfromreader(reader, "vv")).run();

buffer.rewind();
(vvv	= vmloadfromreader(reader, "vvv")).run();

try 	{ vmcall(v, "f", 2); }	// Simply step-in (F11) to the end.
trap e 	{ print(e, nl); }

