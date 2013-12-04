/**
 *	TreeItemComponent.h
 *
 *	-- IDE Common Components --
 *
 *	Common component base class of components that can
 *	be inserted to a tree control common component.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef COMMON_TREE_ITEM_COMPONENT_H_
#define COMMON_TREE_ITEM_COMPONENT_H_

#include "Common.h"
#include "IDECommon.h"
#include "IDEComponent.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include <list>
#include <string>

namespace ide
{
	//----------------------------
	//-- class TreeItemComponent

	class _IDE_API TreeItemComponent :
		public IDEComponent
	{
		DECLARE_IDE_COMPONENT(TreeItemComponent);

	public:
		///--- type definitions

		///--- constructors / destructor
		TreeItemComponent(void);
		~TreeItemComponent(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);
		virtual wxWindow* GetWindow(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC_(void, RegisterChildType, (const std::string& parentId, const std::string& classId),
			_("Register a component as an acceptable child of the tree item"));
		DECLARE_EXPORTED_STATIC_(void, UnregisterChildType, (const std::string& parentId, const std::string& classId),
			_("Unregister a component as an acceptable child of the tree item"));
		DECLARE_EXPORTED_STATIC_(const StdStringList, GetChildrenTypesForClass, (const std::string& parentId),
			_("Retrieve a list with the classIds of components that are acceptable children for the given item"));
		DECLARE_EXPORTED_STATIC_(const StringList, GetSupportedExtensions, (const std::string& classId),
			_("Retrieve a list of supported resource extensions"));
		DECLARE_EXPORTED_STATIC_(const StdStringList, GetExtensionTypes, (const std::string& extension),
			_("Retrieve a list of type resources that support the given extension"));

		DECLARE_EXPORTED_FUNCTION_(virtual const StdStringList, GetChildrenTypes, (void),
			_("Retrieve a list with the classIds of components that are acceptable children for this item"));

		DECLARE_EXPORTED_FUNCTION_(virtual const String&, GetName, (void),
			_("Get component's name"));
		DECLARE_EXPORTED_FUNCTION_(virtual const String&, GetOriginalName, (void),
			_("Get component's original name (for storage related components to allow maintaining environment variables)"));
		DECLARE_EXPORTED_FUNCTION_(virtual const String&, GetURI, (void),
			_("Get component's URI"));
		DECLARE_EXPORTED_FUNCTION_(virtual bool, HasDefaultSymbolicURI, (void),
			_("Check if the symbolic uri is auto-generated"));
		DECLARE_EXPORTED_FUNCTION_(virtual const String, GetSymbolicURI, (void),
			_("Get component's symbolic URI"));
		DECLARE_EXPORTED_FUNCTION_(virtual const String, GetPath, (void),
			_("Get component's directory path"));
		DECLARE_EXPORTED_FUNCTION_(virtual const String, GetLeafURI, (void),
			_("Get component's leaf node of the URI"));

		DECLARE_EXPORTED_FUNCTION_(virtual void, SetName, (const String& name),
			_("Set component's name"));
		DECLARE_EXPORTED_FUNCTION_(virtual void, SetOriginalName, (const String& name),
			_("Set component's original name (for storage related components to allow maintaining environment variables)"));
		DECLARE_EXPORTED_FUNCTION_(virtual void, SetURI, (const String& uri),
			_("Set component's URI"));
		DECLARE_EXPORTED_FUNCTION_(virtual void, SetSymbolicURI, (const String& uri),
			_("Set component's symbolic URI"));
		DECLARE_EXPORTED_FUNCTION_(virtual void, SetTreeView, (const Handle& handle),
			_("Set component's parent tree view"));

		DECLARE_EXPORTED_FUNCTION_(void, SetExpanded, (bool value),
			_("Set if the tree component item is expanded or collapsed"));

		DECLARE_EXPORTED_FUNCTION_(const String, GetDefaultSymbolicURIFromURI, (const String& uri),
			_("Transforms the given uri to a symbolic uri using the default conversion (relative path)"));
		DECLARE_EXPORTED_FUNCTION_(void, CreateDefaultSymbolicURI, (void),
			_("Create a default symbolic URI for the component based on its uri"));

		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetParent, (void),
			_("Return a handle to the parent"));
		DECLARE_EXPORTED_FUNCTION_(virtual HandleList, CollectChildren, (const String& type),
			_("Return a list of handles of all children components of the specified type"));
		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetChild, (const String& name),
			_("Return a handle to a child given its name"));
		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetChildByURI, (const String& uri),
			_("Return a handle to a child given its URI"));
		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetChildBySymbolicURI, (const String& uri),
			_("Return a handle to a child given its symbolic URI"));

		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetDirectChild, (const String& name),
			_("Return a handle to a direct child given its name"));
		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetDirectChildByURI, (const String& uri),
			_("Return a handle to a direct child given its URI"));
		DECLARE_EXPORTED_FUNCTION_(virtual Handle, GetDirectChildBySymbolicURI, (const String& uri),
			_("Return a handle to a direct child given its symbolic URI"));

		DECLARE_EXPORTED_FUNCTION_(virtual bool, HasNamingConflict, (const String& name, const String& uri),
			_("Checks if there is a name confict between the given name/uri and this item"));

	protected:
		///--- protected overridden API
		virtual void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);
		virtual void ComponentDestroyed(void);

		///--- protected members
		typedef std::multimap<std::string, std::string> StrStrMMap;
		static StrStrMMap childrenTypes;
		static StrStrMMap extensions;

		Handle treeview;

	private:
		///--- private API

		///--- private members
		String	name;
		String	originalName;
		String	uri;
		bool	hasDefaultSymbolic;
	};
}

#endif	//COMMON_TREE_ITEM_COMPONENT_H_
