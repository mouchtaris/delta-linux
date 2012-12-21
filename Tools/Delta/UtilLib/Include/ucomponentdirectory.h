// ucomponentdirectory.h
// Directory of components.
// ScriptFighter Project.
// Y. Lilis, July 2011.
//

#ifndef	UCOMPONENTDIRECTORY_H
#define	UCOMPONENTDIRECTORY_H

#include "DDebug.h"
#include "utypes.h"
#include <map>

//---------------------------------------------------------------

class ucomponentdirectory {
	protected:
	typedef std::map<std::string, void *> ComponentMap;
	ComponentMap components;

	public:
	void Register (const std::string& name, void* component) {
		DASSERT(components.find(name) == components.end());
		components[name] = component;
	}

	void Unregister (const std::string& name) {
		ComponentMap::iterator i = components.find(name);
		DASSERT(i != components.end());
		components.erase(i);
	}

	void* Get (const std::string& name) {
		ComponentMap::iterator i = components.find(name);
		DASSERT(i != components.end());
		return i->second;
	}

	void Clear (void) { components.clear(); }

	ucomponentdirectory (void) {}
	virtual ~ucomponentdirectory () { Clear(); }
};

//---------------------------------------------------------------

#define UCOMPONENT_DIRECTORY_GET(dir, type)	((type*)(dir).Get(#type))

//---------------------------------------------------------------

#endif	// Do not ad stuff beyond this point.