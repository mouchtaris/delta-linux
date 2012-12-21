//////////////////////////////////////////////////////////////////////////////////
// Assembling dependencies, utility functions
// and execution code for various components.
//
&function LoadComponentSpecs {
	function ComponentSpecs(deps, funcs, code) {
		local specs = [];
		if (deps)
			specs.deps = deps;
		if (funcs)
			specs.funcs = funcs;
		if (code)
			specs.code = code;
		return specs;
		//return [
		//	@deps : deps,
		//	@funcs : funcs,
		//	@code : code
		//];
	}
	//Suppose specs are read from file
	return [
		ComponentSpecs(<<using std;>>, nil, <<print("printer\n");>>),
		ComponentSpecs(nil, nil, <<print("xml\n");>>),
		ComponentSpecs(
			nil,
			<<function bind1st(f, arg) { return lambda (...) { f(arg, |arguments|) }; }>>,
			<<bind1st(print, "binder\n")();>>
		)		
	];
}
&deps = nil;
&funcs = nil;
&code = nil;
&foreach(spec, LoadComponentSpecs()) {
	deps = <<~deps;~(spec.deps);>>;
	funcs = <<~funcs;~(spec.funcs);>>;
	code = <<~code;~(spec.code);>>;
}
!(deps);
!(funcs);
!(code);

//////////////////////////////////////////////////////////////////////////////////
// Assembling initialization and cleanup code based on used components.
//
&function GenerateComponent(c) {
	return [
		@initialization : <<print("Initialization of component ", ~c, "\n");>>,
		@cleanup : <<print("Cleanup of component ", ~c, "\n");>>
	];
}

&function AssembleComponents(components) {
	local initialization = nil;
	local cleanup = nil;
	foreach(local comp, components) {
		initialization = <<~initialization;~(comp.initialization);>>;
		cleanup = <<~cleanup;~(comp.cleanup);>>;
	}
	return [
		@initialization : initialization,
		@cleanup : cleanup
	];
}

&program = AssembleComponents(
	std::list_new(GenerateComponent("A"), GenerateComponent("B"), GenerateComponent("C"))
);
!(program.initialization);
print("Main application code\n");
!(program.cleanup);
