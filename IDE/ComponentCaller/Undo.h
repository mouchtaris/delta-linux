/**
 *	Undo.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_UNDO_H
#define IDE_UNDO_H

#include "Common.h"
#include "IDECore.h"
#include "Message.h"
#include "MessageUtils.h"
#include "ExportedFunctionStub.h"
#include "ComponentUndoRegistrar.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;

////////////////////////////////////////////////////////////////////////

template<class Signature>
class Undo :
	public ComponentUndoRegistrar,
	public ExportedFunctionStub<false, Message, Undo<Signature>, Signature> {
public:
	Undo (void) {}
	Undo (const Handle& handle, const std::string& function) :
		ComponentUndoRegistrar(handle, function) {}

	Undo (const std::string& classId, const std::string& function) :
		ComponentUndoRegistrar(classId, function) {}
};

////////////////////////////////////////////////////////////////////////

template<class Signature>
class MUndo :
	public ComponentUndoRegistrar,
	public ExportedFunctionStub<true, MessageVec, MUndo<Signature>, Signature> {
public:
	MUndo (void) {}
	MUndo (const std::string& classId, const std::string& function) :
		ComponentUndoRegistrar(classId, function) {}
};

////////////////////////////////////////////////////////////////////////

#define SCOPED_BATCH_UNDO(callerId) \
	ide::ComponentUndoRegistrar::ScopedBatchAction _scopedBatch(callerId)

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_UNDO_H
