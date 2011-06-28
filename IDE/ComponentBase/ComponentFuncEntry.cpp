/**
 *	ComponentRegistry.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentFuncEntry.h"
#include "Algorithms.h"
#include "StringUtils.h"

#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentFuncEntry class
//
ComponentFuncEntry::ComponentFuncEntry (void) :
	m_memberFunc(0), m_staticFunc(0)
{
}

//**********************************************************************

ComponentFuncEntry::ComponentFuncEntry (
		MemberFunc			memberFunc,
		StaticFunc			staticFunc,
		const std::string&	retType,
		const std::string&	id,
		const std::string&	argList,
		DocFunc				docFunc,
		bool				isSlot
	) : m_memberFunc(memberFunc), m_staticFunc(staticFunc), m_slot(isSlot), m_id(id),
	m_docFunc(docFunc)
{
	FunctionDecomposeRetType(m_retType, retType);
	FunctionDecomposeArgs(m_argVec, argList);
}

//**********************************************************************

ComponentFuncEntry::ComponentFuncEntry (
		MemberFunc			func,
		const std::string&	retType,
		const std::string&	id,
		const std::string&	argList,
		DocFunc				docFunc,
		bool				isSlot
	) : m_memberFunc(func), m_staticFunc(0), m_slot(isSlot), m_id(id), m_docFunc(docFunc)
{
	FunctionDecomposeRetType(m_retType, retType);
	FunctionDecomposeArgs(m_argVec, argList);
}

//**********************************************************************

ComponentFuncEntry::ComponentFuncEntry (
		StaticFunc			func,
		const std::string&	retType,
		const std::string&	id,
		const std::string&	argList,
		DocFunc				docFunc,
		bool				isSlot
	) : m_memberFunc(0), m_staticFunc(func), m_slot(isSlot), m_id(id), m_docFunc(docFunc)
{
	FunctionDecomposeRetType(m_retType, retType);
	FunctionDecomposeArgs(m_argVec, argList);
}

//**********************************************************************

ComponentFuncEntry::~ComponentFuncEntry (void)
{

}

//**********************************************************************

const std::string ComponentFuncEntry::GetArguments (void) const
{
	return FunctionGetArgList(m_argVec);
}

//**********************************************************************

const std::string ComponentFuncEntry::GetSignature (void) const
{
	return this->GetRetType() + " " + m_id + " (" + this->GetArguments() + ")";
}

//**********************************************************************

const String ComponentFuncEntry::GetDocumentation (void) const
{
	if (m_docFunc)
		return m_docFunc(*this);
	return String();
}

//**********************************************************************

void ComponentFuncEntry::UpdateSlotSignature (const std::string& signature)
{
	const size_t index = signature.find("(");
	const std::string arglist = signature.substr(index);
	m_argVec.clear();
	FunctionDecomposeArgs(m_argVec, arglist);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
