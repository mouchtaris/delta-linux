/**
 *	BuildOrder.h
 *
 *	-- IDE Console Output --
 *
 *	Output for the ordered build.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	June 2010
 */
#ifndef IDE_OUTPUT_BUILD_ORDER_H_
#define IDE_OUTPUT_BUILD_ORDER_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "DockableComponent.h"

#include "TextCtrl.h"

#include "UndefineAllWin32Shit.h"

#include <stack>

#pragma warning(disable: 4251)

namespace ide
{
	class BuildOrder :
		public TextCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(BuildOrder);

	public:
		///--- type definitions

		///--- constructors / destructor
		BuildOrder(void);
		~BuildOrder(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(void, AppendOrdered, (const String& text, const UIntList& buildId),
			_("Append text to the output"));
		DECLARE_EXPORTED_FUNCTION_(void, Enter, (const UIntList& buildId), _("Enter a new build id level"));
		DECLARE_EXPORTED_FUNCTION_(void, Exit, (const UIntList& buildId), _("Exit from the given build id level"));

		DECLARE_EXPORTED_FUNCTION_(const String, GetText, (void), _("Returns text of the output console"));
		DECLARE_EXPORTED_FUNCTION_(void, Clear, (void), _("Clear output text"));

		DECLARE_EXPORTED_FUNCTION(void, ResetOrder, (void));

		DECLARE_EXPORTED_STATIC_(void, Configure, (void), _("Configures the BuildOrder Window"));

		DECLARE_EXPORTED_FUNCTION(void, OnWorkspaceLoaded, (const Handle& workspace, const String& uri));
		DECLARE_EXPORTED_FUNCTION(void, OnResourceWorkStarted, (const Handle& resource, const String& task, const UIntList& workId));
		DECLARE_EXPORTED_FUNCTION(void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId));
		
		DECLARE_EXPORTED_FUNCTION(void, OnWorkStarted, (const std::string workspace, const Handle& root, const String& task));
		DECLARE_EXPORTED_FUNCTION(void, OnWorkCompleted, (const std::string workspace, const Handle& root, const String& task));
		DECLARE_EXPORTED_FUNCTION(void, OnWorkCanceled, (const std::string workspace, const Handle& root, const String& task));
		
		DECLARE_EXPORTED_FUNCTION(void, OnCompilationMessage, (const std::string& caller, const UIntList& buildId,
			const String& type, const String& content, const String& file, uint line));

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onDisplay(wxCommandEvent& event);
		void onSelect(wxMouseEvent& event);

		bool ProcessEnter (const UIntList& buildId);
		bool ProcessExit (const UIntList& buildId);
		bool ProcessText (const UIntList& buildId, const String& text);
		void HandlePendingMessages(void);

		static const String OrderedText(const String& text, const UIntList& workId);

		///--- private members
		bool				working;
		UIntList			current;
		std::stack<uint>	nextIdLevels;

		enum MessageType {
			ENTER	= 0,
			EXIT	= 1,
			TEXT	= 2
		};
		typedef boost::tuple<MessageType, UIntList, String> WorkMessage;
		typedef std::list<WorkMessage> WorkMessageList;
		WorkMessageList messages;
	};
}

#endif	//IDE_OUTPUT_BUILD_ORDER_H_
