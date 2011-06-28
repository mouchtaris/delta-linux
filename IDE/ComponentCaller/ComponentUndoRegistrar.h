/**
 *	ComponentUndoRegistrar.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTUNDOREGISTRAR_H
#define IDE_COMPONENTUNDOREGISTRAR_H

#include "Common.h"
#include "IDECore.h"
#include "Message.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;
class Handle;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentUndoRegistrar {
public:

	////////////////////////////////////////////////////////////////////

	class _IDE_API ScopedBatchAction {
	public:
		ScopedBatchAction (const std::string& callerId);
		~ScopedBatchAction (void);
	private:
		const std::string m_callerId;
	};

	////////////////////////////////////////////////////////////////////

	ComponentUndoRegistrar (void) {}

	ComponentUndoRegistrar (
		const Handle&		handle,
		const std::string&	function
	);
	ComponentUndoRegistrar (
		const std::string&	classId,
		const std::string&	function
	);

	~ComponentUndoRegistrar (void);

	//******************************************************************

	bool Invoke (Message& result, const Buffer& args) const;
	bool Invoke (MessageVec& results, const Buffer& args) const;

private:
	mutable Message	m_msg;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTUNDOREGISTRAR_H
