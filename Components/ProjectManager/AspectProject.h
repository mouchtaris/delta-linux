/**
 *	AspectProject.h
 *
 *	-- IDE Aspect Project Component --
 *
 *	AspectProject component is the component representing
 *	a collection of aspect files in the Project Manager's tree view.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2012
 */
#ifndef PROJECT_MANAGER_ASPECT_PROJECT_H_
#define PROJECT_MANAGER_ASPECT_PROJECT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include "Container.h"

namespace ide
{
	class AspectProject :
		public Container
	{
		DECLARE_IDE_COMPONENT(AspectProject);

	public:
		///--- type definitions

		///--- constructors / destructor
		AspectProject(void);
		~AspectProject(void);

		///--- public API
		virtual const List&	GetChildren (void) const { return orderedChildren; }

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(virtual void, SetName, (const String& name),
			_("Set component's name"));

		DECLARE_EXPORTED_FUNCTION_(void, DefaultAction, (void), _("Execute aspect project's default action (configure)"));

		DECLARE_EXPORTED_FUNCTION(const HandleList, GetPreTransformations, (void));
		DECLARE_EXPORTED_FUNCTION(const HandleList, GetInterimTransformations, (void));
		DECLARE_EXPORTED_FUNCTION(const HandleList, GetPostTransformations, (void));

		DECLARE_EXPORTED_FUNCTION(void, MoveChildUp, (const Handle& child));
		DECLARE_EXPORTED_FUNCTION(void, MoveChildDown, (const Handle& child));

		DECLARE_EXPORTED_FUNCTION(void, UpdateScriptProperties, (void));
		DECLARE_EXPORTED_FUNCTION(void, RemoveCtx, (void));

		DECLARE_EXPORTED_FUNCTION(void, SetChildProperties, (const Handle& handle));
		DECLARE_EXPORTED_FUNCTION(int, OnCompareItems, (const Handle& handle1, const Handle& handle2));
		DECLARE_EXPORTED_FUNCTION(void, OnLibraryDefinitionsChanged, (const std::string& classId, const StringList newDefinitions));
		DECLARE_EXPORTED_FUNCTION(void, OnAspectProjectAdded, (const std::string& classId, const Handle& project));
		DECLARE_EXPORTED_FUNCTION(void, OnAspectProjectRemoved, (const std::string& classId, const Handle& project));

	protected:
		///--- protected API
		virtual void ComponentDestroyed (void);
		virtual void ChildAdded (Component* component);
		virtual void ChildRemoved (Component* component);
		virtual void ChildDestroyed (Component* component);

	private:
		///--- private API
		const HandleList GetTransformations(const String& type);

		///--- private members
		List orderedChildren;
	};
}

#endif	//PROJECT_MANAGER_ASPECT_PROJECT_H_