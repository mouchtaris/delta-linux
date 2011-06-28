/**
 *	ComponentMenu.cpp
 *
 *	-- IDEWidgets --
 *
 *	Component menu is a wrapper of the wxMenu
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#include "ComponentMenu.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class ComponentMenuBar ---------------------------//

	ComponentMenuBar::ComponentMenuBar(void)
	{
	}

	//-----------------------------------------------------------------------

	ComponentMenuBar::~ComponentMenuBar(void)
	{
	}

	//-----------------------------------------------------------------------

	void ComponentMenuBar::SetMenus(const ComponentMenuList& menus, const StringList& titles)
	{
		///--- Functor MenuAppender
		struct MenuAppender :
			std::binary_function<ComponentMenu*, const String&, int>
		{
			MenuAppender(ComponentMenuBar* menubar)
				: menubar(menubar) {}

			result_type operator ()(first_argument_type menu, second_argument_type title) {
				menubar->Append(menu, title);
				return 0;
			}

		private:
			ComponentMenuBar* menubar;
		};
		///------------------------------------

		std::list<int> useless;
		std::transform(
			menus.begin(), menus.end(), titles.begin(), std::back_inserter(useless),
			MenuAppender(this)
		);
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class ComponentMenu ------------------------------//

	wxMenuItem* ComponentMenu::Append(wxMenuItem* item, const Callback& action)
	{
		actions->push_back(action);
		return wxMenu::Append(item);
	}

	//-----------------------------------------------------------------------

	wxMenuItem* ComponentMenu::Append(const String& title, const Callback& action)
	{
		actions->push_back(action);
		return wxMenu::Append((int)actions->size() -1, title);
	}

	//-----------------------------------------------------------------------

	wxMenuItem* ComponentMenu::AppendCheckItem(const String& title, const Callback& action)
	{
		actions->push_back(action);
		return wxMenu::AppendCheckItem((int)actions->size() -1, title);
	}

	//-----------------------------------------------------------------------
}
