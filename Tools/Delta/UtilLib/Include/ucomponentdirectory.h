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
	typedef std::map<std::string, void*> ComponentMap;
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

	void* Get (const std::string& name) const {
		ComponentMap::const_iterator i = components.find(name);
		DASSERT(i != components.end());
		return i->second;
	}
	template <typename T>
	T*	GetEx (const std::string& name) const 
		{ return (T*) Get(name); }

	void Clear (void) { components.clear(); }

	ucomponentdirectory (void){}
	virtual ~ucomponentdirectory() { Clear(); }
};

//---------------------------------------------------------------

#define UCOMPONENT_DIRECTORY_GET(dir, type)	((type*)(dir).Get(#type))

//---------------------------------------------------------------

class ucomponentdirectoryclient {
	protected:
	ucomponentdirectory*		directory;

	public:
	ucomponentdirectory*		GetDirectory (void)
									{ return DNULLCHECK(directory); }
	const ucomponentdirectory*	GetDirectory (void) const
									{ return DNULLCHECK(directory); }
	void						SetDirectory (ucomponentdirectory* _directory)
									{ directory = DNULLCHECK(_directory); }

	ucomponentdirectoryclient (ucomponentdirectory* _directory = (ucomponentdirectory*) 0) : directory(_directory) {}
	virtual ~ucomponentdirectoryclient(){}
};

#endif	// Do not ad stuff beyond this point.