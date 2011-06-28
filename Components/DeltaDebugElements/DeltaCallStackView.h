/**
 *	DeltaCallStackView.h
 *
 *	-- IDE Call Stach View --
 *
 *	View of a Delta program's call stack during debugging.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#ifndef IDE_DELTA_CALL_STACK_VIEW_COMPONENT_H_
#define IDE_DELTA_CALL_STACK_VIEW_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "DockableComponent.h"

#include "ItemListCtrl.h"

#include "UndefineAllWin32Shit.h"

#pragma warning(disable: 4251)

namespace ide
{
	class DeltaCallStackView :
		public ItemListCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(DeltaCallStackView);

	public:
		///--- type definitions

		///--- constructors / destructor
		DeltaCallStackView(void);
		~DeltaCallStackView(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(int, GetTotalStackFrames, (void),
			_("Return the total number of stack frame in call stack"));
		DECLARE_EXPORTED_FUNCTION_(String, GetStackFrame, (uint index),
			_("Return the stack frame with specified index"));
		DECLARE_EXPORTED_FUNCTION_(void, MoveStackFrame, (uint index),
			_("Move stack pointer to specified index"));
		DECLARE_EXPORTED_FUNCTION_(void, RefreshCallStackView, (void),
			_("Refresh the call stach view"));

		DECLARE_EXPORTED_STATIC_(void, View, (void),
			_("Show the Call Stack View Window"));

		///--- exported SLOTS
		DECLARE_EXPORTED_FUNCTION_(void, slotPushStackFrame,
			(const std::string& classId, const String& record, uint defLine, uint callLine,
				uint scope, const String& params),
			_("Push stack frame to the list view"));
		DECLARE_EXPORTED_FUNCTION_(void, slotStackFrameMoved, (const std::string& classId, uint index),
			_("Update trace information"));
		DECLARE_EXPORTED_FUNCTION_(void, slotClear,	(const std::string& classId),
			_("Clear call stack list view"));
		DECLARE_EXPORTED_FUNCTION_(void, slotTreeItemSymbolicURIChanged, (const Handle& handle, const String& oldUri, const String& newUri),
			_("Load trace information for the editor"));
		DECLARE_EXPORTED_FUNCTION_(void, slotFileNameChanged, (const Handle& editor, const String& uri),
			_("Load trace information for the editor"));
	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void MarkActivationRecord(uint index);
		void ClearActivationRecordMarkings(void);
		void AddStackFrame(const String& record, uint defLine, uint callLine, uint scope, const String& params);

		void onItemActivated(wxListEvent& event);

		///--- private members
		typedef std::pair<String, int> TracePointInfo;	// <source, line>
		TracePointInfo tracePoint;
	};
}

#endif	//IDE_DELTA_CALL_STACK_VIEW_COMPONENT_H_
