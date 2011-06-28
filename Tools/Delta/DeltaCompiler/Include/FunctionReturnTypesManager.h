// FunctionReturnTypesManager.h
// Manages function return types and keeps artificial
// entries for nresolved imported library (user defined) types.
// ScriptFighter Project.
// A. Savidis, December 2010.
//

#ifndef	FUNCTIONRETURNTYPESMANAGER_H
#define	FUNCTIONRETURNTYPESMANAGER_H

#include "usingleton.h"
#include "LibraryTypes.h"
#include "DeltaByteCodeTypes.h"
#include <map>
#include <string>
#include <list>

///////////////////////////////////////////////////////////

class DeltaFunctionReturnTypesManager {
	private:
	static DeltaFunctionReturnTypesManager* singletonPtr;
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

	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	USINGLETON_APISTYLE_DECLARE_GETTER(DeltaFunctionReturnTypesManager)

	DeltaFunctionReturnTypesManager (void);
	~DeltaFunctionReturnTypesManager();
};

///////////////////////////////////////////////////////////

#define	DELTARETURNTYPES	GetReturnTypes()

inline DeltaFunctionReturnTypesManager& GetReturnTypes (void) 
	{  return DeltaFunctionReturnTypesManager::GetSingleton(); }

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
