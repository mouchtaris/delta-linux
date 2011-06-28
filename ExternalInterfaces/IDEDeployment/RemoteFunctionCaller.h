/**
 *	RemoteFunctionCaller.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EXT_REMOTEFUNCTIONCALLER_H
#define EXT_REMOTEFUNCTIONCALLER_H

#include "Common.h"
#include "IDECore.h"
#include "ComponentHandle.h"
#include "MessageRouterClient.h"
#include "IDEDeploymentCommon.h"

#include <exception>

namespace ext {

////////////////////////////////////////////////////////////////////////

class _DEPLOY_API RemoteCallException : public std::exception {
public:
	RemoteCallException (const Message& msg);
	~RemoteCallException (void);

	const Message& GetMessage (void) const { return m_msg; }

private:
	const Message m_msg;
};

////////////////////////////////////////////////////////////////////////

class _DEPLOY_API RemoteFunctionCaller {
public:
	RemoteFunctionCaller (void) {}

	RemoteFunctionCaller (
		const std::string&	callerId,
		const std::string&	classId,
		const std::string&	function
	);
	RemoteFunctionCaller (
		const std::string&	callerId,
		const ide::Handle&	handle,
		const std::string&	function
	);

	~RemoteFunctionCaller (void);

	//******************************************************************

	void				SetFunction (const std::string& function);
	const std::string&	GetFunction (void) const;

	void				SetClassId (const std::string& classId);
	const std::string&	GetClassId (void) const;

	//******************************************************************

	void SetInstance (const std::string& classId);

	//******************************************************************

	bool Invoke (Message& result, const Buffer& args) const;
	bool Invoke (MessageVec& results, const Buffer& args) const;

private:
	mutable Message	m_msg;
	std::string		m_callerId;
};

////////////////////////////////////////////////////////////////////////

} // namespace ext

#endif // EXT_REMOTEFUNCTIONCALLER_H
