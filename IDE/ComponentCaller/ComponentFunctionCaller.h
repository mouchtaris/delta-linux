/**
 *	ComponentFunctionCaller.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTFUNCTIONCALLER_H
#define IDE_COMPONENTFUNCTIONCALLER_H

#include "Common.h"
#include "IDECore.h"
#include "Message.h"

#include <exception>

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;
class Handle;

////////////////////////////////////////////////////////////////////////

class _IDE_API CallException : public std::exception {
public:
	CallException (const Message& msg);
	~CallException (void);

	const Message& GetMessage (void) const { return m_msg; }

private:
	const Message m_msg;
};

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentFunctionCaller {
public:
	ComponentFunctionCaller (void) {}

	ComponentFunctionCaller (
		Component*			caller,
		const Handle&		handle,
		const std::string&	function
	);
	ComponentFunctionCaller (
		Component*			caller,
		const std::string&	classId,
		const std::string&	function
	);
	ComponentFunctionCaller (
		const std::string&	callerId,
		const Handle&		handle,
		const std::string&	function
	);
	ComponentFunctionCaller (
		const std::string&	callerId,
		const std::string&	classId,
		const std::string&	function
	);

	~ComponentFunctionCaller (void);

	//******************************************************************

	void				SetFunction (const std::string& function);
	const std::string&	GetFunction (void) const;

	void				SetClassId (const std::string& classId);
	const std::string&	GetClassId (void) const;

	//******************************************************************

	void SetInstance (const std::string& classId);
	void SetInstance (const Handle& handle);
	void SetInstance (Component* comp);

	//******************************************************************

	bool Invoke (Message& result, const Buffer& args) const;
	bool Invoke (MessageVec& results, const Buffer& args) const;

private:
	mutable Message	m_msg;
	std::string		m_callerId;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTFUNCTIONCALLER_H
