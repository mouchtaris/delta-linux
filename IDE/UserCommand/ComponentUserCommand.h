/**
 *	ComponentUserCommand.h
 *
 *	-- IDE Component User Command --
 *
 *	Component user command is the basic command
 *	type that components expose to the user and
 *	are used for menu and toolbar options.
 *
 *	Below is the grammar for InsertUserCommand
 *	path variable:
 *
 *	start
 *			::=		content_list
 *				;
 *	content_list
 *			::=		SLASH content content_list
 *				|	SLASH content
 *				;
 *	content
 *			::=		priority option
 *				|	option
 *				;
 *	priority
 *			::=		L_CURLY INTEGER R_CURLY
 *				;
 *	option
 *			::=		SEPARATOR label SEPARATOR
 *				|	SEPARATOR label
 *				|	label SEPARATOR
 *				|	label
 *				;
 *	label
 *			::=		DISABLED CHECK ID
 *				|	DISABLED ID
 *				|	CHECK ID
 *				|	ID
 *				;
 *
 *	e.g. "(/[{<priority>}][--][#][@]<label>[--])+"
 *
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_COMPONENT_USER_COMMAND_H_
#define IDE_COMPONENT_USER_COMMAND_H_

#include "Common.h"
#include "IDECommon.h"

#include "DynamicCall.h"

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

class wxMenuBar;
class wxToolBar;

namespace ide
{
	class ComponentMenu;
	class ComponentMenuBar;
	class ComponentToolBar;

	enum MenuType {
		MT_MAIN		= 1 << 0,
		MT_CTX		= 1 << 1,
		MT_TOOLBAR	= 1 << 2,
		MT_NMAIN	= MT_CTX  | MT_TOOLBAR,
		MT_NCTX		= MT_MAIN | MT_TOOLBAR,
		MT_NTOOLBAR	= MT_MAIN | MT_CTX,
		MT_ALL		= MT_MAIN | MT_CTX | MT_TOOLBAR,
	};

	//----------------------------
	//-- class UserCommand

	class _IDE_API UserCommand
	{
	public:

		///--- Callbacks
		typedef DCall<void>						Callback;
		typedef std::vector<Callback>			CallbackVec;
		typedef boost::shared_ptr<CallbackVec>	CallbackVecPtr;

		///--- type definitions
		enum UserCommandType {
			UCT_INVALID=0, UCT_NORMAL,
			UCT_SEPARATOR, UCT_CHECK_ITEM,
			UCT_SUB_MENU,
		};

		///--- constructors / destructor
		UserCommand(UserCommandType type=UCT_SUB_MENU, uint flags=MT_ALL, bool isStatic=true, const String& image=String())
			: type(type), menuType(flags), isStatic(isStatic), enabled(true), image(image) {}

		UserCommand(const Callback& action, UserCommandType type=UCT_NORMAL,
			uint flags=MT_ALL, bool isStatic=true, bool enabled=true, const String& image=String())
			: type(type), menuType(flags), isStatic(isStatic), enabled(enabled), image(image), action(action)
			{ assert(type != UCT_SUB_MENU && type != UCT_SEPARATOR); }

		~UserCommand(void) {}

		///--- Accessor API
		bool IsValid(void) const		{ return type != UCT_INVALID; }
		bool IsSeparator(void) const	{ return type == UCT_SEPARATOR; }
		bool IsCheck(void) const		{ return type == UCT_CHECK_ITEM; }
		bool IsSubMenu(void) const		{ return type == UCT_SUB_MENU; }
		bool IsStatic(void) const		{ return isStatic; }
		bool IsEnabled(void) const		{ return enabled; }

		uint GetFlags(void) const	{ return menuType; }
		void SetFlags(uint flags)	{ menuType = flags; }

		const String& GetImage(void) const	{ return image; }
		void SetImage(const String& image)	{ this->image = image; }

		void SetEnabled(bool val = true)		{ enabled = val; }
		void SetCheck(bool check = true)		{ type = UCT_CHECK_ITEM; }
		void SetAction(const Callback& action)	{ this->action = action; }
		void ChangeOwner(const std::string& toClassId, const std::string& fromClassId);

		const Callback& GetAction(void) const	{ return action; }

		///--- Build API
		bool InsertUserCommand(const String& path, const UserCommand& command, int priority=0);
		bool RemoveUserCommand(const String& path);
		bool EnableUserCommand(const String& path);
		bool DisableUserCommand(const String& path);
		bool HasUserCommand(const String& path) const;

		static UserCommand Merge(const UserCommand& left, const UserCommand& right);
		UserCommand& Merge(const UserCommand& right);

		static UserCommand Subtract(const UserCommand& left, const UserCommand& right);
		UserCommand& Subtract(const UserCommand& right);
		UserCommand& SubtractNonStatic(const UserCommand& right);

		///--- Retrieval API
		const UserCommand& GetUserCommand(const String& path) const;
		UserCommand& GetUserCommand(const String& path);

		///--- GUI Creation Functions
		ComponentMenu*	  GenerateMenu(ComponentMenu*) const;
		ComponentMenuBar* GenerateMenuBar(ComponentMenuBar* menubar) const;
		ComponentToolBar* GenerateToolBar(ComponentToolBar* toolbar) const;

		///--- Overloaded Operators
		UserCommand  operator +(const UserCommand& right)	{ return UserCommand::Merge(*this, right); }
		UserCommand& operator +=(const UserCommand& right)	{ return Merge(right); }
		UserCommand  operator -(const UserCommand& right)	{ return UserCommand::Subtract(*this, right); }
		UserCommand& operator -=(const UserCommand& right)	{ return Subtract(right); }

	private:
		friend class MenuGenerator;
		friend class MenuBarGenerator;
		friend class ToolBarGenerator;

		friend struct OptionSeeker;
		friend struct InsertCommand;
		friend struct MakeLeaf;

		friend class boost::serialization::access;

		template<class Archive>
		void serialize (Archive& ar, const unsigned int version)
			{ ar & type & menuType & image & action & menu; }

		typedef std::pair<String, UserCommand> Option;
		typedef std::multimap<int, Option> Menu;

		///--- private API
		bool AppendCommandMapping(const Option& mapping);
		static void NormalizeSeparators(Menu& menu);

		Menu::const_iterator GetCommand(const String& title) const;
		Menu::iterator GetCommand(const String& title);

		///--- private members
		UserCommandType type;
		uint menuType;
		bool enabled;
		bool isStatic;

		String image;
		Callback action;

		Menu menu;
	};


	//----------------------------
	//-- class MenuGenerator

	class MenuGenerator
	{
	public:

		///--- type definitions
		typedef UserCommand::CallbackVecPtr CallbackVecPtr;

		///--- constructors / destructor
		MenuGenerator(uint menuType)
			: menuType(menuType) {}

		///--- public API
		ComponentMenu* Generate(ComponentMenu* menu, const UserCommand& command,
			CallbackVecPtr actions=CallbackVecPtr()) const;

	private:
		///--- private API
		void CreateMenuItem(ComponentMenu* menu,
			const UserCommand::Option& option) const;

		///--- private members
		uint menuType;
	};


	//----------------------------
	//-- class MenuBarGenerator

	class MenuBarGenerator
	{
	public:
		///--- public API
		ComponentMenuBar* Generate(ComponentMenuBar* menubar, const UserCommand& command) const;
	};


	//----------------------------
	//-- class ToolBarGenerator

	class ToolBarGenerator
	{
	public:
		///--- public API
		ComponentToolBar* Generate(ComponentToolBar* toolbar, const UserCommand& command) const;

	private:
		///--- private API
		void CreateToolItem(ComponentToolBar* toolbar,
			const UserCommand::Option& option) const;
	};
}

#endif	//IDE_COMPONENT_USER_COMMAND_H_
