/**
 *	ComponentContainer.cpp
 *
 *	-- IDE Shell Component --
 *
 *	A NotebookCnt wrapper for containing components
 *	instead of wxWindows.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
*/
#include "ComponentContainer.h"

#include "Algorithms.h"
#include "CommonGUIUtils.h"

#include "ComponentFactory.h"

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- class ComponentContainer -------------------------//

	bool ComponentContainer::AddComponent(Component* comp, bool select, const wxBitmap& bitmap)
	{
		if (!AddPage(comp->GetWindow(), comp->GetName(), select, bitmap))
			return false;
		children.push_back(comp);
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentContainer::RemoveComponent(const Handle& handle)
	{
		HandleVec::iterator it = std::find(children.begin(), children.end(), handle);
		if (it == children.end())
			return false;

		size_t index = std::distance(children.begin(), it);
		RemovePage(index);
		children.erase(it);
		if (index > 0)
			--index;
		if (GetPageCount() > 0)
			SetSelection(index);
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentContainer::Contains(const Handle& handle)
	{
		return util::exists(children.begin(), children.end(), handle);
	}

	//-----------------------------------------------------------------------

	bool ComponentContainer::Rename(const Handle& handle, const String& title)
	{
		HandleVec::iterator it = std::find(children.begin(), children.end(), handle);
		if (it == children.end())
			return false;

		size_t index = std::distance(children.begin(), it);
		SetPageText(index, title);
		return true;
	}

	//-----------------------------------------------------------------------

	void ComponentContainer::Clear(void)
	{
		for (uint i=0; i < children.size(); ++i)
			RemovePage(i);

		while (!children.empty())
			children.front()->Destroy();	///< when a component is destroyed it will automatically remove itself from the pane
	}

	//-----------------------------------------------------------------------

	bool ComponentContainer::FocusComponent(const Handle& handle)
	{
		HandleVec::iterator it =
			std::find(children.begin(), children.end(), handle);
		if (it == children.end())
			return false;

		SetSelection(std::distance(children.begin(), it));
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentContainer::RemoveComponent(uint32 index)
	{
		assert(index < children.size());

		if (!RemovePage(index))
			return false;
		children.erase(children.begin() + index);
		return true;
	}

	//-----------------------------------------------------------------------

	Component* ComponentContainer::GetComponent(uint32 index)
	{
		assert(index < children.size());

		return children[index].Resolve();
	}

	//-----------------------------------------------------------------------
}
