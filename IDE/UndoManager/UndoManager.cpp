/**
 *	UndoManager.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "UndoManager.h"
#include "Streams.h"
#include "ComponentRegistry.h"
#include "Component.h"
#include "MessageRouter.h"
#include "MessageUtils.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// Class UndoList
//
UndoManager::UndoList::UndoList (void)
{
	m_undoList.push_back(UndoEntry());
	m_undoIter = m_undoList.begin();
}

//**********************************************************************

UndoManager::UndoList::UndoList (const UndoList& other) : m_undoList(other.m_undoList)
{
	size_t offset = std::distance<List::const_iterator>(
			other.m_undoList.begin(), other.m_undoIter
		);
	m_undoIter = m_undoList.begin();
	std::advance(m_undoIter, offset);
}

//**********************************************************************

void UndoManager::UndoList::Append (const UndoEntry& entry)
{
	this->DropAll();
	m_undoList.push_back(entry);
	++m_undoIter;
}

//**********************************************************************

void UndoManager::UndoList::Append (const Message& msg)
	{ m_undoIter->msgs.push_back(msg); }

//**********************************************************************

void UndoManager::UndoList::Change (const UndoEntry& entry)
{
	*m_undoIter = entry;
	s_changedUndoOrRedo = true;
}

//**********************************************************************

void UndoManager::UndoList::Change (const Message& msg)
{
	m_undoIter->msgs[s_undoEntryPos] = msg;
	s_changedUndoOrRedo = true;
}

//**********************************************************************

bool UndoManager::UndoList::CanUndo (void) const
	{ return m_undoIter != m_undoList.begin(); }

bool UndoManager::UndoList::CanRedo (void) const
{
	assert(m_undoIter != m_undoList.end());
	List::iterator iter(m_undoIter);
	return ++iter != m_undoList.end();
}

//**********************************************************************

void UndoManager::UndoList::Undo (const std::string& callerId)
{
	if (this->CanUndo()) {
		if (!execute(callerId, *m_undoIter))
			m_undoIter = m_undoList.erase(m_undoIter);
		--m_undoIter;
	}
}

void UndoManager::UndoList::Redo (const std::string& callerId)
{
	if (this->CanRedo()) {
		if (!execute(callerId, *++m_undoIter)) {
			List::iterator iter(m_undoIter);
			--m_undoIter;
			m_undoList.erase(iter);
		}
	}
}

void UndoManager::UndoList::Drop (void)
{
	assert(m_undoIter != m_undoList.end());

	if (m_undoIter != m_undoList.begin())
		m_undoIter = m_undoList.erase(m_undoIter);
}

//**********************************************************************

void UndoManager::UndoList::UndoAll (const std::string& callerId)
{
	while (this->CanUndo())
		this->Undo(callerId);
}

void UndoManager::UndoList::RedoAll (const std::string& callerId)
{
	while (this->CanRedo())
		this->Redo(callerId);
}

void UndoManager::UndoList::DropAll (void)
{
	assert(m_undoIter != m_undoList.end());

	List::iterator iter(m_undoIter);
	m_undoList.erase(++iter, m_undoList.end());
}

//**********************************************************************

bool UndoManager::UndoList::execute (const std::string& callerId, UndoEntry& entry)
{
	s_undoEntryPos = entry.msgs.size();

	while (s_undoEntryPos--) {

		s_changedUndoOrRedo = false;

		const Message& msg = entry.msgs[s_undoEntryPos];
		Component* component = ComponentRegistry::Instance().
			GetComponentEntry(msg.GetDst().classId).GetInstance(msg.GetDst().serial);

		if (component)
			MessageRouter::Instance().DispatchMessage(callerId, Message(), msg);

		if (!s_changedUndoOrRedo) {
			DBGOUT << "removing entry: " << s_undoEntryPos << DBGENDL;
			MessageVec::iterator iter(entry.msgs.begin());
			std::advance(iter, s_undoEntryPos);
			entry.msgs.erase(iter);
		}
	}
	s_undoEntryPos = 0;
	return entry.msgs.size() != 0;
}

//**********************************************************************

size_t	UndoManager::UndoList::s_undoEntryPos = 0;
bool	UndoManager::UndoList::s_changedUndoOrRedo = false;

////////////////////////////////////////////////////////////////////////
// UndoManager
//
IMPLEMENT_SINGLETON(UndoManager);

//**********************************************************************

void UndoManager::OnInitialize (void)
{
	MessageRouter::Instance().sigPreDispatchMessage.connect(
			boost::bind(&UndoManager::preDispatchMessage, this, _1, _2, _3, _4)
		);
	MessageRouter::Instance().sigPostDispatchMessage.connect(
			boost::bind(&UndoManager::postDispatchMessage, this, _1, _2, _3, _4, _5)
		);
	MessageRouter::Instance().sigRegisteredUndoMessage.connect(
			boost::bind(&UndoManager::registerUndoMessage, this, _1)
		);
	MessageRouter::Instance().sigBatchUndoBegin.connect(
			boost::bind(&UndoManager::BeginBatchUndoAction, this, _1)
		);
	MessageRouter::Instance().sigBatchUndoEnd.connect(
			boost::bind(&UndoManager::EndBatchUndoAction, this, _1)
		);

	m_undoList.push_back(std::string());
	m_undoIter = m_undoList.begin();
}

//**********************************************************************

void UndoManager::OnCleanUp (void)
{
	m_undoMap.clear();
	m_undoList.clear();
	m_undoIter = m_undoList.end();
}

//**********************************************************************

void UndoManager::BeginBatchUndoAction (const std::string& callerId)
{
	//if (++m_batchUndoRedoDepth == 1 && m_dispatchDepth == 0) {
	//	this->append(callerId, UndoEntry());
	//	m_initialCallerId = callerId;
	//}
}

//**********************************************************************

void UndoManager::EndBatchUndoAction (const std::string& callerId)
{
	//--m_batchUndoRedoDepth;
}

//**********************************************************************

bool UndoManager::CanUndo (void) const
	{ return m_undoIter != m_undoList.begin(); }

bool UndoManager::CanRedo (void) const
{
	assert(m_undoIter != m_undoList.end());
	GlobalUndoList::iterator iter(m_undoIter);
	return ++iter != m_undoList.end();
}
 
//**********************************************************************

void UndoManager::Undo (void)
{
	if (this->CanUndo()) {
		this->undo(*m_undoIter);
		--m_undoIter;
	}
}

void UndoManager::Redo (void)
{
	if (this->CanRedo())
		this->redo(*++m_undoIter);
}

void UndoManager::Drop (void)
{
	assert(m_undoIter != m_undoList.end());

	if (m_undoIter != m_undoList.begin())
		m_undoIter = m_undoList.erase(m_undoIter);
}

//**********************************************************************

void UndoManager::UndoAll (void)
{
	while (m_undoIter != m_undoList.begin()) {
		this->undo(*m_undoIter);
		--m_undoIter;
	}
}

void UndoManager::RedoAll (void)
{
	GlobalUndoList::iterator iter(m_undoIter);
	while (++iter != m_undoList.end())
		this->redo(*(m_undoIter = iter));
}

void UndoManager::DropAll (void)
{
	assert(m_undoIter != m_undoList.end());

	GlobalUndoList::iterator iter(m_undoIter);
	while (++iter != m_undoList.end())
		this->drop(*iter);

	iter = m_undoIter;
	m_undoList.erase(++iter, m_undoList.end());
}

//**********************************************************************

bool UndoManager::CanUndo (const std::string& callerId) const
{
	CIDUndoMap::const_iterator iter = m_undoMap.find(callerId);
	if (iter != m_undoMap.end())
		return iter->second.CanUndo();
	return false;
}

bool UndoManager::CanRedo (const std::string& callerId) const
{
	CIDUndoMap::const_iterator iter = m_undoMap.find(callerId);
	if (iter != m_undoMap.end())
		return iter->second.CanRedo();
	return false;
}

//**********************************************************************

void UndoManager::Undo (const std::string& callerId)
{
	this->removeLast(callerId);
	this->undo(callerId); 
}

void UndoManager::Redo (const std::string& callerId)
{
	this->removeLast(callerId);
	this->redo(callerId);
}

void UndoManager::Drop (const std::string& callerId)
{
	this->removeLast(callerId);
	this->drop(callerId);
}

//**********************************************************************

void UndoManager::UndoAll (const std::string& callerId)
{
	this->removeAll(callerId);
	this->undoAll(callerId);
}

void UndoManager::RedoAll (const std::string& callerId)
{
	this->removeAll(callerId);
	this->redoAll(callerId);
}

void UndoManager::DropAll (const std::string& callerId)
{
	this->removeAll(callerId);
	this->dropAll(callerId);
}

//**********************************************************************

void UndoManager::append (const std::string& callerId, const UndoEntry& entry)
{
	m_undoMap[callerId].Append(entry);

	GlobalUndoList::iterator iter(m_undoIter);
	m_undoList.erase(++iter, m_undoList.end());
	m_undoList.push_back(callerId);
	++m_undoIter;
}

//**********************************************************************

void UndoManager::append (const std::string& callerId, const Message& msg)
	{ m_undoMap[callerId].Append(msg); }

//**********************************************************************

void UndoManager::change (const UndoEntry& entry)
	{ m_undoMap[*m_undoIter].Change(entry); }

//**********************************************************************

void UndoManager::change (const Message& msg)
	{ m_undoMap[*m_undoIter].Change(msg); }

//**********************************************************************

void UndoManager::removeLast (const std::string& callerId)
{
	if (*m_undoIter == callerId)
		m_undoIter = m_undoList.erase(m_undoIter);
	else {
		GlobalUndoList::iterator iter(m_undoIter);
		while (--iter != m_undoList.begin()) {
			if (*iter == callerId) {
				m_undoList.erase(iter);
				break;
			}
		}
	}
}

//**********************************************************************

void UndoManager::removeAll (const std::string& callerId)
{
	GlobalUndoList::iterator iter = m_undoList.begin();
	for (; iter != m_undoList.begin(); ++iter) {
		if (*iter == callerId) {
			if (iter == m_undoIter)
				m_undoIter = m_undoList.erase(iter);
			else
				m_undoList.erase(iter);
		}
	}
}

//**********************************************************************

void UndoManager::undo (const std::string& callerId)
{
	m_duringUndoRedo = true;
	m_undoMap[callerId].Undo(callerId);
	m_duringUndoRedo = false;
}

void UndoManager::redo (const std::string& callerId)
{
	m_duringUndoRedo = true;
	m_undoMap[callerId].Redo(callerId);
	m_duringUndoRedo = false;
}

void UndoManager::drop (const std::string& callerId)
{
	m_duringUndoRedo = true;
	m_undoMap[callerId].Drop();
	m_duringUndoRedo = false;
}

//**********************************************************************

void UndoManager::undoAll (const std::string& callerId)
{
	m_duringUndoRedo = true;
	m_undoMap[callerId].UndoAll(callerId);
	m_duringUndoRedo = false;
}

void UndoManager::redoAll (const std::string& callerId)
{
	m_duringUndoRedo = true;
	m_undoMap[callerId].RedoAll(callerId);
	m_duringUndoRedo = false;
}

void UndoManager::dropAll (const std::string& callerId)
{
	m_duringUndoRedo = true;
	m_undoMap[callerId].DropAll();
	m_duringUndoRedo = false;
}

//**********************************************************************

void UndoManager::preDispatchMessage (
		const std::string&			callerId,
		const Message&				msg,
		bool						broadCast,
		const ComponentFuncEntry&	func
	)
{
	//DBGOUT << callerId << " --> " << msg.GetDst().classId << "[" << msg.GetDst().serial << "]" <<
	//	"." << msg.GetDst().function << "()" << std::endl;

	if (m_dispatchDepth++ == 0 && !m_duringUndoRedo) {

		if (m_batchUndoRedoDepth == 0)
			m_initialCallerId = callerId;

		m_storedUndoActionDepth = 0;
	}

	if (broadCast)
		this->BeginBatchUndoAction(callerId);
}

//**********************************************************************

void UndoManager::postDispatchMessage (
		const std::string&			callerId,
		const Message&				msg,
		bool						broadCast,
		const ComponentFuncEntry&	func,
		bool						result
	)
{
	//DBGOUT << callerId << " --> " << msg.GetDst().classId << "[" << msg.GetDst().serial << "]" <<
	//	"." << msg.GetDst().function << "()" << std::endl;

	if (--m_dispatchDepth == 0)
		m_storedUndoActionDepth = 0;

	if (broadCast)
		this->EndBatchUndoAction(callerId);
}

//**********************************************************************

void UndoManager::registerUndoMessage (const Message& msg)
{
	//The following code causes undo registrations of depth 0 to be ignored.
	//In case we get a non component system notification (e.g. EditorClosed) and invoke a member function we
	//end up not being able to register the undo message.
	//TODO: Double check that removing it causes correct behaviour.
	//if (!m_dispatchDepth)
	//	return;

	if (m_duringUndoRedo) {

		if (m_storedUndoActionDepth == 0 || m_storedUndoActionDepth >= m_dispatchDepth) {
			this->change(msg);
			m_storedUndoActionDepth = m_dispatchDepth;
		}
	}
	else if (m_storedUndoActionDepth == 0) {

		this->append(m_initialCallerId, UndoEntry(msg));
		m_storedUndoActionDepth = m_dispatchDepth;

		DBGOUT << "Stored Undo action with callerId: " << m_initialCallerId <<
			", function: " << msg.GetDst().classId << "[" << msg.GetDst().serial << "]" <<
			"." << msg.GetDst().function << "()" << DBGENDL;
	}
	else if (m_storedUndoActionDepth == m_dispatchDepth) {

		this->append(m_initialCallerId, msg);

		DBGOUT << "Added Undo action in batch mode: " << m_initialCallerId <<
			", function: " << msg.GetDst().classId << "[" << msg.GetDst().serial << "]" <<
			"." << msg.GetDst().function << "()" << DBGENDL;
	}
	else if (m_storedUndoActionDepth > m_dispatchDepth) {

		this->change(UndoEntry(msg));
		m_storedUndoActionDepth = m_dispatchDepth;

		DBGOUT << "Replaced Undo action with: " << m_initialCallerId <<
			", function: " << msg.GetDst().classId << "[" << msg.GetDst().serial << "]" <<
			"." << msg.GetDst().function << "()" << DBGENDL;
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
