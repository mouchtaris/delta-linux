/**
 *	ComponentMenu.h
 *
 *	-- IDEWidgets --
 *
 *	Component menu is a wrapper of the wxMenu
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef COMPONENT_MENU_H_
#define COMPONENT_MENU_H_

#include "Common.h"
#include "IDECommon.h"

#include "ComponentUserCommand.h"
#include "UndefineAllWin32Shit.h"

#include <wx/menu.h>

#include <functional>

namespace ide
{
	class ComponentMenu;
	typedef std::list<ComponentMenu*> ComponentMenuList;

	//----------------------------
	//-- class ComponentMenuBar

	class _IDE_API ComponentMenuBar :
		public wxMenuBar
	{
	public:
		///--- type definitions
		typedef UserCommand::CallbackVecPtr CallbackVecPtr;
		typedef UserCommand::Callback		Callback;

		///--- constructors / destructor
		ComponentMenuBar(void);
		virtual ~ComponentMenuBar(void);

		///--- public API
		void SetMenus(const ComponentMenuList& menus, const StringList& titles);
		void SetActions(CallbackVecPtr actions)		{ this->actions = actions; }

		const Callback& GetAction(uint id) const	{ return actions->at(id); }

	private:
		///--- private API

		///--- private members
		CallbackVecPtr actions;
	};


	//----------------------------
	//-- class ComponentMenu

	class _IDE_API ComponentMenu :
		public wxMenu
	{
	public:
		///--- type definitions
		typedef UserCommand::Callback		Callback;
		typedef UserCommand::CallbackVec	CallbackVec;
		typedef UserCommand::CallbackVecPtr CallbackVecPtr;

		///--- constructors / destructor
		ComponentMenu(void) {}
		virtual ~ComponentMenu(void) {}

		///--- public API
		wxMenuItem* Append(wxMenuItem* item, const Callback& action);
		wxMenuItem* Append(const String& title, const Callback& action);
		wxMenuItem* AppendCheckItem(const String& title, const Callback& action);

		void SetActions(CallbackVecPtr actions)
			{ this->actions = (!actions) ? CallbackVecPtr(new CallbackVec) : actions; }

		const Callback& GetAction(uint id) const	{ return actions->at(id); }

	private:
		friend class MenuGenerator;

		///--- private API

		///--- private members
		CallbackVecPtr actions;
	};
}

#endif	//COMPONENT_MENU
