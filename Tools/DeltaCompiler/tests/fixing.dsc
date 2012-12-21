&function ImmediateFixing(actions, handlers) {
	assert actions.total() == std::tablength(handlers);
	local code = nil;
	local i = 0;
	foreach(local action, actions) {
		code = <<
			~code;
			try { ~action; }
			trap $ex {
				assert $ex == ~(handlers[i].class);
				~(handlers[i].handler);
			}
		>>;
		++i;
	}
	return <<{ ~code; }>>;
}

&function DefferedFixing(actions, handlers) {
	assert actions.total() == std::tablength(handlers);
	local errors = <<$errors>>;
	local code = nil;
	local i = 0;
	foreach(local action, actions) {
		code = <<
			~code;
			try { ~action; }
			trap $ex {
				assert $ex == ~(handlers[i].class);
				~errors[~i] = true;
			}
		>>;
		++i;
	}
	for(local j = 0; j < i; ++j)
		code = <<~code; if (~errors[~j]) { ~(handlers[j].handler); }>>;
	return <<{ local ~errors = []; ~code; }>>;
}

function f() { std::print("f\n"); throw "FException"; }
function g() { std::print("g\n"); throw "GException"; }

function f_fix() { std::print("f_fix\n"); }
function g_fix() { std::print("g_fix\n"); }

std::print("Immediate Fixing:\n");
&fixer = ImmediateFixing;
!(fixer(
	std::list_new(<<f()>>, <<g()>>),
	[
		[@class : "FException", @handler: <<f_fix()>>],
		[@class : "GException", @handler: <<g_fix()>>]
	]
));

std::print("Deffered Fixing:\n");
&fixer = DefferedFixing;
!(fixer(
	std::list_new(<<f()>>, <<g()>>),
	[
		[@class : "FException", @handler: <<f_fix()>>],
		[@class : "GException", @handler: <<g_fix()>>]
	]
));

