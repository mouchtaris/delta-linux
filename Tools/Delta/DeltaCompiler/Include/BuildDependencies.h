// BuildDependencies.h
// Delta build dependencies.
// ScriptFighter Project.
// A. Savidis, October 2010.
//

#ifndef	BUILDDEPENDENCIES_H
#define	BUILDDEPENDENCIES_H

#include "DDebug.h"
#include "DeltaCompilerDefs.h"
#include "utypes.h"
#include <list>
#include <string>

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaBuildDependencies {

	friend int	DeltaDependencies_yyparse (void);
	friend int	DeltaDependencies_yylex (void* yylval);

	public:
	enum ResolutionType {
		NotFound	=	1,
		OneFound	=	2,
		ManyFound	=	3
	};
	typedef std::pair<std::string, ResolutionType>	Dependency;
	typedef std::list<Dependency>					Dependencies;

	private:
	typedef std::list<std::string*>	DynamicStrings;
	static DynamicStrings*			dynamicStrings;
	static bool						hasError;
	static Dependencies*			deps;
	static std::string*				path;
	static bool						inUsing;
	
	static void						CleanUp (void);
	static void						Handle (const std::string& id);
	static void						EnterUsing (void);
	static void						ExitUsing (void);
	static bool						InUsing (void);
	static const std::string*		NewString (const std::string& str);

	///////////////////////////////////////////////////////////////

	public:
	static void						SetError (const char* unused,...);
	static bool						Extract (		// false returned on parse error.
										const std::string&	byteCodePath,
										const std::string&	sourceFile, 
										Dependencies*		outDeps
									);	
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.