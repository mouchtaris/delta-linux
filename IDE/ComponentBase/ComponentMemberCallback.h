/**
 *	ComponentMemberCallback.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTMEMBERCALLBACK_H
#define IDE_COMPONENTMEMBERCALLBACK_H

#include "Common.h"
#include "Message.h"
#include <boost/function.hpp>

namespace ide {

class Component;

////////////////////////////////////////////////////////////////////////

#define EXPORTED_FUNCTION_ARGS \
	ide::Message& result, const ide::Message& msg

#define EXPORTED_FUNCTION_ARG_NAMES \
	result, msg

#define EXPORTED_FUNCTION_ARG_TYPES \
	ide::Message&, const ide::Message&

//**********************************************************************

typedef boost::function<bool (Component*, EXPORTED_FUNCTION_ARGS)>	ComponentMemberCallback;
typedef boost::function<bool (EXPORTED_FUNCTION_ARGS)>				ComponentStaticCallback;

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTMEMBERCALLBACK_H
