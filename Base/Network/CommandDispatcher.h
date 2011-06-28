/**
 *	CommandDispatcher.h
 *
 * The command dispatcher is a low-level construct that associates int
 * commands with Handler functions which are responsible for decomposing
 * the received message and potentially reply to the other endpoint.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_COMMANDDISPATCHER_H
#define NET_COMMANDDISPATCHER_H

#include "Common.h"
#include "InstanceValidator.h"

#include <map>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace comm {
	class encoder;
	class decoder;
}

namespace net {

////////////////////////////////////////////////////////////////////////

template<class TLink, class TCommand, class TEncoder = comm::encoder, class TDecoder = comm::decoder>
class CommandDispatcher {
public:
	typedef TCommand									CommandType;
	typedef TLink										LinkType;
	typedef boost::shared_ptr<TLink>					LinkPtrType;
	typedef TEncoder									EncoderType;
	typedef TDecoder									DecoderType;

	typedef boost_asio_error_t							ErrorType;
	typedef util::instance_validator<CommandDispatcher> ValidatorType;

	//******************************************************************

	typedef boost::function<
		bool (PARAM(CommandType), LinkPtrType, REF(DecoderType))
	> Handler;

	typedef boost::function<
		void (PARAM(CommandType), LinkPtrType)
	> CmdErrorHandler;

	typedef boost::function<
		void (const ErrorType&, LinkPtrType)
	> LinkErrorHandler;

	//******************************************************************

	CommandDispatcher (void) : m_continuousMonitor(false)
	{
		ValidatorType::scoped_lock lock(s_validator);
		s_validator.add(this);
	}
	virtual ~CommandDispatcher (void)
	{
		ValidatorType::scoped_lock lock(s_validator);
		s_validator.remove(this);
	}

	//******************************************************************

	void SetContinuousMonitor (bool continuousMonitor = true)
		{ m_continuousMonitor = continuousMonitor; }

	//******************************************************************

	virtual bool RegisterCommand (PARAM(CommandType) command, const Handler& handler)
		{ return m_commandMap.insert(CommandMap::value_type(command, handler)).second; }

	virtual void ClearCommands (void) { m_commandMap.clear(); }

	//******************************************************************

	void SetCommandFailedHandler (const CmdErrorHandler& handler)
		{ m_commandFailedHandler = handler; }

	void SetUndefinedCommandHandler (const CmdErrorHandler& handler)
		{ m_undefinedCommandHandler = handler; }
	
	void SetLinkErrorHandler (const LinkErrorHandler& handler)
		{ m_linkErrorHandler = handler; }

	void ClearHandlers (void)
	{
		m_undefinedCommandHandler.clear();
		m_commandFailedHandler.clear();
		m_linkErrorHandler.clear();
	}

	//******************************************************************

	static void ExtractCommand (REF(CommandType) command, REF(DecoderType) decoder)
		{ decoder.decode(command); }

	static void InsertCommand (PARAM(CommandType) command, REF(EncoderType) encoder)
		{ encoder.encode(command); } 

	//******************************************************************

	void ReadCommandFromLink (LinkPtrType link)
	{
		link->async_read(boost::bind(
			&CommandDispatcher::readCommand, this, _1, _2, link));
	}

	//******************************************************************

	void ReportLinkError (const ErrorType& error, LinkPtrType link)
	{
		if (m_linkErrorHandler)
			m_linkErrorHandler(error, link);	
	}

	//******************************************************************

	void DispatchCommand (
		PARAM(CommandType) command, LinkPtrType link, REF(DecoderType) decoder
	) const {
		CommandMap::const_iterator iter = m_commandMap.find(command);
		if (iter != m_commandMap.end()) {
			if (!iter->second(command, link, decoder) && m_commandFailedHandler)
				m_commandFailedHandler(command, link);
		}
		else if (m_undefinedCommandHandler)
			m_undefinedCommandHandler(command, link);
	}

	//******************************************************************

protected:
	virtual void HandleDispatch (const std::string& data, LinkPtrType conn)
	{
		CommandType command;
		DecoderType decoder(data);
		decoder >> command;
		this->DispatchCommand(command, conn, decoder);
	}

	virtual void HandleLinkError (const ErrorType& error, LinkPtrType conn)
		{ this->ReportLinkError(error, conn); }

private:
	typedef std::map<VALUE(CommandType), Handler> CommandMap;

	//******************************************************************

	void readCommand (const ErrorType& error, const std::string& data, LinkPtrType conn)
	{
		ValidatorType::scoped_lock lock(s_validator);

		if (!s_validator.contains(this))
			return;

		if (!error) {

			this->HandleDispatch(data, conn);

			if (m_continuousMonitor)
				this->ReadCommandFromLink(conn);
		}
		else {
			this->HandleLinkError(error, conn);
		}
	}

	CommandMap				m_commandMap;
	CmdErrorHandler			m_undefinedCommandHandler;
	CmdErrorHandler			m_commandFailedHandler;
	LinkErrorHandler		m_linkErrorHandler;
	bool					m_continuousMonitor;

	static ValidatorType	s_validator;
};

//**********************************************************************

template<class TLink, class TCommand, class TEncoder, class TDecoder>
typename CommandDispatcher<TLink, TCommand, TEncoder, TDecoder>::ValidatorType
CommandDispatcher<TLink, TCommand, TEncoder, TDecoder>::s_validator;

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_COMMANDDISPATCHER_H
