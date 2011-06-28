#include "IViewsApplicationCommunicator.h"

namespace iviews {

//-----------------------------------------------------------------------

IViewsApplicationCommunicator::IViewsApplicationCommunicator(void) : frep::ApplicationCommunicator() {}

//-----------------------------------------------------------------------

bool IViewsApplicationCommunicator::CanDoUndoAction (void) const
	{ return false; }

//-----------------------------------------------------------------------

void IViewsApplicationCommunicator::BeginUndoAction (void)
	{}

//-----------------------------------------------------------------------

void IViewsApplicationCommunicator::EndUndoAction (void)
	{}

//-----------------------------------------------------------------------

bool IViewsApplicationCommunicator::CanWriteToOutput (void) const
	{ return false; }

//-----------------------------------------------------------------------

void IViewsApplicationCommunicator::WriteToOutput (const std::string &) const
	{}

}