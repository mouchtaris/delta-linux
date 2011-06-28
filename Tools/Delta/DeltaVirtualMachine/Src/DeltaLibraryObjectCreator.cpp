// DeltaLibraryObjectCreator.cpp
// Implementation of the creator.
// A. Savidis, December 2007.
//

#include "DeltaLibraryObjectCreator.h"
#include "DeltaLibFuncBinder.h"

/////////////////////////////////////////////////////////////

DeltaObject*	DeltaLibraryObjectCreator::Create (const Functions& info, DeltaObject* obj) 
	{ return CreateTemplate(info, "function", obj); }

DeltaObject*	DeltaLibraryObjectCreator::Create (const APIs& info, DeltaObject* obj)
	{ return CreateTemplate(info, "API", obj); }

/////////////////////////////////////////////////////////////

void DeltaLibraryObjectCreator::Handle (
		const Functions&	info,
		const char*			prefix,
		bool				(*f)(const std::string&, DeltaLibraryFunc)
	) {
		FuncEntry*	p = info.first;
		util_ui32	n = info.second;

		while (n--) {
			std::string name;
			if (p->name[0] == DELTA_LIBRARY_RETAINNAME_PREFIX[0])
				name = p->name + 1;
			else {
				if (prefix)
					name.append(prefix);
				name.append(p->name);
			}
			(*f)(name, p->func);
			++p;
		}
}

void DeltaLibraryObjectCreator::Install (const Functions& info, const char* prefix) 
	{ Handle(info, prefix, &DeltaLibFuncBinder::Add); }

void DeltaLibraryObjectCreator::Uninstall (const Functions& info, const char* prefix) 
	{ Handle(info, prefix, &DeltaLibFuncBinder::Remove); }

/////////////////////////////////////////////////////////////

void DeltaLibraryObjectCreator::Add (
		DeltaObject* mainAPI, 
		DeltaObject*	subAPI,
		const char*		name
	) {
	APIEntry api[] = { { name, subAPI } };
	APIs apis(api, 1);
	Create(apis, mainAPI);
}


/////////////////////////////////////////////////////////////