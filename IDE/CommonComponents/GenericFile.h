/**
 *	GenericFile.h
 *
 *	-- IDE Common Components --
 *
 *	GenericFile component is the component representing
 *	a generic file in the tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef COMMON_COMPONENTS_FILE_H_
#define COMMON_COMPONENTS_FILE_H_

#include "Common.h"
#include "IDECommon.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include "TreeItemComponent.h"

namespace ide
{
	class _IDE_API GenericFile :
		public TreeItemComponent
	{
		DECLARE_IDE_COMPONENT(GenericFile);

	public:
		///--- type definitions

		///--- constructors / destructor
		GenericFile(void);
		~GenericFile(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(virtual bool, Load, (const String& uri),
			_("Load file from disk"));
		DECLARE_EXPORTED_FUNCTION_(bool, Save, (void),
			_("Save file to disk"));
		DECLARE_EXPORTED_FUNCTION_(virtual bool, SaveAs, (const String& uri),
			_("Save file to a new file"));
		DECLARE_EXPORTED_FUNCTION_(void, DefaultAction, (void),
			_("Execute file's default action (open)"));

		DECLARE_EXPORTED_FUNCTION(virtual void, Open, (void));
		DECLARE_EXPORTED_FUNCTION(void, RemoveCtx, (void));
		DECLARE_EXPORTED_FUNCTION(void, RenameDialog, (void));
		DECLARE_EXPORTED_FUNCTION(virtual bool, Rename, (const String& newName));
		DECLARE_EXPORTED_FUNCTION(void, Properties, (void));

	protected:
		///--- protected API
		virtual void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);

	private:
		///--- private API

		///--- private members
	};
}

#endif	//COMMON_COMPONENTS_FILE_H_
