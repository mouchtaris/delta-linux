#include <stdio.h>
#include "CompilerAPI.h"
#include "BuildDependencies.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaCompilerInit.h"
#include "DDebug.h"
#include "Optimizer.h"
#include "usystem.h"
#include "uinit.h"

#define	FILENAME(ext) \
			(std::string(argv[1])+ext).c_str()

#define	SRC			FILENAME(".dsc")
#define	BIN			FILENAME(".dbc")
#define	TXT			FILENAME(".txt")
#define	ICODE		FILENAME(".idc")
#define	UNP			FILENAME(".unp")

static void onerror (const char* error) {
	fprintf(stderr, "%s", error);
	fflush(stderr);
}

int main (int argc, char* argv[]) {

	if (argc != 2)
		return 0;
	dinit(onerror);
	UtilPackage::Initialise();
	DeltaCompilerInit::Initialise();

	DeltaCompiler::SetErrorCallback(onerror);

	DeltaCompiler::AddExternFuncs(DeltaStdLib_FuncNames());

	if (DeltaCompiler::Compile(SRC) && !DeltaCompErrorsExist()) {
		DeltaCompiler::DumpBinaryCode(BIN);
		DeltaCompiler::DumpInterCode(ICODE);
		DeltaCompiler::DumpTextCode(TXT);
		DeltaCompiler::DumpUnparsed(UNP);
	}

	typedef DeltaBuildDependencies::Dependencies Deps;
	Deps deps;
	if (!DeltaBuildDependencies::Extract("", SRC, &deps))
		printf("Parse error in dependencies.\n");
	else
		for (Deps::iterator i = deps.begin(); i != deps.end(); ++i)
			printf(
				"Depends on '%s' (%s)\n", 
				i->first.c_str(), 
				i->second == DeltaBuildDependencies::NotFound ? "file not found; failed" :
				i->second == DeltaBuildDependencies::OneFound ? "one file found; resolved" :
				"many files; ambiguous"
			);
	DeltaCompiler::CleanUp();

	DeltaCompilerInit::CleanUp();

	UtilPackage::CleanUp();
	dclose();
	return 0;
}
