/**
 *	ComponentUndoRegistrar.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentUndoRegistrar.h"
#include "MessageRouter.h"
#include "Component.h"
#include "ComponentHandle.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// ScopedBatchAction
//
ComponentUndoRegistrar::ScopedBatchAction::ScopedBatchAction (const std::string& callerId) :
	m_callerId(callerId)
	{ MessageRouter::Instance().BeginBatchUndoAction(m_callerId); }

ComponentUndoRegistrar::ScopedBatchAction::~ScopedBatchAction (void)
	{ MessageRouter::Instance().EndBatchUndoAction(m_callerId); }

////////////////////////////////////////////////////////////////////////

ComponentUndoRegistrar::ComponentUndoRegistrar (
		const Handle&		handle,
		const std::string&	function
	) : m_msg(Message::Endpoint(handle.GetClassId(), handle.GetSerial(), function))
{

}

//**********************************************************************

ComponentUndoRegistrar::ComponentUndoRegistrar (
		const std::string&	classId,
		const std::string&	function
	) : m_msg(Message::Endpoint(classId, function))
{

}

//**********************************************************************

ComponentUndoRegistrar::~ComponentUndoRegistrar (void)
{

}

//**********************************************************************

bool ComponentUndoRegistrar::Invoke (Message& result, const Buffer& args) const
{
	m_msg.SetData(args);
	MessageRouter::Instance().RegisterUndoMessage(m_msg);
	return true;
}

//**********************************************************************

bool ComponentUndoRegistrar::Invoke (MessageVec& results, const Buffer& args) const
{
	m_msg.SetData(args);
	MessageRouter::Instance().RegisterUndoMessage(m_msg);
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
