/**
 *	DebugInfo.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	May 2010
 */

#include <algorithm>
#include "DebugInfo.h"
#include "StringUtils.h"

namespace iviews {

namespace dinfo {

//-----------------------------------------------------------------------

#define DO_UNDO_OR_REDO(CAN_DO_ACTION, ACTION, VERTEX)									\
	if (!undoRedo.CAN_DO_ACTION())														\
		return false;																	\
																						\
	UndoRedoDataList data = undoRedo.ACTION();											\
	for(SlotDebugInfoList::iterator slot = slots.begin(); slot != slots.end(); ++slot)	\
		for(UndoRedoDataList::iterator i = data.begin(); i != data.end(); ++i)			\
			if (*i == *slot)															\
				i->isKey	? slot->SetKeyTargetVertex(VERTEX)							\
							: slot->SetContentTargetVertex(VERTEX);						\
	return true


//-----------------------------------------------------------------------

DebugInfo::DebugInfo(const DebugInfo & dInfo)
:	nextStringfieldId(dInfo.nextStringfieldId),
	absolutRef(dInfo.absolutRef),
	slots(dInfo.slots),
	undoRedo(dInfo.undoRedo)
{}

//-----------------------------------------------------------------------

DebugInfo::DebugInfo(const std::string & absolutRef_) 
:	nextStringfieldId(0),
	absolutRef(absolutRef_),
	slots(),
	undoRedo()
{}

//-----------------------------------------------------------------------

bool DebugInfo::Undo(void) 
	{ DO_UNDO_OR_REDO(CanUndo, Undo, i->vertex); }

//-----------------------------------------------------------------------

bool DebugInfo::Redo(void) 
	{ DO_UNDO_OR_REDO(CanRedo, Redo, ((graphs::Vertex *)0)); }

//-----------------------------------------------------------------------

void DebugInfo::AddSlot(const SlotDebugInfo & di) {
	slots.push_back(SlotDebugInfo(di));
}

//-----------------------------------------------------------------------

void DebugInfo::AddSlot(
		const id_t				id,
		const bool				isKeyAggregate,
		const std::string	&	keyString,
		const std::string	&	keyTypeInfoString,
		const graphs::Vertex*	keyTargetVertex,
		const bool				isContentAggergate,
		const std::string	&	contentString,
		const graphs::Vertex*	contentTargetVertex
	) {
	slots.push_back(
		SlotDebugInfo(	
			id,
			absolutRef + utility::string_cast(nextStringfieldId++),
			isKeyAggregate,
			keyString,
			keyTypeInfoString,
			keyTargetVertex,
			isContentAggergate,
			contentString,
			contentTargetVertex
		)
	);
}

//-----------------------------------------------------------------------

DebugInfo::SlotDebugInfoList & DebugInfo::GetAllSlots(void) 
	{ return slots; }

//-----------------------------------------------------------------------

const DebugInfo::SlotDebugInfoList & DebugInfo::GetAllSlots(void) const 
	{ return slots; }

//-----------------------------------------------------------------------

const std::string & DebugInfo::GetAbsolutRef(void) const 
	{ return absolutRef; }

//-----------------------------------------------------------------------

void DebugInfo::RemoveVertexFromSlotsList(graphs::Vertex * vertex){
	undoRedo.NewUndoAction();
	
	for(SlotDebugInfoList::iterator i = slots.begin(); i != slots.end(); ++i){
		if (i->GetKeyTargetVertex() == vertex) {
			undoRedo.AddInUndoStack(i->Id(), true, i->GetKeyTargetVertex());
			i->SetKeyTargetVertex(static_cast<graphs::Vertex *>(0));
		}

		if (i->GetContentTargetVertex() == vertex)  {
			undoRedo.AddInUndoStack(i->Id(), false, i->GetContentTargetVertex());
			i->SetContentTargetVertex(static_cast<graphs::Vertex *>(0));
		}
	}
}

////////////////////////////////////////////
//	implementation of iner class

///--------------------------------- UndoRedoHelperForDebugInfo
//-----------------------------------------------------------------------

DebugInfo::UndoRedoHelperForDebugInfo::UndoRedoHelperForDebugInfo() 
	:	UndoRedoHelper() {}

//-----------------------------------------------------------------------

DebugInfo::UndoRedoHelperForDebugInfo::UndoRedoHelperForDebugInfo(const UndoRedoHelperForDebugInfo & o) {
	undoStack = o.undoStack;
	redoStack = o.redoStack;
}

//-----------------------------------------------------------------------

void DebugInfo::UndoRedoHelperForDebugInfo::AddInUndoStack(id_t				slotId,
														   const bool		isKey,
														   graphs::Vertex * vertex) 
{
	assert(!undoStack.empty());
	undoStack.back().push_back(UndoRedoData(slotId, isKey, vertex)); 
}



}	//namespace dinfo

}	//namespace iviews