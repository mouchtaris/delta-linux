/**
 *	Output.h
 *
 *	-- IDE Console Output --
 *
 *	Console output component for the IDE.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef IDE_OUTPUT_COMPONENT_H_
#define IDE_OUTPUT_COMPONENT_H_

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
	class Output :
		public TextCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(Output);

	public:
		///--- type definitions

		///--- constructors / destructor
		Output(void);
		~Output(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(void, Append, (const String& text),
			_("Append text to the output"));
		DECLARE_EXPORTED_FUNCTION_(void, AppendOrdered, (const String& text, const UIntList& order),
			_("Append text to the output (order is only shown, not maintained)"));
		DECLARE_EXPORTED_FUNCTION_(const String, GetText, (void),
			_("Returns text of the output console"));
		DECLARE_EXPORTED_FUNCTION_(void, Clear, (void), _("Clear output text"));

		DECLARE_EXPORTED_STATIC_(void, View, (void), _("Show the Output Window"));
		DECLARE_EXPORTED_STATIC_(void, Configure, (void), _("Configures the Output Window"));

		DECLARE_EXPORTED_FUNCTION(void, OnWorkspaceLoaded, (const Handle& workspace, const String& uri));

		DECLARE_EXPORTED_FUNCTION(void, OnResourceWorkStarted, (const Handle& resource, const String& task, const UIntList& workId));
		DECLARE_EXPORTED_FUNCTION(void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId));
		
		DECLARE_EXPORTED_FUNCTION(void, OnWorkStarted, (const std::string workspace, const Handle& root, const String& task));
		DECLARE_EXPORTED_FUNCTION(void, OnWorkCanceled, (const std::string workspace, const Handle& root, const String& task));

		DECLARE_EXPORTED_FUNCTION(void, OnCompilationMessage, (const std::string& caller, const UIntList& buildId,
			const String& type, const String& content, const String& file, uint line));

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onDisplay(wxCommandEvent& event);
		void onSelect(wxMouseEvent& event);

		static const String OrderedText(const String& text, const UIntList& order);

		///--- private members
	};
}

#endif	//IDE_OUTPUT_COMPONENT_H_
