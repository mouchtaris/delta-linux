/*
 * SparrowApplicationCommunicator.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#include "Call.h"
#include "Common.h"
#include "Streams.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentRegistry.h"

#include "SparrowApplicationCommunicator.h"

namespace ide {
	//-----------------------------------------------------------------------
	
	void SparrowApplicationCommunicator::BeginUndoAction (void)
		{ Call<void (void)>("FindAndReplace", "Editor", "BeginUndoAction")(); }

	//-----------------------------------------------------------------------
	
	void SparrowApplicationCommunicator::EndUndoAction (void)
		{ Call<void (void)>("FindAndReplace", "Editor", "EndUndoAction")(); }

	//-----------------------------------------------------------------------

	bool SparrowApplicationCommunicator::CanWriteToOutput (void) const {
		return	ComponentRegistry::Instance().GetFocusedInstance("Output")			&&
				ComponentRegistry::Instance().GetFocusedInstance("ProjectManager");
	}

	//-----------------------------------------------------------------------

	void SparrowApplicationCommunicator::WriteToOutput (const std::string & msg) const {
		Call<void (void)>("FindAndReplace", "Output", "Clear")();

		Call<void (const String& text)>
		("FindAndReplace", "Output", "Append")
		(util::std2str(msg));
		return;
	}

}	//namespace ide