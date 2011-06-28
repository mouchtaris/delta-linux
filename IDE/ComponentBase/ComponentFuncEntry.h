/**
 *	ComponentFuncEntry.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTFUNCENTRY_H
#define IDE_COMPONENTFUNCENTRY_H

#include "Common.h"
#include "IDECommon.h"
#include "ComponentMemberCallback.h"
#include "FunctionUtils.h"

#include <vector>

#include <boost/function.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;
class ComponentEntry;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentFuncEntry {
public:
	typedef const String (*FuncDocFunc) (const ComponentFuncEntry&);
	typedef boost::function<const String (const ComponentFuncEntry&)> DocFunc;

	typedef ComponentMemberCallback	MemberFunc;
	typedef ComponentStaticCallback	StaticFunc;

	//******************************************************************

	ComponentFuncEntry (void);

	ComponentFuncEntry (
		MemberFunc			memberFunc,
		StaticFunc			staticFunc,
		const std::string&	retType = std::string(),
		const std::string&	id = std::string(),
		const std::string&	argList = std::string(),
		DocFunc				docFunc = DocFunc(),
		bool				isSlot = false
	);
	ComponentFuncEntry (
		MemberFunc			func,
		const std::string&	retType = std::string(),
		const std::string&	id = std::string(),
		const std::string&	argList = std::string(),
		DocFunc				docFunc = DocFunc(),
		bool				isSlot = false
	);
	ComponentFuncEntry (
		StaticFunc			func,
		const std::string&	retType = std::string(),
		const std::string&	id = std::string(),
		const std::string&	argList = std::string(),
		DocFunc				docFunc = DocFunc(),
		bool				isSlot = false
	);
	~ComponentFuncEntry (void);

	//******************************************************************

	operator bool (void) const { return !!m_memberFunc || !!m_staticFunc; }

	bool IsMemberFunc (void) const { return !!m_memberFunc; }
	bool IsStaticFunc (void) const { return !!m_staticFunc; }

	//******************************************************************

	MemberFunc GetMemberFunc (void) const { return m_memberFunc; }
	StaticFunc GetStaticFunc (void) const { return m_staticFunc; }

	const std::string&	GetRetType (void) const { return m_retType; }
	const std::string&	GetId (void) const { return m_id; }
	const ArgumentVec&	GetArgumentVec (void) const { return m_argVec; }
	const std::string	GetArguments (void) const;

	//******************************************************************

	const std::string	GetSignature (void) const;
	const String		GetDocumentation (void) const;

	//******************************************************************

	bool IsSlot	(void) const { return m_slot; }

private:
	friend class Component;
	friend class ComponentEntry;

	void UpdateSlotSignature (const std::string& signature);

	MemberFunc			m_memberFunc;
	StaticFunc			m_staticFunc;

	std::string			m_retType;
	ArgumentVec			m_argVec;

	const std::string	m_id;
	DocFunc				m_docFunc;

	bool				m_slot;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTFUNCENTRY_H
