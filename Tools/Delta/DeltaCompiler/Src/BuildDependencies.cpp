// BuildDependencies.cpp
// Delta build dependencies.
// ScriptFighter Project.
// A. Savidis, October 2010.
//

#include "DeltaStdDefs.h"
#include "BuildDependencies.h"
#include "ufunctors.h"
#include "ustrings.h"
#include "ufiles.h"
#include "usystem.h"
#include <algorithm>

extern int		DeltaDependencies_yydebug;
extern FILE*	DeltaDependencies_yyin;
extern void		DeltaDependencies_ResetLex (FILE* fp);
extern int		DeltaDependencies_yyparse (void);

DeltaBuildDependencies::DynamicStrings*	DeltaBuildDependencies::dynamicStrings	= (DynamicStrings*) 0;
bool									DeltaBuildDependencies::hasError		= false;
bool									DeltaBuildDependencies::inUsing			= false;
DeltaBuildDependencies::Dependencies*	DeltaBuildDependencies::deps			= (Dependencies*) 0;
std::string*							DeltaBuildDependencies::path			= (std::string*) 0;

///////////////////////////////////////////////////////////////

void DeltaBuildDependencies::CleanUp (void) {
	std::for_each(
		DPTR(dynamicStrings)->begin(),
		DPTR(dynamicStrings)->end(),
		uptrdestructorfunctor<std::string*>()
	);
	udelete(dynamicStrings);
	udelete(path);
	unullify(deps);
}

///////////////////////////////////////////////////////////////

void DeltaBuildDependencies::EnterUsing (void)
	{ DASSERT(!inUsing); inUsing = true; }

void DeltaBuildDependencies::ExitUsing (void)
	{ DASSERT(inUsing); inUsing = false; }

bool DeltaBuildDependencies::InUsing (void)
	{ return inUsing; }

///////////////////////////////////////////////////////////////

void DeltaBuildDependencies::Handle (const std::string& id) {
	std::string file = id + ".dbc";
	ustrlist fullPaths = ufileresolveallwithpaths(file, utokenizestr(*path, ";,"), false);	// Ignore cwd directory.
	if (fullPaths.empty())
		deps->push_back(Dependency(file, NotFound));
	else
	if (fullPaths.size() > 1)
		deps->push_back(Dependency(ustrconcat(fullPaths, ";"), ManyFound));
	else
		deps->push_back(Dependency(fullPaths.front(), OneFound));
}

///////////////////////////////////////////////////////////////

const std::string* DeltaBuildDependencies::NewString (const std::string& str) {
	DASSERT(dynamicStrings);
	DPTR(dynamicStrings)->push_back(DNEWCLASS(std::string, (str)));
	return DPTR(dynamicStrings)->back();
}

///////////////////////////////////////////////////////////////

void DeltaBuildDependencies::SetError (const char* format,...) 
	{ hasError = true; }

///////////////////////////////////////////////////////////////

bool DeltaBuildDependencies::Extract (	
		const std::string&	byteCodePath,
		const std::string&	sourceFile, 
		Dependencies*		outDeps
	) {

	DASSERT(!dynamicStrings && !hasError && !deps && !path);
	FILE* fp =  fopen(sourceFile.c_str(), "rt");
	if (!fp)
		return false;

	DeltaDependencies_ResetLex(fp);

	unew(dynamicStrings);
	path = DNEWCLASS(std::string, (ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH)));
	if (!byteCodePath.empty())
		DPTR(path)->append(std::string(";") + byteCodePath);

	deps		= outDeps;
	hasError	= false;
	inUsing		= false;
	DeltaDependencies_yyparse();

	fclose(fp);
	CleanUp();
	return !hasError;
}

///////////////////////////////////////////////////////////////
