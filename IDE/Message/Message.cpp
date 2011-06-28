/**
 *	Message.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Message.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

Message::Message (void)
{
}

//**********************************************************************

Message::~Message (void)
{
}

////////////////////////////////////////////////////////////////////////

Message::Endpoint::Endpoint (void) : serial(0)
{
}

//**********************************************************************

Message::Endpoint::~Endpoint (void)
{
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
