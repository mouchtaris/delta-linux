/**
 *	Filter.h
 *
 *	-- IDE Profile Manager Component --
 *
 *	Filter component is the component representing a virtual
 *	folder able to contain projects, scripts and files.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2009
 */
#ifndef PROJECT_MANAGER_FILTER_H_
#define PROJECT_MANAGER_FILTER_H_

#include "Common.h"

#include "VirtualContainer.h"

namespace ide
{
	class Filter :
		public VirtualContainer
	{
		DECLARE_IDE_COMPONENT(Filter);

	public:
		///--- type definitions

		///--- constructors / destructor
		Filter(void);
		~Filter(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_FUNCTION(void, RemoveCtx, (void));
		DECLARE_EXPORTED_FUNCTION(void, RenameDialog, (void));
		DECLARE_EXPORTED_FUNCTION(bool, Rename, (const String& newName));

		///--- Refined methods
		DECLARE_EXPORTED_FUNCTION(const StdStringList, GetChildrenTypes, (void));
		DECLARE_EXPORTED_FUNCTION(Handle, GetChild, (const String& name));
		DECLARE_EXPORTED_FUNCTION(bool, HasNamingConflict, (const String& name, const String& uri));
		DECLARE_EXPORTED_FUNCTION(bool, Save, (void));

		DECLARE_EXPORTED_FUNCTION(bool, SetStartupResource, (const String& uri));
		DECLARE_EXPORTED_FUNCTION(const String, GetStartupResource, (void));
	
		DECLARE_EXPORTED_FUNCTION(const String, GetPath, (void));

		DECLARE_EXPORTED_FUNCTION(void, SetChildProperties, (const Handle& child));

	protected:
		///--- protected API
		virtual bool HasChildrenNamingConflict(const String& name, const String& uri);

		virtual void ComponentAddedProperty(const std::string& id, const conf::Property* prop);

	private:
		///--- private API
		///--- private members
	};
}

#endif	//PROJECT_MANAGER_FILTER_H_