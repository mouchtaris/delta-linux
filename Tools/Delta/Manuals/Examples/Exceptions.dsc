// Exception handling, reflection (compiling code at runtime), 
// cross-vm invocations and debugging of dynamic code,
// and trapping runtime errors.
// AS, 2012.

using std;

const TEXT = "
using std;
function g { local x=0;\n x();\n throw \"hello\\n\"; } 
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
// Example showing how to track-down runtime errors as exceptions
// by emulating mutliple trap blocks on tagged errors with a dispatcher
// inside the trap block.

try { 
	local f;
	f();
}
trap e {
	local h = [
		{	std::DELTA_ARITH_OPERATOR_ERROR,
			std::DELTA_RELAT_OPERATOR_ERROR
			: function { print("<Arithmetic / Relational>", e, nl); }
		},
		{	std::DELTA_NOT_CALLABLE_ERROR
			: function { print("<Call>", e, nl); }
		}
	][std::getvaluetag(e)];

	if (not h)
		throw(e);	// rethrow or could be default logic
	else
		h();
}

//////////////////////////////////////////////////////
// Example showing how to distinguish if a trapped exception is 
// a std runtime error or a normal excpetion.

try
	(function(b) {
		if (b)
			std::error("User [foo] runtime error");
		else
			throw "Client [bar] exception";
	})(true);
	
trap e
	print(
		(std::iserrortag(std::getvaluetag(e)) ? "<Error>" : "<Exception>"),
		"(", e, ")", 
		nl
	);

//////////////////////////////////////////////////////
// trapping stack overflow on functions or methods

try
	(lambda { @lambda(1,2,3,4,5,6,7) })();
trap e
	print(e, nl);

try
	[ method f { @f(11,22,33,44,55,66,77); } ].f();
trap e
	print(e, nl);

//////////////////////////////////////////////////////
