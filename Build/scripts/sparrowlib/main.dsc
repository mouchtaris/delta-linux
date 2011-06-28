using std;

///--- preloading of supplied libraries
util 		= nil;
sparrowlib 	= nil;
sparrowDir 	= spw::installationdir() + "/";

function libSparrow {
	if (sparrowlib)
		vmunload(sparrowlib);
	sparrowlib = vmload(sparrowDir + "scripts/bin/sparrowlib.dbc", "sparrowlib");
	assert sparrowlib;
	vmrun(sparrowlib);
}

function Sparrow {
	if (not (local spw = vmcall(sparrowlib, "sparrow"))) {
		libSparrow();
		spw = vmcall(sparrowlib, "sparrow");
	}
	return spw;
}

function Utility { return ::util; }

libSparrow();

util = [ { .bind : vmfuncaddr(sparrowlib, "bind") }];

