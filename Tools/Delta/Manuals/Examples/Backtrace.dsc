// Example regarding the use of vmcurrcall() and vmnextcall()
// reflective functions to backtrace the current call.
// A. Savidis, July 2009.

using std;
function backtrace {
	for (local c = vmcurrcall(); c; c = vmnextcall(c.tag))
		print(c.name, "\n");
}

function f { 
	[ method @operator() { backtrace(); } ]();
}
 
(function a {
	(function b {
		(function c {
			(function d { callglobal("f"); })();
		})();
	})();
})();