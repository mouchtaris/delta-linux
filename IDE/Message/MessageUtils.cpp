/**
 *	MessageUtils.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MessageUtils.h"
#include "ComponentRegistry.h"
#include "Component.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

void MessageSetDefaultEndpoints (Message& msg, const Message& received, Component* comp)
{
	msg.GetSrc().classId = comp->GetClassId();
	msg.GetSrc().serial = comp->GetSerial();
	msg.GetSrc().function = received.GetDst().function;
}

//**********************************************************************

void MessageSetDefaultEndpoints (Message& msg, const Message& received)
{
	msg.GetSrc().classId = received.GetDst().classId;
	msg.GetSrc().serial = 0;
	msg.GetSrc().function = received.GetDst().function;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
