#include <stdio.h>
#include "DeltaMetaCompiler.h"
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

static void onerror (const char* error, void*) {
	fprintf(stderr, "%s", error);
	fflush(stderr);
}

int main (int argc, char* argv[]) {

	if (argc != 2)
		return 0;
	DeltaCompilerInit::Initialise();

	DeltaCompiler* compiler = DNEW(DeltaMetaCompiler);
	DPTR(compiler)->SetErrorCallback(onerror);

	DPTR(compiler)->AddExternFuncs(DeltaStdLib_FuncNames());

	if (DPTR(compiler)->Compile(SRC) && !DPTR(compiler)->ErrorsExist()) {
		DPTR(compiler)->DumpBinaryCode(BIN);
		DPTR(compiler)->DumpInterCode(ICODE);
		DPTR(compiler)->DumpTextCode(TXT);
		DPTR(compiler)->DumpUnparsed(UNP);
	}

	typedef DeltaBuildDependencies::Dependencies Deps;
	DeltaBuildDependencies dependencies;
	Deps deps;
	if (!dependencies.Extract("", SRC, &deps))
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
	DPTR(compiler)->CleanUp();
	DDELETE(compiler);

	DeltaCompilerInit::CleanUp();
	return 0;
}
