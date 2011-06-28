/**
 *	ComponentSignalEmitter.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTSIGNALEMITTER_H
#define IDE_COMPONENTSIGNALEMITTER_H

#include "Common.h"
#include "IDECore.h"
#include "Message.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentSignalEmitter {
public:
	ComponentSignalEmitter (const std::string& classId, const std::string& signalId);
	~ComponentSignalEmitter (void);

	//******************************************************************

	const std::string& GetClassId (void) const { return m_msg.GetSrc().classId; }

	//******************************************************************

	bool Invoke (Component* comp, const Buffer& args) const;
	bool Invoke (const Buffer& args) const;

private:
	mutable Message m_msg;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTSIGNALEMITTER_H
