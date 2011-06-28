/**
 *	ComponentInitializer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentInitializer.h"
#include "ComponentRegistrar.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentInitializer class
//
void ComponentInitializer::InitializePendingComponent (const std::string& compId)
{
	ComponentImageRegistrar::Commit(compId);

	ComponentEntryCreator::Commit(compId);
	ComponentMetadataSetter::Commit(compId);

	ComponentEntryFunctionRegistrar::Commit(compId);
	ComponentEntryUserCmdFunctionRegistrar::Commit(compId);
	ComponentEntrySignalRegistrar::Commit(compId);
	ComponentEntrySlotRegistrar::Commit(compId);

	ComponentPostInitializer::Commit(compId);
}

//**********************************************************************

void ComponentInitializer::InitializePendingComponents (void)
{
	ComponentImageRegistrar::Commit();

	ComponentEntryCreator::Commit();
	ComponentMetadataSetter::Commit();

	ComponentEntryFunctionRegistrar::Commit();
	ComponentEntryUserCmdFunctionRegistrar::Commit();
	ComponentEntrySignalRegistrar::Commit();
	ComponentEntrySlotRegistrar::Commit();

	ComponentPostInitializer::Commit();
}

//**********************************************************************

void ComponentInitializer::ClearPendingComponents (void)
{
	ComponentImageRegistrar::Clear();

	ComponentEntryCreator::Clear();
	ComponentMetadataSetter::Clear();

	ComponentEntryFunctionRegistrar::Clear();
	ComponentEntryUserCmdFunctionRegistrar::Clear();
	ComponentEntrySignalRegistrar::Clear();
	ComponentEntrySlotRegistrar::Clear();

	ComponentPostInitializer::Clear();
}

//**********************************************************************

bool ComponentInitializer::HasPendingComponents (void)
{
	return ComponentEntryCreator::HasPendingComponents();
}

//**********************************************************************

void ComponentInitializer::GetPendingComponents (StdStringList& result)
{
	ComponentEntryCreator::GetPendingComponents(result);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
