// FunctionReturnTypesManager.h
// Manages function return types and keeps artificial
// entries for nresolved imported library (user defined) types.
// ScriptFighter Project.
// A. Savidis, December 2010.
//

#ifndef	FUNCTIONRETURNTYPESMANAGER_H
#define	FUNCTIONRETURNTYPESMANAGER_H

#include "LibraryTypes.h"
#include "DeltaByteCodeTypes.h"
#include "CompilerComponentDirectory.h"
#include <map>
#include <string>
#include <list>

///////////////////////////////////////////////////////////

class DeltaFunctionReturnTypesManager {

	USE_COMPILER_COMPONENT_DIRECTORY();

	private:
	typedef std::map<std::string, const DeltaLibraryUserDefinedType*> TypeMap;
	TypeMap unresolvedTypes;

	const DeltaLibraryUserDefinedType*		LookupResolved(const std::string& fullPath) const;
	const DeltaLibraryUserDefinedType*		LookupUnresolved (const std::string& fullPath) const;
	const DeltaLibraryTypeInfo				Convert (const DeltaFunctionReturnType& type);
	const DeltaLibraryUserDefinedType*		Convert (const DeltaFunctionReturnLibraryType& type);
	const DeltaFunctionReturnLibraryType*	Convert (const DeltaLibraryUserDefinedType& type);


	///////////////////////////////////////////////////////////

	public:
	void									Convert (
												const DeltaFunctionReturnTypes&			from,
												std::list<DeltaLibraryTypeInfo>*		to
											);
	void									Convert (
												const std::list<DeltaLibraryTypeInfo>&	from,
												DeltaFunctionReturnTypes*				to
												
											);

	void									Initialise (void);
	void									CleanUp (void);

	DeltaFunctionReturnTypesManager (void);
	~DeltaFunctionReturnTypesManager();
};

///////////////////////////////////////////////////////////

#define DELTARETURNTYPES_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaFunctionReturnTypesManager)))

#define DELTARETURNTYPES	DELTARETURNTYPES_EX(COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
