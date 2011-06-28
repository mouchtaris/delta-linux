function lib_g () { print("g()\n"); return 1; }
function lib_f () { print("f()\n"); return 2; }
function lib_h () { print("h()\n"); return (function(){ print("un-named\n"); return 3; }); }

function f() { s; { local s; } }

x;

{ local y; local z; { local y; local z; } }

y;
z;

