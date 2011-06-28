/**
 *	ComponentHolder.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTHOLDER_H
#define IDE_COMPONENTHOLDER_H

#include "Common.h"
#include "IDECommon.h"
#include "Singleton.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentHolder : public util::Singleton {
	DECLARE_SINGLETON(ComponentHolder);

public:
	typedef std::list<Component*> ComponentList;

	ComponentHolder (void);
	~ComponentHolder (void);

	//******************************************************************

	const ComponentList	Find (const std::string& classId) const;
	Component*			Find (const std::string& classId, uint instanceSerial) const;

	//******************************************************************

};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTHOLDER_H
