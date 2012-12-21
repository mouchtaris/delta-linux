&function ExceptionHandler() {
	if (std::isundefined(static singleton))
		singleton = [
			@dispatcher : [],
			method AddPolicy(id, func) { assert @dispatcher[id] == nil; @dispatcher[id] = func; },
			method New(policy...) { 
				local handler = @dispatcher[policy];
				assert handler;
				arguments.pop_front();
				return handler(...);				
			}
		];
	return singleton;
}

&const NONE					= 0;
&const INACTION				= 1;
&const LOG					= 2;
&const RETRY_LATER			= 3;
&const CLEANUP_RETHROW		= 4;
&const GUARDED_SUSPENSION	= 5;
&const RETHROW_ABSTRACT		= 6;

&function InstallExceptionPolicies {
	local ex = ExceptionHandler();
	ex.AddPolicy(NONE, function {
		return [
			method defs { return nil; },
			method @operator()(code) { return code; }
		];
	});
	ex.AddPolicy(INACTION, function {
		return [
			method defs { return nil; },
			method @operator()(code ){ return <<try { ~code; } trap $ex;>>; }
		];
	});
	ex.AddPolicy(LOG, function {
		return [
			method defs { return <<logger = (function{ std::print(..., "\n"); })>>; },
			method @operator()(code) { return <<try { ~code; } trap $ex logger($ex);>>; }
		];
	});
	ex.AddPolicy(RETRY_LATER, function (retryData) {
		assert 	std::typeof(retryData) == std::TYPEOF_OBJECT	and
				retryData.attempts								and
				retryData.delay;
		return [
			method defs {
				return <<
					sleeper = (function (delay) {
						local time = std::currenttime();
						while (time + delay > std::currenttime());
					});
				>>;
			},
			method @operator()(code) { return <<{
					for (local $i = 0; $i < ~(retryData.attempts); ++$i)
						try { ~code; break; } trap $ex sleeper(~(retryData.delay));
					if ($i == ~(retryData.attempts))
						{ ~(retryData.fail); }						
				}>>;
			}
		];
	});
	ex.AddPolicy(CLEANUP_RETHROW, function (cleanupCode) {
		return [
			method defs { return nil; },
			method @operator()(code) {
				return <<try { ~code; } trap $ex { ~cleanupCode; throw $ex; }>>;
			}
		];
	});
	ex.AddPolicy(GUARDED_SUSPENSION, function (condition) {
		return [
			method defs { return nil; },
			method @operator()(code) {
				return <<try { ~code; } trap $ex { while(not ~condition); ~code;}>>;
			}
		];
	});
	ex.AddPolicy(RETHROW_ABSTRACT, function (abstractExData) {
		return [
			method defs { return nil; },
			method @operator()(code) {
				return <<try { ~code; } trap $ex { throw [~abstractExData, @source : $ex]; }>>;
			}
		];
	});	
}

&InstallExceptionPolicies();
&ex = [];
&ex[NONE] = ExceptionHandler().New(NONE);
&ex[INACTION] = ExceptionHandler().New(INACTION);
&ex[LOG] = ExceptionHandler().New(LOG);
&ex[RETRY_LATER] = ExceptionHandler().New(RETRY_LATER, [@attempts : 5, @delay : 1000, @fail : <<std::print("retry failed\n")>>]);
&ex[CLEANUP_RETHROW] = ExceptionHandler().New(CLEANUP_RETHROW, <<std::print("cleanup code\n")>>);
&ex[GUARDED_SUSPENSION] = ExceptionHandler().New(GUARDED_SUSPENSION, <<called_three_times()>>);
&ex[RETHROW_ABSTRACT] = ExceptionHandler().New(RETHROW_ABSTRACT, <<@class : "AbstractException">>);

!(ex[0].defs());
!(ex[1].defs());
!(ex[2].defs());
!(ex[3].defs());
!(ex[4].defs());
!(ex[5].defs());
!(ex[6].defs());

function f(x) { std::print("f(" + x + ") being called\n"); throw "Exception"; }

function called_three_times() {
	if (std::isundefined(static counter))
		counter = 1;
	return ++counter > 3;
}

function g() {
	if (not called_three_times())
		throw "Exception";
	std::print("After the third call!\n");
}

!(ex[INACTION](<<f(1)>>));
!(ex[LOG](<<f(2)>>));
!(ex[RETRY_LATER](<<f(3)>>));
//!(ex[CLEANUP_RETHROW](<<f(4)>>));
//!(ex[NONE](<<f(0)>>));	//will throw so test last
!(ex[GUARDED_SUSPENSION](<<g()>>));
!(ex[RETHROW_ABSTRACT](<<f(6)>>));

&function DbyC (object, m) {
	return <<
		if ((local $precondition = ~(object)["pre_" + ~m]) and not $precondition())
			throw ~m + "_precondition_exception";
	>>;
}

stack = [
		method empty() { return true; },
		method pop(){},
		method pre_pop(){ return not @empty(); }
];

//combining exception handling policies with other exception patterns (e.g. DbyC).
!(ex[LOG](DbyC(<<stack>>, "pop")));
