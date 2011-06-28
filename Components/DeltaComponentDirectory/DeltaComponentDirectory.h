/**
 *	DeltaComponentDirectory.h
 *
 *	-- IDE Delta Component Directory --
 *
 *	Delta Component Directory component is a visual component
 *	that allows browsing and management of available Delta
 *	script components, a utility for Sparrow extension authors.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_DELTA_COMPONENT_DIRECTORY_COMPONENT_H_
#define IDE_DELTA_COMPONENT_DIRECTORY_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "ComponentUserCommand.h"

#include "DockableComponent.h"

#include <wx/panel.h>

///////////////////////////////////////////////////////////
// DEFINE DLL EXPORT //////////////////////////////////////

#ifdef DELTACOMPONENTDIRECTORY_EXPORTS
#	define _DELTA_COMPONENT_DIRECTORY_API	UTIL_EXPORT
#	define _DELTA_COMPONENT_DIRECTORY_C_API	UTIL_C_EXPORT
#else
#	define _DELTA_COMPONENT_DIRECTORY_API	UTIL_IMPORT
#	define _DELTA_COMPONENT_DIRECTORY_C_API	UTIL_C_IMPORT
#endif

#pragma warning(disable: 4251)

class wxListView;
class wxListEvent;
class wxCommandEvent;

namespace ide
{
	class ImageList;

	class DeltaComponentDirectory :
		public wxPanel,
		public DockableComponent
	{
		DECLARE_IDE_COMPONENT(DeltaComponentDirectory);

	public:
		///--- type definitions

		///--- constructors / destructor
		DeltaComponentDirectory(void);
		~DeltaComponentDirectory(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void)	{ return this; }

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_MEMBER_(bool, RegisterComponent, (const String& uri),
			_("Register a script component to the system by supplying its source URI"));
		DECLARE_EXPORTED_MEMBER_(bool, UnregisterComponent, (const std::string& classId),
			_("Unregister a component from the system"));
		DECLARE_EXPORTED_MEMBER_(bool, CheckByteCodeFileExistsAndUpTodate, (const std::string& classId),
			_("Update the binary of the script component"));
		DECLARE_EXPORTED_MEMBER_(bool, LoadComponent, (const std::string& classId),
			_("Load a script component definition (no instance creation)"));
		DECLARE_EXPORTED_MEMBER_(Handle, CreateComponent, (const std::string& classId),
			_("Creates a script instance and returns a handle to it"));
		DECLARE_EXPORTED_MEMBER_(bool, UnloadComponent, (const std::string& classId),
			_("Unload a component from the environment"));
		DECLARE_EXPORTED_MEMBER_(StringList, GetAvailableComponents, (void),
			_("Return a class id list of available Delta script components"));
		DECLARE_EXPORTED_MEMBER_(const String, GetComponentFromSymbolicURI, (const String& symbolic),
			_("Return a class id from the given symbolic uri of a DeltaComponent"));
		DECLARE_EXPORTED_MEMBER_(const String, GetURIFromComponent, (const String& classId),
			_("Return the uri of a DeltaComponent based on its class id"));


		DECLARE_EXPORTED_STATIC_(void, View, (void),
			_("Show the Delta Component Directory"));

	private:
		friend struct ProfileLoader;

		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		const std::string GetExtensionDirectory(void) const;
		const std::string GetXDFDirectory(void) const;

		void ConstructToolbar(bool loaded, bool selected=true);

		void CreateXDF(const std::string& classId, const std::string& source);
		void DeleteXDF(const std::string& classId);

		void InsertComponent(const std::string& classId);
		void RemoveComponent(const std::string& classId);

		void slotComponentLoaded(const std::string& classId);
		void slotComponentUnloaded(const std::string& classId);

		void onItemSelected(wxListEvent& event);
		void onItemActivated(wxListEvent& event);
		void onComponentRegister(wxCommandEvent& event);
		void onComponentUnregister(wxCommandEvent& event);
		void onComponentLoad(wxCommandEvent& event);
		void onComponentUnload(wxCommandEvent& event);

		///--- private members
		wxToolBar* toolbar;
		wxListView* listview;
		ImageList* images;

		typedef std::map<std::string, bool> FailedMap;
		FailedMap failed;
		void	AssumeFailed (const std::string& classId);
		void	AssumeSucceeded (const std::string& classId);
		bool	HadFailed (const std::string& classId) const;
	};
}

#endif	//IDE_DELTA_COMPONENT_DIRECTORY_COMPONENT_H_
