/**
 *	VirtualContainer.h
 *
 *	-- IDE VirtualContainer Component --
 *
 *	A virtual (non-storage related) container for components in the ProjectManager hierarchy.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	June 2010
 */
#ifndef PROJECT_MANAGER_VIRTUAL_CONTAINER_H_
#define PROJECT_MANAGER_VIRTUAL_CONTAINER_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include "TreeItemComponent.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "xml.h"

namespace ide
{
	class VirtualContainer :
		public TreeItemComponent
	{
		DECLARE_IDE_COMPONENT(VirtualContainer);

	public:
		///--- constructors / destructor
		VirtualContainer(void);
		~VirtualContainer(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION(virtual bool, CanDestroy, (void));

		DECLARE_EXPORTED_FUNCTION(virtual bool, Save, (void));
		DECLARE_EXPORTED_FUNCTION(virtual bool, SaveProperties, (void));
		DECLARE_EXPORTED_FUNCTION(virtual bool, SaveAll, (void));

		DECLARE_EXPORTED_FUNCTION(virtual void, BuildCtx, (void));
		DECLARE_EXPORTED_FUNCTION(virtual unsigned long, Build, (const UIntList& workId));
		DECLARE_EXPORTED_FUNCTION(virtual void, CleanCtx, (void));
		DECLARE_EXPORTED_FUNCTION(virtual unsigned long, Clean, (const UIntList& workId));

		DECLARE_EXPORTED_FUNCTION(virtual void, EnableWorkCommands, (void));
		DECLARE_EXPORTED_FUNCTION(virtual void, DisableWorkCommands, (void));

		DECLARE_EXPORTED_FUNCTION(virtual void, NewItemCtx, (void));
		DECLARE_EXPORTED_FUNCTION(virtual void, AddExistingItemCtx, (void));
		DECLARE_EXPORTED_FUNCTION(virtual void, AddFilterCtx, (void));

		DECLARE_EXPORTED_FUNCTION(virtual bool, NewItem, (const std::string& type, const String& name, const String& uri));
		DECLARE_EXPORTED_FUNCTION(virtual bool, AddExistingItem, (const std::string& type, const String& uri));
		DECLARE_EXPORTED_FUNCTION(virtual bool, SetStartupResource, (const String& resource));
		DECLARE_EXPORTED_FUNCTION(virtual const String, GetStartupResource, (void));
		DECLARE_EXPORTED_FUNCTION(virtual void, ResetStartupResource, (void));

		DECLARE_EXPORTED_FUNCTION(virtual void, SetChildProperties, (const Handle& handle));
		DECLARE_EXPORTED_FUNCTION(virtual int, OnCompareItems, (const Handle& handle1, const Handle& handle2));
		DECLARE_EXPORTED_FUNCTION(virtual void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId));
		DECLARE_EXPORTED_FUNCTION(virtual void, OnWorkCanceled, (const std::string& caller, const Handle& root, const String& task));

	protected:
		///--- protected API
		virtual bool IsTopLevelContainer(void) const { return false; }
		virtual bool HasChildrenNamingConflict(const String& name, const String& uri);

		virtual void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);
		virtual void ChildRemoved(Component* component);
		virtual void ChildDestroyed(Component* component);

	private:

		///--- private API
		bool AreChildrenUpToDate(Component* container);

		void WorkThread(const std::string& task, const UIntList& workId);
		void ClearWorkingChildren(void);
		
		const String NewFilterName(void);
		void StopWorker(void);
		bool SaveComponent(Component *comp);
		static void OnResourceWorkStarted(Component *component, const String& task, const UIntList& workId);
		static void OnResourceWorkCompleted(Component *component, const String& task, const UIntList& workId);

		///--- private members
		typedef boost::thread* boost_thread_ptr_t;
		typedef std::pair<boost_thread_ptr_t, bool> WorkerInfo;
		WorkerInfo			m_worker;
		boost::mutex		m_mutex;
		volatile bool		m_stopWorker;

		typedef std::pair<unsigned long, UIntList>	WorkInfo;
		typedef std::map<Component *, WorkInfo>		WorkMap;
		WorkMap					m_workingChildren;
		boost::recursive_mutex	m_workingChildrenMutex;
		UIntList				m_workId;
	};
}

#endif	//PROJECT_MANAGER_VIRTUAL_CONTAINER_H_
