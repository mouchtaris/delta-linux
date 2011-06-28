/**
 *	ErrorList.h
 *
 *	-- IDE Console Output --
 *
 *	List control component for compilation error/warning.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef IDE_ERROR_LIST_COMPONENT_H_
#define IDE_ERROR_LIST_COMPONENT_H_

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
	class ErrorList :
		public ItemListCtrl,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(ErrorList);

	public:
		///--- type definitions

		///--- constructors / destructor
		ErrorList(void);
		~ErrorList(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(void, Append, (const String& type, const String& content, const String& file, uint line),
			_("Append an error or warning message to the error list"));
		DECLARE_EXPORTED_FUNCTION_(void, Clear, (void),
			_("Clear error list messages"));

		DECLARE_EXPORTED_STATIC_(void, View, (void),
			_("Show the Error List Window"));

		DECLARE_EXPORTED_FUNCTION(void, OnWorkspaceLoaded, (const Handle& workspace, const String& uri));
		DECLARE_EXPORTED_FUNCTION(void, OnWorkStarted, (const std::string workspace, const Handle& root, const String& task));
		DECLARE_EXPORTED_FUNCTION(void, OnCompilationMessage, (const std::string& caller, const UIntList& buildId,
			const String& type, const String& content, const String& file, uint line));
	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		void onItemActivated(wxListEvent& event);

		///--- private members
		uint32 counter;
	};
}

#endif	//IDE_ERROR_LIST_COMPONENT_H_
