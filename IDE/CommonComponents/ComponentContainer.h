/**
 *	ComponentContainer.h
 *
 *	-- IDE Shell Component --
 *
 *	A NotebookCnt wrapper for containing components
 *	instead of wxWindows.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
*/
#ifndef SHELL_COMPONENT_CONTAINER_H_
#define SHELL_COMPONENT_CONTAINER_H_

#include "Common.h"

#include "NotebookCnt.h"

#include "Component.h"
#include "ComponentHandle.h"

#define SHELL_COMPONENT_CNT_FLAGS	\
	wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_TOP |	\
	wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE

namespace ide
{
	//----------------------------
	//-- class ComponentContainer

	class ComponentContainer :
		public NotebookCnt
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		ComponentContainer(
			wxWindow*		parent,
			long			flags = SHELL_COMPONENT_CNT_FLAGS,
			const wxSize&	size = wxDefaultSize
		)
			: NotebookCnt(parent, flags, size) {}
		~ComponentContainer(void) {}

		///--- public API
		bool AddComponent(Component* comp, bool select=false, const wxBitmap& bitmap=wxNullBitmap);
		bool RemoveComponent(const Handle& handle);
		bool Contains(const Handle& handle);
		bool Rename(const Handle& handle, const String& title);
		void Clear(void);

		bool FocusComponent(const Handle& handle);
		bool RemoveComponent(uint32 index);
		Component* GetComponent(uint32 index);

		HandleVec& GetComponents(void)	{ return children; }

	private:
		///--- private API

		///--- private members
		HandleVec children;
	};
}

#endif	//SHELL_COMPONENT_CONTAINER
