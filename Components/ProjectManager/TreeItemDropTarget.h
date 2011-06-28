/**
 *	TreeItemDropTarget.h
 *
 *	-- IDE Drop target implementation for the Project Manager tree --
 *
 *	Support for dropping items in the Project Manager treeview.
 *	Objects accepted correspond to TreeItemDataObjects.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	April 2009
 */
#ifndef IDE_TREE_ITEM_DROP_TARGET_H_
#define IDE_TREE_ITEM_DROP_TARGET_H_

#include "Common.h"
#include "ComponentHandle.h"

#include <wx/treectrl.h>
#include <wx/dnd.h>

namespace ide
{
	class Component;
	class ComponentTreeView;

	class TreeItemDropTarget : public wxDropTarget 
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		TreeItemDropTarget(ComponentTreeView* treeview);
		~TreeItemDropTarget();

		///--- public API
	private:
		///--- private API
		void UnHighlightCurrent (void);
		bool AddItemFromURI (const Handle& parent, const StdStringList& parentTypes, const String& uri);

		virtual wxDragResult	OnEnter		(wxCoord x, wxCoord y, wxDragResult def);
		virtual wxDragResult	OnDragOver	(wxCoord x, wxCoord y, wxDragResult def);
		virtual void			OnLeave		(void);
		virtual bool			OnDrop		(wxCoord x, wxCoord y);
		virtual wxDragResult	OnData		(wxCoord x, wxCoord y, wxDragResult def);

		///--- private members
		wxTreeItemId		highlighted;
		ComponentTreeView*	treeview;

		static const std::string s_classId;
	};
}

#endif	//IDE_TREE_ITEM_DROP_TARGET_H_