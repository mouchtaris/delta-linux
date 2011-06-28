/**
 * A delta compiler command line tool.
 * Using "pure" C++.
 *
 * Nikos Mouchtaris <muhtaris@ics.forth.gr>
 * February 2010
 */

//
#include "DeltaCompilerInvoker.h"
#include "DDebug.h"
#include "uinit.h"
//
#include <iostream>

/////////////////////////////

static void onError (char const* const error) {
	std::cerr << error;
	std::cerr.flush();
}

int main(int argc, char* argv[]) {
	dinit(onError);
	UtilPackage::Initialise();
	//
	{
		DeltaCompilerInvoker dcomp(argc, argv, onError);

		if (dcomp.NeedHelp())
			dcomp.WriteHelpTo(std::cout) << std::endl;
		else
			dcomp.CompileAll();

	}

	UtilPackage::CleanUp();
	dclose();

	return 0;
}
