// Test.cpp.
// Testing Delta compiler.
// ScriptFighter Project
// A. Savidis, October 1999.
//

#include <stdio.h>
#include "DDebug.h"
#include "CompilerAPI.h"
#include "DeltaStdLibFuncNames.h"
#include "RcLibFuncNames.h"
#include "DeltaALADINLibFuncNames.h"

// The postfix .dsc stands for Delta Source Coce,
// while .dbc stands for Delta Byte Code.
//
#define	TEST_INPUT_FILE_NAME	"aladintest.dsc"
#define	TEST_OUT_FILE_NAME		"aladintest.dbc"

static void OnError (char* msg) {
	fprintf(stderr,"%s", msg);
}


FILE* errorFile;

static void onderror (char* msg) {
	fprintf(errorFile, "%s", msg);
}


main() {

	errorFile=fopen("derror.txt", "wt");
	dseterrorcallback(onderror);

	DeltaCompiler::SetErrorCallback(OnError);

	char* includeDirs[]={
		"e:\\edutainment\\scriptfighter\\deltacompiler\\tests",
		(char*) 0
	};

	char* defines[]={
		"DEBUG",
		"TEST",
		(char*) 0
	};

	DeltaCompiler::SetCPPArguments(
		"d:\\bin\\cpp",
		defines,
		includeDirs
	);

	DeltaCompiler::SetCPPUse(true);
	DeltaCompiler::AddExternFuncs(DeltaStdLib_FuncNames());
	DeltaCompiler::AddExternFuncs(RcLib_FuncNames());
	DeltaCompiler::AddExternFuncs(DeltaALADINLib_FuncNames());

	for (unsigned int i=0; i<1; i++) {
		fprintf(stderr, "Compilation '%d'.\n", i+1);
		DeltaCompiler::Compile(TEST_INPUT_FILE_NAME);
		DeltaCompiler::DumpBinaryCode(TEST_OUT_FILE_NAME, true);
		DeltaCompiler::DumpInterCode("quads.txt");
		DeltaCompiler::DumpTextCode("instructions.txt");
	}

	DeltaCompiler::CleanUp();
	dclose();
	fclose(errorFile);

	return 1;
}