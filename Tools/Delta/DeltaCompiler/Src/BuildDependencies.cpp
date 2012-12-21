// BuildDependencies.cpp
// Delta build dependencies.
// ScriptFighter Project.
// A. Savidis, October 2010.
//

#include "DeltaStdDefs.h"
#include "BuildDependencies.h"
#include "DeltaDependenciesScanner.h"
#include "DeltaSyntaxParser.h"
#include "ParsingContext.h"
#include "ufunctors.h"
#include "ustrings.h"
#include "ufiles.h"
#include "usystem.h"
#include <algorithm>

extern int DeltaDependencies_yydebug;

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

	unew(dynamicStrings);
	path = DNEWCLASS(std::string, (ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH)));
	if (!byteCodePath.empty())
		DPTR(path)->append(std::string(";") + byteCodePath);

	deps		= outDeps;
	hasError	= false;
	inUsing		= false;

	DeltaDependenciesFlexLexer lexer;
	ParsingContext context(lexer);
	context.Register("DeltaBuildDependencies", this);
	DeltaSyntaxParser parser(lexer, context, &DeltaDependencies_yyparse);
	parser.ParseFile(sourceFile);

	CleanUp();
	return !hasError;
}

///////////////////////////////////////////////////////////////

DeltaBuildDependencies::DeltaBuildDependencies (void) :
	dynamicStrings((DynamicStrings*) 0),
	hasError(false),
	inUsing(false),
	deps((Dependencies*) 0),
	path((std::string*) 0) {}

///////////////////////////////////////////////////////////////

