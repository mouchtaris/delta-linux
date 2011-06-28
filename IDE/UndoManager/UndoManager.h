/**
 *	UndoManager.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_UNDOMANAGER_H
#define IDE_UNDOMANAGER_H

#include "Common.h"
#include "Singleton.h"
#include "IDECommon.h"
#include "Message.h"

#include <map>
#include <list>
#include <vector>

namespace ide {

////////////////////////////////////////////////////////////////////////

class ComponentFuncEntry;

////////////////////////////////////////////////////////////////////////

class _IDE_API UndoManager : public util::Singleton {
	DECLARE_SINGLETON(UndoManager);

public:
	void OnInitialize (void);
	void OnCleanUp (void);

	//******************************************************************

	void BeginBatchUndoAction (const std::string& callerId);
	void EndBatchUndoAction (const std::string& callerId);

	//******************************************************************

	bool CanUndo (void) const;
	bool CanRedo (void) const;

	void Undo (void);
	void Redo (void);
	void Drop (void);

	void UndoAll (void);
	void RedoAll (void);
	void DropAll (void);

	//******************************************************************

	bool CanUndo (const std::string& callerId) const;
	bool CanRedo (const std::string& callerId) const;

	void Undo (const std::string& callerId);
	void Redo (const std::string& callerId);
	void Drop (const std::string& callerId);

	void UndoAll (const std::string& callerId);
	void RedoAll (const std::string& callerId);
	void DropAll (const std::string& callerId);

private:

	////////////////////////////////////////////////////////////////////

	struct UndoEntry {
		UndoEntry (const Message& msg) : msgs(1, msg) {}
		UndoEntry (void) : msgs(0) {}

		MessageVec msgs;
	};

	//******************************************************************

	class UndoList {
	public:
		UndoList (void);
		UndoList (const UndoList& other);

		void Append (const UndoEntry& entry);
		void Append (const Message& msg);
		void Change (const UndoEntry& entry);
		void Change (const Message& msg);

		bool CanUndo (void) const;
		bool CanRedo (void) const;

		void Undo (const std::string& callerId);
		void Redo (const std::string& callerId);
		void Drop (void);

		void UndoAll (const std::string& callerId);
		void RedoAll (const std::string& callerId);
		void DropAll (void);

	private:
		typedef std::list<UndoEntry> List;
	
		static bool execute (const std::string& callerId, UndoEntry& entry);

		List			m_undoList;
		List::iterator	m_undoIter;

		static size_t	s_undoEntryPos;
		static bool		s_changedUndoOrRedo;
	};

	////////////////////////////////////////////////////////////////////

	typedef std::list<const std::string>			GlobalUndoList;
	typedef std::map<const std::string, UndoList>	CIDUndoMap;

	//******************************************************************

	UndoManager (void) : m_dispatchDepth(0), m_storedUndoActionDepth(0),
		m_batchUndoRedoDepth(0), m_duringUndoRedo(false) {}

	//******************************************************************

	void append (const std::string& callerId, const UndoEntry& entry);
	void append (const std::string& callerId, const Message& msg);
	void change (const UndoEntry& entry);
	void change (const Message& msg);

	void removeLast (const std::string& callerId);
	void removeAll (const std::string& callerId);

	//******************************************************************

	void undo (const std::string& callerId);
	void redo (const std::string& callerId);
	void drop (const std::string& callerId);

	void undoAll (const std::string& callerId);
	void redoAll (const std::string& callerId);
	void dropAll (const std::string& callerId);

	//******************************************************************

	void preDispatchMessage (
			const std::string&			callerId,
			const Message&				msg,
			bool						broadCast,
			const ComponentFuncEntry&	func
		);
	void postDispatchMessage (
			const std::string&			callerId,
			const Message&				msg,
			bool						broadCast,
			const ComponentFuncEntry&	func,
			bool						result
		);
	void registerUndoMessage (const Message& msg);

	//******************************************************************

	GlobalUndoList				m_undoList;
	GlobalUndoList::iterator	m_undoIter;
	CIDUndoMap					m_undoMap;

	uint						m_dispatchDepth;
	uint						m_storedUndoActionDepth;
	uint						m_batchUndoRedoDepth;

	bool						m_duringUndoRedo;
	std::string					m_initialCallerId;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_UNDOMANAGER_H
