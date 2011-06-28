/**
 *	MessageRouter.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_MESSAGEROUTER_H
#define IDE_MESSAGEROUTER_H

#include "Common.h"
#include "Singleton.h"
#include "IDECommon.h"
#include "Message.h"

#include <boost/signal.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

class ComponentFuncEntry;

////////////////////////////////////////////////////////////////////////

class _IDE_API MessageRouter : public util::Singleton {
	DECLARE_SINGLETON(MessageRouter);

public:

	// callerId: string, msg: Message, broadCasted: bool, func: ComponentFuncEntry, result: bool
	//
	typedef boost::signal<
		void (const std::string&, const Message&, bool, const ComponentFuncEntry&)
	> Signal;

	typedef boost::signal<
		void (const std::string&, const Message&, bool, const ComponentFuncEntry&, bool)
	> SignalResult;

	// msg: Message
	//
	typedef boost::signal<void (const Message&)>	UndoSignal;

	// callerId : string
	//
	typedef boost::signal<void (const std::string)>	BatchUndoSignal;

	//******************************************************************

	bool DispatchMessage (const std::string& callerId, Message& result, const Message& msg);
	bool DispatchMessageToAll (const std::string& callerId, MessageVec& results, const Message& msg);

	void DispatchSignal (const Message& msg);

	void BeginBatchUndoAction (const std::string& callerId);
	void EndBatchUndoAction (const std::string& callerId);
	void RegisterUndoMessage (const Message& msg);

	Signal			sigPreDispatchMessage;
	SignalResult	sigPostDispatchMessage;

	BatchUndoSignal	sigBatchUndoBegin;
	BatchUndoSignal	sigBatchUndoEnd;
	UndoSignal		sigRegisteredUndoMessage;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MESSAGEROUTER_H
