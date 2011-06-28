vmrun(vm = vmload("vmfuncs_funcs.dbc", "functions"));
funcs = vmfuncs(vm);

for (i = 0, t = tabindices(funcs); i < tablength(t); ++i)
	funcs[ t[i] ]();
