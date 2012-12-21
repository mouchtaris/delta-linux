/**
 *	Container.h
 *
 *	-- IDE Container Component --
 *
 *	A concrete (storage related) container for components in the ProjectManager hierarchy.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	June 2010
 */
#ifndef PROJECT_MANAGER_CONTAINER_H_
#define PROJECT_MANAGER_CONTAINER_H_

#include "Common.h"

#include "VirtualContainer.h"

namespace ide
{
	class Container :
		public VirtualContainer
	{
		DECLARE_IDE_COMPONENT(Container);

	public:
		///--- constructors / destructor
		Container(void);
		~Container(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION_(virtual bool, Load, (const String& uri), _("Load item from disk"));
		DECLARE_EXPORTED_FUNCTION_(virtual bool, Reload, (void), _("Reload item from disk"));
		DECLARE_EXPORTED_FUNCTION_(virtual bool, Save, (void), 	_("Save item to disk"));
		DECLARE_EXPORTED_FUNCTION_(virtual bool, SaveProperties, (void), 	_("Save item properties"));
		DECLARE_EXPORTED_FUNCTION_(virtual bool, SaveAs, (const String& uri), _("Save item to a new file"));

		DECLARE_EXPORTED_FUNCTION(virtual void, Edit, (void));
		DECLARE_EXPORTED_FUNCTION(virtual void, PropertiesCtx, (void));

		DECLARE_EXPORTED_FUNCTION_(virtual void, Run, (void), _("Run"));

	protected:

		///--- protected API
		virtual void ComponentDestroyed(void);
		virtual void ChildDestroyed(Component* component);

	private:
		///--- private API
		bool LoadChildrenForComponent(const xml::Node& root, Component* comp);
		void SaveChildrenForComponent(xml::Node& root, Component* comp);

		void CancelFileModificationWatcher(void);
		void OnFileModification(void);
		void AskForReload(void);

		///--- private members
		unsigned			m_internalSaveCounter;
		unsigned long long	m_changeWatcher;
		boost::mutex		m_changeWatcherMutex;
		bool				m_reloadPending;
		bool				m_duringReload;
		boost::mutex		m_reloadMutex;
	};
}

#endif	//PROJECT_MANAGER_CONTAINER_H_
