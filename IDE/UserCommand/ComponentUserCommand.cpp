/**
 *	ComponentUserCommand.cpp
 *
 *	-- IDE Component Data Types --
 *
 *	Component user command is the basic command
 *	type that components expose to the user and
 *	are used for menu and toolbar options.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#include "ComponentUserCommand.h"
#include "ComponentMenu.h"
#include "ComponentToolBar.h"
#include "BitmapRegistry.h"
#include "ComponentRegistry.h"

#include "StringUtils.h"
#include "Algorithms.h"
#include "Adaptors.h"
#include "Predicates.h"
#include "Streams.h"

#include "MenuParser.h"

#include <wx/menu.h>
#include <wx/toolbar.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <functional>

namespace ide
{
	//-------------------------------------------------------//
	//---- class UserCommand --------------------------------//

	void UserCommand::ChangeOwner(const std::string& toClassId, const std::string& fromClassId)
	{
		if (action.GetClassId() == fromClassId)
			action.SetClassId(toClassId);

		std::for_each(
			menu.begin(), menu.end(),
			util::project2nd<Menu::value_type>(
				util::project2nd<Option>(
					boost::bind(&UserCommand::ChangeOwner, _1, toClassId, fromClassId)
				)
			)
		);
	}

	//-----------------------------------------------------------------------

	bool UserCommand::InsertUserCommand(const String& path, const UserCommand& command, int priority)
	{
		if (!path)
			return false;

		bool status = parse(MenuParser(command, this), path);
		if (status)
			NormalizeSeparators(menu);
		return status;
	}

	//-----------------------------------------------------------------------

	bool UserCommand::RemoveUserCommand(const String& path)
	{
		if (path.empty())
			return true;

		StringList tokens;
		util::stringtokenizer(tokens, path, _T("/"));

		String last;
		if (!tokens.empty())
		{
			last = tokens.back();
			tokens.pop_back();
		}

		String parent_path =
			util::reduce(
				tokens.begin(), tokens.end(),
				std::plus<String>(),
				boost::bind(std::plus<String>(), String(_T("/")), _1)
			);
		UserCommand& parent = GetUserCommand(parent_path);

		if (parent.IsValid())
		{
			Menu::iterator it = parent.GetCommand(last);
			if (it != parent.menu.end())
				parent.menu.erase(it);
			if (parent.menu.size() == 0)
				RemoveUserCommand(parent_path);
			NormalizeSeparators(menu);
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool UserCommand::EnableUserCommand(const String& path)
	{
		if (path.empty())
			return false;

		StringList tokens;
		util::stringtokenizer(tokens, path, _T("/"));

		String last;
		if (!tokens.empty())
		{
			last = tokens.back();
			tokens.pop_back();
		}

		String parent_path =
			util::reduce(
				tokens.begin(), tokens.end(),
				std::plus<String>(),
				boost::bind(std::plus<String>(), String(_T("/")), _1)
			);
		UserCommand& parent = GetUserCommand(parent_path);

		if (parent.IsValid())
		{
			parent.SetEnabled();
			Menu::iterator it = parent.GetCommand(last);
			if (it != parent.menu.end())
				it->second.second.SetEnabled();
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool UserCommand::DisableUserCommand(const String& path)
	{
		if (path.empty())
			return false;

		StringList tokens;
		util::stringtokenizer(tokens, path, _T("/"));

		String last;
		if (!tokens.empty())
		{
			last = tokens.back();
			tokens.pop_back();
		}

		String parent_path =
			util::reduce(
				tokens.begin(), tokens.end(),
				std::plus<String>(),
				boost::bind(std::plus<String>(), String(_T("/")), _1)
			);
		UserCommand& parent = GetUserCommand(parent_path);

		if (parent.IsValid())
		{
			Menu::iterator it = parent.GetCommand(last);
			if (it != parent.menu.end())
				it->second.second.SetEnabled(false);
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool UserCommand::HasUserCommand(const String& path) const
	{
		return GetUserCommand(path).IsValid();
	}

	//-----------------------------------------------------------------------

	UserCommand UserCommand::Merge(const UserCommand& left, const UserCommand& right)
	{
		UserCommand result = left;
		return result.Merge(right);
	}

	//-----------------------------------------------------------------------

	UserCommand& UserCommand::Merge(const UserCommand& right)
	{
		Menu::const_iterator rightIt = right.menu.begin();
		while (rightIt != right.menu.end())
		{
			Menu::iterator leftIt =
				std::find_if(
					this->menu.begin(),
					this->menu.end(),
					util::project2nd<Menu::value_type>(
						util::project1st<Option>(
							boost::bind(std::equal_to<String>(), _1, rightIt->second.first)
						)
					)
				);

			if (leftIt == this->menu.end())
				this->menu.insert(*rightIt);
			else
				leftIt->second.second += rightIt->second.second;
			++rightIt;
		}
		
		NormalizeSeparators(menu);
		return *this;
	}

	//-----------------------------------------------------------------------

	UserCommand UserCommand::Subtract(const UserCommand& left, const UserCommand& right)
	{
		UserCommand result = left;
		return result.Subtract(right);
	}

	//-----------------------------------------------------------------------

	UserCommand& UserCommand::Subtract(const UserCommand& right)
	{
		BOOST_FOREACH(const Menu::value_type& value, right.menu)
		{
			const Option& option = value.second;
			if (option.second.IsSubMenu()) {
				UserCommand& left = GetUserCommand(option.first);
				left.Subtract(option.second);
				if (left.menu.size() == 0)
					RemoveUserCommand(option.first);
			}
			else if (!GetUserCommand(option.first).IsSubMenu())	//Do not remove a submenu due to a single item remove
				RemoveUserCommand(option.first);
		}

		return *this;
	}

	//-----------------------------------------------------------------------

	UserCommand& UserCommand::SubtractNonStatic(const UserCommand& right)
	{
		BOOST_FOREACH(const Menu::value_type& value, right.menu)
		{
			const Option& option = value.second;
			if (option.second.IsSubMenu()) {
				UserCommand& left = GetUserCommand(option.first);
				left.SubtractNonStatic(option.second);
				if (left.menu.size() == 0)
					RemoveUserCommand(option.first);
			}
			else
			{
				ComponentEntry& compEntry =
					ComponentRegistry::Instance().GetComponentEntry(option.second.GetAction().GetClassId());
				const ComponentFuncEntry& funcEntry =
					compEntry.GetFuncEntry(option.second.GetAction().GetFunction());
				if (funcEntry.IsMemberFunc() && !GetUserCommand(option.first).IsSubMenu())
					RemoveUserCommand(option.first);
			}
		}

		return *this;
	}

	//-----------------------------------------------------------------------

	const UserCommand& UserCommand::GetUserCommand(const String& path) const
	{
		static UserCommand null_UserCommand(UCT_INVALID);

		StringList tokens;
		util::stringtokenizer(tokens, path, _T("/"));

		if (tokens.empty())
			return *this;

		Menu::const_iterator it = GetCommand(tokens.front());
		if (it == menu.end())
			return null_UserCommand;

		tokens.pop_front();
		return
			it->second.second.GetUserCommand(
				util::reduce(
					tokens.begin(), tokens.end(),
					std::plus<String>(),
					boost::bind(std::plus<String>(), String(_T("/")), _1)
				)
			);
	}

	//-----------------------------------------------------------------------

	UserCommand& UserCommand::GetUserCommand(const String& path)
	{
		static UserCommand null_UserCommand(UCT_INVALID);

		StringList tokens;
		util::stringtokenizer(tokens, path, _T("/"));

		if (tokens.empty())
			return *this;

		Menu::iterator it = GetCommand(tokens.front());
		if (it == menu.end())
			return null_UserCommand;

		tokens.pop_front();
		return
			it->second.second.GetUserCommand(
				util::reduce(
					tokens.begin(), tokens.end(),
					std::plus<String>(),
					boost::bind(std::plus<String>(), String(_T("/")), _1)
				)
			);
	}

	//-----------------------------------------------------------------------

	bool UserCommand::AppendCommandMapping(const Option& mapping)
	{
		return InsertUserCommand(mapping.first, mapping.second);
	}

	//-----------------------------------------------------------------------

	void UserCommand::NormalizeSeparators(Menu& menu)
	{
		Menu::iterator prev = menu.end();
		for (Menu::iterator it = menu.begin(); it != menu.end(); /*empty*/)
		{
			if (it == menu.begin())
			{
				if (it->second.second.IsSeparator())
				{
					it = menu.erase(it);
					continue;
				}
				else
					NormalizeSeparators(it->second.second.menu);
			}
			else
			{
				assert(prev != menu.end());
				if (it->second.second.IsSeparator() && prev->second.second.IsSeparator())
					it = menu.erase(prev);
				else if (!it->second.second.IsSeparator())
					NormalizeSeparators(it->second.second.menu);
			}
			prev = it++;
		}
		if (prev != menu.end() && prev->second.second.IsSeparator())
			menu.erase(prev);
	}

	//-----------------------------------------------------------------------

	UserCommand::Menu::const_iterator UserCommand::GetCommand(const String& title) const
	{
		return std::find_if(
			menu.begin(),
			menu.end(),
			util::project2nd<Menu::value_type>(
				util::project1st<Option>(
					boost::bind(
						std::equal_to<String>(),
						_1, title
					)
				)
			)
		);
	}

	//-----------------------------------------------------------------------

	UserCommand::Menu::iterator UserCommand::GetCommand(const String& title)
	{
		return std::find_if(
			menu.begin(),
			menu.end(),
			util::project2nd<Menu::value_type>(
				util::project1st<Option>(
					boost::bind(
						std::equal_to<String>(),
						_1, title
					)
				)
			)
		);
	}

	//-----------------------------------------------------------------------

	ComponentMenu* UserCommand::GenerateMenu(ComponentMenu* menu) const
	{
		return MenuGenerator(MT_CTX).Generate(menu, *this);
	}

	//-----------------------------------------------------------------------

	ComponentMenuBar* UserCommand::GenerateMenuBar(ComponentMenuBar* menubar) const
	{
		return MenuBarGenerator().Generate(menubar, *this);
	}

	//-----------------------------------------------------------------------

	ComponentToolBar* UserCommand::GenerateToolBar(ComponentToolBar* toolbar) const
	{
		return ToolBarGenerator().Generate(toolbar, *this);
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class MenuGenerator ------------------------------//

	ComponentMenu* MenuGenerator::Generate(ComponentMenu* menu, const UserCommand& command,
		CallbackVecPtr actions) const
	{
		menu->SetActions(actions);
		std::for_each(
			command.menu.begin(),
			command.menu.end(),
			util::project2nd<UserCommand::Menu::value_type>(
				boost::bind(&MenuGenerator::CreateMenuItem, this, menu, _1)
			)
		);
		return menu;
	}

	//-----------------------------------------------------------------------

	void MenuGenerator::CreateMenuItem(ComponentMenu* menu,
		const UserCommand::Option& option) const
	{
		if (!(option.second.menuType & menuType))
			return;

		wxMenuItem* item = (wxMenuItem*) 0;
		switch (option.second.type)
		{
			case UserCommand::UCT_NORMAL: {
				//-- Append a Normal option
				item = new wxMenuItem(menu, (int)menu->actions->size(), option.first);

				//-- Attach an optional image
				if (!option.second.image.empty()) {
					wxImage image = BitmapRegistry::Instance().Get(option.second.image);
					if (!image.Ok())
						image = BitmapRegistry::Instance().Get(_T("Cog"));
					item->SetBitmap(image);
				}
				menu->Append(item, option.second.GetAction());
				break;
			}

			case UserCommand::UCT_SEPARATOR: {
				//-- Append a Separator
				menu->AppendSeparator();
				break;
			}

			case UserCommand::UCT_CHECK_ITEM: {
				//-- Append a Check option
				item = new wxMenuItem(menu, (int)menu->actions->size(), option.first,
					String(), wxITEM_CHECK);

				//-- Attach an optional image
				if (!option.second.image.empty()) {
					wxImage image = BitmapRegistry::Instance().Get(option.second.image);
					if (!image.Ok())
						image = BitmapRegistry::Instance().Get(_T("Cog"));
					item->SetBitmap(image);
				}
				menu->Append(item, option.second.GetAction());
				break;
			}

			case UserCommand::UCT_SUB_MENU: {
				//-- Append a sub-menu
				item = menu->AppendSubMenu(Generate(new ComponentMenu, option.second, menu->actions), option.first);
				break;
			}

			default:
				DBGOUT << "UserCommand type not compatible with context menus" << DBGENDL;
				break;
		}
		if (item)
			item->Enable(option.second.IsEnabled());
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class MenuBarGenerator ---------------------------//

	ComponentMenuBar* MenuBarGenerator::Generate(ComponentMenuBar* menubar, const UserCommand& command) const
	{
		UserCommand::CallbackVecPtr actions(new UserCommand::CallbackVec);
		ComponentMenuList menus;
		StringList titles;

		BOOST_FOREACH(const UserCommand::Menu::value_type& menu, command.menu)
		{
			if (!(menu.second.second.GetFlags() & MT_MAIN) ||	//skip non main menu enties
				menu.second.second.IsSeparator()				//skip top level separators (used in CTX entries)
			)
				continue;

			menus.push_back(
				MenuGenerator(MT_MAIN).Generate(
					new ComponentMenu, menu.second.second, actions
				)
			);

			titles.push_back(menu.second.first);
		}

		menubar->SetMenus(menus, titles);
		menubar->SetActions(actions);
		return menubar;
	}

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class ToolBarGenerator ---------------------------//

	ComponentToolBar* ToolBarGenerator::Generate(ComponentToolBar* toolbar, const UserCommand& command) const
	{
		std::for_each(
			command.menu.begin(),
			command.menu.end(),
			util::project2nd<UserCommand::Menu::value_type>(
				boost::bind(&ToolBarGenerator::CreateToolItem, this, toolbar, _1)
			)
		);
		return toolbar;
	}

	//-----------------------------------------------------------------------

	void ToolBarGenerator::CreateToolItem(ComponentToolBar* toolbar,
		const UserCommand::Option& option) const
	{
		if (!(option.second.menuType & MT_TOOLBAR))
			return;

		switch (option.second.type)
		{
		case UserCommand::UCT_NORMAL: {
			const UserCommand::Callback& action = option.second.GetAction();
			const String longHelpStr = ComponentRegistry::Instance().
				GetComponentEntry(action.GetClassId()).GetFuncEntry(action.GetFunction()).
				GetDocumentation();

			wxImage image = BitmapRegistry::Instance().Get((!option.second.image.empty() ? option.second.image : _T("Cog")));
			if (!image.Ok())
				image = BitmapRegistry::Instance().Get(_T("Cog"));

			wxToolBarToolBase* tool = toolbar->AddTool(
				option.first, image, option.second.GetAction(), longHelpStr
			);
			break;
		}

		case UserCommand::UCT_SEPARATOR: {
			toolbar->AddSeparator();
			break;
		}

		case UserCommand::UCT_CHECK_ITEM: {
			const UserCommand::Callback& action = option.second.GetAction();
			const String longHelpStr = ComponentRegistry::Instance().
				GetComponentEntry(action.GetClassId()).GetFuncEntry(action.GetFunction()).
				GetDocumentation();

			wxToolBarToolBase* tool = toolbar->AddCheckTool(
				option.first,
				BitmapRegistry::Instance().Get((!option.second.image.empty() ? option.second.image : _T("Cog"))),
				option.second.GetAction(), longHelpStr
			);
			break;
		}

		case UserCommand::UCT_SUB_MENU: {
			//-- Generate sub-options
			std::for_each(
				option.second.menu.begin(),
				option.second.menu.end(),
				util::project2nd<UserCommand::Menu::value_type>(
					boost::bind(&ToolBarGenerator::CreateToolItem, this, toolbar, _1)
				)
			);
			break;
		}

		default:
			DBGOUT << "UserCommand type not compatible with toolbar" << DBGENDL;
			break;
		};
	}

	//-----------------------------------------------------------------------
}
