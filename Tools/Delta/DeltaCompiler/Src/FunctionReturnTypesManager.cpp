// FunctionReturnTypesManager.cpp
// Manages function return types and keeps artificial
// entries for nresolved imported library (user defined) types.
// ScriptFighter Project.
// A. Savidis, December 2010.
//

#include "FunctionReturnTypesManager.h"
#include "LibraryNamespaceHolder.h"
#include <algorithm>
#include "ufunctors.h"

///////////////////////////////////////////////////////////

DeltaFunctionReturnTypesManager::DeltaFunctionReturnTypesManager (ucomponentdirectory* directory) :
	ucomponentdirectoryclient(directory) {}
DeltaFunctionReturnTypesManager::~DeltaFunctionReturnTypesManager () 
	{ CleanUp(); }

///////////////////////////////////////////////////////////

void DeltaFunctionReturnTypesManager::Initialise (void) 
	{ DASSERT(unresolvedTypes.empty()); }

void DeltaFunctionReturnTypesManager::CleanUp (void) {
	std::for_each(
		unresolvedTypes.begin(),
		unresolvedTypes.end(),
		udestroy_second<std::pair<std::string, const DeltaLibraryUserDefinedType*> >()
	);
	unresolvedTypes.clear();
}

///////////////////////////////////////////////////////////

const DeltaLibraryUserDefinedType* DeltaFunctionReturnTypesManager::LookupResolved (const std::string& fullPath) const {

	DeltaLibraryNamespace*	global	= DNULLCHECK(DELTANAMESPACES.GetGlobalNamespace());
	ustrlist				path	= utokenizestr(fullPath, ":");
	DASSERT(!path.empty());

	std::string typeName = path.back();
	path.pop_back();

	for (ustrlist::iterator i = path.begin(); i != path.end(); ++i)
		if (!(global = DPTR(global)->LookupNamespace(*i)))
			return (DeltaLibraryUserDefinedType*) 0;	// Could not find namespace

	return DPTR(global)->LookupType(typeName);
}

//**************************

const DeltaLibraryUserDefinedType* DeltaFunctionReturnTypesManager::LookupUnresolved (const std::string& fullPath) const {
	TypeMap::const_iterator i = unresolvedTypes.find(fullPath);
	return i != unresolvedTypes.end() ? i->second : (DeltaLibraryUserDefinedType*) 0;
}

///////////////////////////////////////////////////////////

const DeltaLibraryTypeInfo DeltaFunctionReturnTypesManager::Convert (const DeltaFunctionReturnType& type) {
	if (type.IsBasicType())
		return DeltaLibraryTypeInfo(type.GetBasicType());
	else
		return DeltaLibraryTypeInfo(Convert(*DPTR(type.GetLibraryType())));
}

//**************************

const DeltaLibraryUserDefinedType* DeltaFunctionReturnTypesManager::Convert (const DeltaFunctionReturnLibraryType& type) {
	if (const DeltaLibraryUserDefinedType* userType = LookupResolved(type.GetFullPath()))
		return userType;
	else
	if (userType == LookupUnresolved(type.GetFullPath()))
		return userType;
	else {
		std::string	typeName = utokenizestr(type.GetFullPath(), ":").back();
		unresolvedTypes[type.GetFullPath()] = userType = DNEWCLASS(DeltaLibraryUserDefinedType, (typeName, type.GetFullPath()));

		typedef std::list<const DeltaFunctionReturnLibraryType*> BaseList;
		const BaseList& bases = type.GetBaseTypes();
		for (BaseList::const_iterator i = bases.begin(); i != bases.end(); ++i)
			const_cast<DeltaLibraryUserDefinedType*>(userType)->AddBaseType(Convert(**i));

		return userType;
	}
}

//**************************

const DeltaFunctionReturnLibraryType* DeltaFunctionReturnTypesManager::Convert (const DeltaLibraryUserDefinedType& type) {

	DeltaFunctionReturnLibraryType* retType = DNEWCLASS(DeltaFunctionReturnLibraryType, (type.GetFullPath()));

	typedef ubag<const DeltaLibraryUserDefinedType*> BaseBag;
	const BaseBag& bases = type.GetBaseTypes();

	for (BaseBag::const_iterator i = bases.begin(); i != bases.end(); ++i)
		DPTR(retType)->AddBaseType(Convert(*DPTR(i->first)));

	return retType;
}

///////////////////////////////////////////////////////////

void DeltaFunctionReturnTypesManager::Convert (
		const DeltaFunctionReturnTypes&			from,
		std::list<DeltaLibraryTypeInfo>*		to
	) {
	typedef std::list<const DeltaFunctionReturnType*> TypeList;
	const TypeList& types = from.GetReturnTypes();
	for (TypeList::const_iterator i = types.begin(); i != types.end(); ++i)
		to->push_back(Convert(**i));
}

//**************************

void DeltaFunctionReturnTypesManager::Convert (
		const std::list<DeltaLibraryTypeInfo>&	from,
		DeltaFunctionReturnTypes*				to
	) {
	typedef std::list<DeltaLibraryTypeInfo> TypeInfoList;
	for (TypeInfoList::const_iterator i = from.begin(); i != from.end(); ++i)
		if (i->IsBuiltIn())
			to->IncludeReturnType(i->GetBuiltIn());
		else
			to->IncludeReturnType(Convert(*DPTR(i->GetUserDefined())));
}

///////////////////////////////////////////////////////////
