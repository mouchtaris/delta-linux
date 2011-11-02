/**
 *	IDEComponent.h
 *
 *	-- IDE Shell Component --
 *
 *	IDEComponent is the base component of the IDE
 *	it provides some basic exported functions shared
 *	by all the components in the system.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *
 *	July 2007
 */
#include "IDEComponent.h"

#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "ComponentUserCommand.h"
#include "ComponentFactory.h"
#include "StringUtils.h"
#include "Call.h"
#include "Undo.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	Component* IDEComponent::dmslC=0;


	//-------------------------------------------------------//
	//---- class IDEComponent -------------------------------//

	COMPONENT_METADATA(
		IDEComponent,
		_("IDE Component"),
		_("Base class of all the IDE components that provides some common functions"),
		_T("Giannis Georgalis <jgeorgal@ics.forth.gr>"),
		_T("0.1a")
	);
	IMPLEMENT_COMPONENT(IDEComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(IDEComponent, table)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(IDEComponent, void, Initialize, (void))
	{
		if (dmslC)
			return;

		dmslC = ComponentFactory::Instance().CreateComponent("DMSLCompiler");
		if (!dmslC)
			return;

		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		Call<bool (const String&)>(s_classId, dmslC, "Compile")(sparrowDir + _T("sparrow.dmsl"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(IDEComponent, void, CleanUp, (void))
	{
		if (dmslC)
			ComponentFactory::Instance().DestroyComponent(dmslC);
		dmslC=0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(IDEComponent, StringList, GetDecisions, (const String& component, const String& profile))
	{
		if (!Call<bool (const String&)>(s_classId, dmslC, "LoadProfile")(profile)	||
			!Call<bool (const String&)>(s_classId, dmslC, "Evaluate")(component))
			return StringList();

		return Call<StringList (void)>(s_classId, dmslC, "GetDecisions")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, void, AddCommand, (const String& path, const UserCommandDesc& desc))
	{
		const UserCommand command(
			UserCommand::Callback("User", desc.GetCallback().first, desc.GetCallback().second),
			desc.IsCheck() ? UserCommand::UCT_CHECK_ITEM : UserCommand::UCT_NORMAL,
			desc.GetFlags(),
			desc.IsStatic(),
			desc.IsEnabled(),
			desc.GetImage()
		);
		AddUserCommand(path, command);
		Undo<void (const String&)>(this, "RemoveCommand")(path);
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, void, RemoveCommand, (const String& path))
	{
		const UserCommandDesc desc = this->GetCommand(path);
		if (desc.IsValid())
		{
			RemoveUserCommand(path);
			Undo<void (const String&, UserCommandDesc)>(this, "AddCommand")(path, desc);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, void, EnableCommand, (const String& path))
	{
		const UserCommandDesc desc = this->GetCommand(path);
		if (desc.IsValid())
		{
			EnableUserCommand(path);
			Undo<void (const String&)>(this, "DisableCommand")(path);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, void, DisableCommand, (const String& path))
	{
		const UserCommandDesc desc = this->GetCommand(path);
		if (desc.IsValid())
		{
			DisableUserCommand(path);
			Undo<void (const String&)>(this, "EnableCommand")(path);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, void, AddInstanceCommand, (const String& path, const UserCommandDesc& desc))
	{
		const UserCommand command(
			UserCommand::Callback("User", desc.GetCallback().first, desc.GetCallback().second),
			desc.IsCheck() ? UserCommand::UCT_CHECK_ITEM : UserCommand::UCT_NORMAL,
			desc.GetFlags(),
			desc.IsStatic(),
			desc.IsEnabled(),
			desc.GetImage()
		);
		ComponentAddedUserCommand(path, command);
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, void, RemoveInstanceCommand, (const String& path))
	{
		ComponentRemovedUserCommand(path);
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(IDEComponent, const UserCommandDesc, GetCommand, (const String& path))
	{
		const UserCommand& command =  GetUserCommand(path);
		const UserCommand::Callback& action = command.GetAction();

		UserCommandDesc desc(
			UserCommandDesc::Callback(action.GetClassId(), action.GetFunction()),
			command.IsCheck(),
			static_cast<long>(command.GetFlags()),
			command.IsStatic(),
			command.IsEnabled(),
			command.GetImage()
		);
		return desc;
	}

	//-----------------------------------------------------------------------
}
