/*	VertexToDebugInfoMapping.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include "VertexToDebugInfoMapping.h"

namespace iviews {
namespace graphs {

//-----------------------------------------------------------------------

bool VertexToDebugInfoMapping::Undo(void) { 
	if (!undoRedo.CanUndo())
		return false; 

	VertexToDebugInfoList undoList = undoRedo.Undo();
	for (VertexToDebugInfoList::iterator i = undoList.begin(); i != undoList.end(); ++i)
		insert(*i);

	//undo all the Dubug Info
	for (iterator i = begin(); i != end(); ++i )
		i->second.Undo();

	return true;
}

//-----------------------------------------------------------------------

bool VertexToDebugInfoMapping::Redo(void) { 
	if (!undoRedo.CanRedo())
		return false;

	VertexToDebugInfoList redoList = undoRedo.Redo();
	for (VertexToDebugInfoList::iterator i = redoList.begin(); i != redoList.end(); ++i) {
		iterator elementToDelete = find(i->first);
		assert(elementToDelete != end());

		map::erase(elementToDelete);
	}
	
	//redo all the Dubug Info
	for (iterator i = begin(); i != end(); ++i )
		i->second.Redo();

	return true; 
}

//-----------------------------------------------------------------------

void VertexToDebugInfoMapping::NewUndoAction(void) 
	{ undoRedo.NewUndoAction(); }

//-----------------------------------------------------------------------

void VertexToDebugInfoMapping::RemoveVertexFromSlots(const Vertex & v) {
	for(iterator i = begin(); i != end(); ++i) 
		i->second.RemoveVertexFromSlotsList( const_cast<Vertex *>(&v) );
}

//-----------------------------------------------------------------------

void VertexToDebugInfoMapping::erase (iterator position) {
	assert(position != end());
	undoRedo.AddInUndoStack(position->first, position->second);
	map::erase(position);
}

//-----------------------------------------------------------------------

VertexToDebugInfoMapping::UndoRedoHelperForVertexToDInfo::UndoRedoHelperForVertexToDInfo() : UndoRedoHelper() {}

//-----------------------------------------------------------------------

void VertexToDebugInfoMapping::UndoRedoHelperForVertexToDInfo::AddInUndoStack(Vertex * v, DebugInfo & dinfo) { 
	assert(!undoStack.empty());
	undoStack.back().push_back(std::make_pair(v, dinfo)); 
}


}	//graphs
}	//iviews