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

class DeltaDependenciesFlexLexer;
extern int DeltaDependencies_yyparse (DeltaDependenciesFlexLexer& lexer);

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaBuildDependencies {

	friend class DeltaDependenciesFlexLexer;
	friend int DeltaDependencies_yyparse (DeltaDependenciesFlexLexer& lexer);

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
	DynamicStrings*					dynamicStrings;
	bool							hasError;
	Dependencies*					deps;
	std::string*					path;
	bool							inUsing;
	
	void							CleanUp (void);
	void							Handle (const std::string& id);
	void							EnterUsing (void);
	void							ExitUsing (void);
	bool							InUsing (void);
	const std::string*				NewString (const std::string& str);

	///////////////////////////////////////////////////////////////

	public:
	void							SetError (const char* unused,...);
	bool							Extract (		// false returned on parse error.
										const std::string&	byteCodePath,
										const std::string&	sourceFile, 
										Dependencies*		outDeps
									);
	DeltaBuildDependencies (void);
	~DeltaBuildDependencies () {}
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
