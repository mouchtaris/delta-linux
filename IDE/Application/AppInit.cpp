/**
*	AppInit.h
*
*	-- Delta IDE --
*
*	AppInit class initializes the IDE's subsystems
*	and components.
*
*	Themistoklis Bourdenas <themis@ics.forth.gr>
*	March 2007
*/
#include "AppInit.h"

#include "Blackboard.h"

#include "ComponentHolder.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentInitializer.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class AppInit ------------------------------------//

	void AppInit::Initialize(void)
	{
		Blackboard::Initialize();

		ComponentHolder::Initialize();
		ComponentRegistry::Initialize();
		ComponentFactory::Initialize();
		ComponentInitializer::InitializePendingComponents();
	}

	//-----------------------------------------------------------------------

	void AppInit::CleanUp(void)
	{
		//ComponentInitializer::CleanUp();
		ComponentRegistry::CleanUp();
		ComponentFactory::CleanUp();
		ComponentHolder::CleanUp();

		Blackboard::CleanUp();
	}

	//-----------------------------------------------------------------------
}
