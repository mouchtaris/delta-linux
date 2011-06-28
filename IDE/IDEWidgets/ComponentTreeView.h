/**
 *	ComponentTreeView.h
 *
 *	-- IDEWidgets --
 *
 *	ComponentTreeView is a wrapper of a TreeCtrl that
 *	holds components. Every node in the tree is a
 *	component and has to comply to a standard API in
 *	order to be presented accordingly.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef COMPONENT_TREE_VIEW_H_
#define COMPONENT_TREE_VIEW_H_

#include "Common.h"
#include "IDECommon.h"

#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentUserCommand.h"
#include "ComponentMenu.h"

#include <boost/function.hpp>

#include <wx/treebase.h>
#include <wx/treectrl.h>

namespace ide
{
	class ImageList;

	//----------------------------
	//-- class ComponentTreeView

	class _IDE_API ComponentTreeView :
		public wxTreeCtrl
	{
		DECLARE_DYNAMIC_CLASS(ComponentTreeView);
		//This is just needed for declaring the class dynamic
		//(so as to have the OnCompareItems). Never to be called.
		ComponentTreeView(void) { assert(false); }
	public:
		typedef boost::function<int (const Handle&, const Handle&)> Comparator;
		///--- type definitions

		///--- constructors / destructor
		ComponentTreeView(wxWindow* parent, wxWindowID id,
			const wxPoint& pos=wxDefaultPosition,
			const wxSize& size=wxDefaultSize,
			long style=wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS);
		~ComponentTreeView(void) {}

		///--- public API
		bool InsertItem(const Handle& parent, const Handle& item);
		bool RemoveItem(const Handle& item);

		const Handle& GetRoot(void) const;
		const Handle& GetComponent(wxTreeItemId item) const;
		const Handle& GetParent(wxTreeItemId item) const;

		wxTreeItemId GetNode(const Handle& comp) const;

		bool AddImage(const String& id);

		void SetComparator(const Comparator& comparator);

		virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onBeginLabelEdit(wxTreeEvent& event);
		void onEndLabelEdit(wxTreeEvent& event);
		void onContextSelect(wxTreeEvent& event);
		void onKeyDown(wxTreeEvent& event);
		void onCtxMenuSelect(wxCommandEvent& event);
		void onMouseButtonDown(wxMouseEvent& event);

		///--- private members
		ImageList* images;
		ComponentMenu* popup;
		Comparator comparator;
	};
}

#endif	//COMPONENT_TREE_VIEW
