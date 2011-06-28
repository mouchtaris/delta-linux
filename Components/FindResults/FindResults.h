/**
 *	FindResults.h
 *
 *	-- IDE Find Results Output --
 *
 *	Output for the find results for the IDE.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	October 2008
 */
#ifndef IDE_FIND_RESULTS_COMPONENT_H_
#define IDE_FIND_RESULTS_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "DockableComponent.h"

#include "TextCtrl.h"

#include "UndefineAllWin32Shit.h"

#pragma warning(disable: 4251)

namespace ide
{
	class FindResults :
		public TextCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(FindResults);

	public:
		///--- type definitions

		///--- constructors / destructor
		FindResults(void);
		~FindResults(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(void, Append, (const String& text),
			_("Append text to the output"));
		DECLARE_EXPORTED_FUNCTION_(const String, GetText, (void),
			_("Returns text of the output console"));
		DECLARE_EXPORTED_FUNCTION_(void, Clear, (void),
			_("Clear output text"));

		DECLARE_EXPORTED_STATIC_(void, View, (void),
			_("Show the Output Window"));

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onDisplay(wxCommandEvent& event);
		void onSelect(wxMouseEvent& event);

		///--- private members
	};
}

#endif	//IDE_FIND_RESULTS_COMPONENT_H_
