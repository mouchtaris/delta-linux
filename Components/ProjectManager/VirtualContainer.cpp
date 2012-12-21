/**
 *	VirtualContainer.cpp
 *
 *	-- IDE VirtualContainer Component --
 *
 *	A virtual (non-storage related) container for components in the ProjectManager hierarchy.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	June 2010
 */
#include "VirtualContainer.h"
#include "NewItemDialog.h"
#include "IDEDialogs.h"

#include "StringUtils.h"
#include "Properties.h"
#include "PropertyUtils.h"
#include "GenericDialogs.h"
#include "FileChangeWatcher.h"

#include "ComponentEntry.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "ConsoleHost.h"
#include "DelayedCaller.h"

#include "PropertyTable.h"
#include "Streams.h"
#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"
#include "Algorithms.h"

#include <set>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <wx/filename.h>

#include "Icons/build.xpm"
#include "Icons/add_new_item.xpm"
#include "Icons/add_existing_item.xpm"
#include "Icons/add_filter.xpm"

namespace ide
{
	///--- Functor DescendingCaller
	class DescendingCaller : std::unary_function<Component*, bool>
	{
	public:
		result_type operator ()(argument_type comp) {
			const ComponentEntry& entry =
				ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
			assert(entry);

			BOOST_FOREACH(const std::string& func, funcs)
				if (entry.HasFunction(func))
					return Call<bool (void)>(caller, comp, func)();
			return defaultRetval;
		}

		DescendingCaller(Component *caller, const StdStringList& funcs, bool defaultRetval = false) : 
			caller(caller), funcs(funcs), defaultRetval(defaultRetval) {}
	private:
		Component*		caller;
		StdStringList	funcs;
		bool			defaultRetval;
	};

	//-------------------------------------------------------//
	//---- class VirtualContainer ----------------------------------//

	COMPONENT_METADATA(
		VirtualContainer,
		_("VirtualContainer"),
		_("VirtualContainer for ProjectManager components"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("0.1")
		);
	IMPLEMENT_COMPONENT_(VirtualContainer, TreeItemComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(VirtualContainer, table)
	{
	}


	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(VirtualContainer, ResourceWorkStarted, (const String& task, const UIntList& workId));
	EXPORTED_SIGNAL(VirtualContainer, ResourceWorkCompleted, (const String& task, const UIntList& workId));

	//-----------------------------------------------------------------------

	VirtualContainer::VirtualContainer(void) : m_worker(boost_thread_ptr_t(0), false), m_stopWorker(false)
	{
		AddInstanceProperty("startup", new conf::StringProperty(_("Startup Resource"), String(),
			_("Startup resource for the container"))
		);
	}

	//-----------------------------------------------------------------------

	VirtualContainer::~VirtualContainer(void)
	{
		StopWorker();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, CanDestroy, (void))
	{
		bool hasWorker = false;
		{
			boost::mutex::scoped_lock lock(m_mutex);
			if (m_worker.first)
				hasWorker = true;
		}

		bool hasChildren = false;
		{
			boost::recursive_mutex::scoped_lock lock(m_workingChildrenMutex);
			hasChildren  = !m_workingChildren.empty();
		}

		if (hasWorker || hasChildren)
		{
			const String str = m_worker.second ? _T("Build is in progress. Cancel build?"):
				_T("Clean is in progress. Cancel clean?");
			if (gui::getConfirmation(0, util::std2str(GetClassId()), str))
			{
				ClearWorkingChildren();
				StopWorker();
				return true;
			}
			else
				return false;
		}
		else {
			const Component::List children = this->Component::GetChildren();
			//copy the list to avoid having children removed due to other code running concurrently with this iteration
			BOOST_FOREACH(Component* comp, children)
			{
				const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
				assert(entry);
				if (entry.HasFunction("CanDestroy")&& !Call<bool (void)>(this, comp, "CanDestroy")())
					return false;
			}
			return true;
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, Save, (void))
	{
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, SaveProperties, (void))
	{
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, SaveAll, (void))
	{
		if (!Save())
			return false;

		//-- Save children recursively
		StdStringList saveFuncs;
		saveFuncs.push_back("SaveAll");
		saveFuncs.push_back("Save");
		return util::reduce(
			GetChildren().begin(),
			GetChildren().end(),
			std::logical_and<bool>(),
			DescendingCaller(this, saveFuncs)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(VirtualContainer, "build", build_xpm);
	EXPORTED_CMD_FUNCTION(VirtualContainer, BuildCtx, _("/{10}--Build"), MT_NMAIN, "build")
	{
		const Handle& workspace = Call<const Handle& (void)>(this, treeview, "GetWorkspace")();
		if (!Call<const Handle& (void)>(this, workspace, "GetRootWorkingResource")()) {
			Call<void (const Handle&, const String& task)>(this, workspace, "StartWorking")(this, _T("Build"));
			Build(UIntList(1, 1));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, unsigned long, Build, (const UIntList& workId))
	{
		boost::mutex::scoped_lock lock(m_mutex);
		if (!m_worker.first) {
			m_worker.second = true;
			m_worker.first = new boost::thread(boost::bind(&VirtualContainer::WorkThread, this, "Build", workId));
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(VirtualContainer, CleanCtx, _("/{15}Clean"), MT_CTX, "")
	{
		const Handle& workspace = Call<const Handle& (void)>(this, treeview, "GetWorkspace")();
		if (!Call<const Handle& (void)>(this, workspace, "GetRootWorkingResource")()) {
			uint sn = GetSerial();
			Call<void (const Handle&, const String& task)>(this, workspace, "StartWorking")(this, _T("Clean"));
			if (ComponentRegistry::Instance().IsValidInstance(this, sn))
				Clean(UIntList(1, 1));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, unsigned long, Clean, (const UIntList& workId))
	{
		boost::mutex::scoped_lock lock(m_mutex);
		if (!m_worker.first) {
			m_worker.second = false;
			m_worker.first = new boost::thread(boost::bind(&VirtualContainer::WorkThread, this, "Clean", workId));
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, void, EnableWorkCommands, (void))
	{
		EnableCommand(_("/Build"));
		EnableCommand(_("/Clean"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, void, DisableWorkCommands, (void))
	{
		DisableCommand(_("/Build"));
		DisableCommand(_("/Clean"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(VirtualContainer, "add_new_item", add_new_item_xpm);
	EXPORTED_CMD_FUNCTION(VirtualContainer, NewItemCtx, _("/{20}--Add/{0}New Item..."), MT_NMAIN, "add_new_item")
	{
		NewItemDialog dlg(0, GetChildrenTypes(), GetPath());
		dlg.ShowModal();

		if (dlg.IsOk())
			NewItem(util::str2std(dlg.GetItemType()), dlg.GetItemName(), dlg.GetItemURI());
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(VirtualContainer, "add_existing_item", add_existing_item_xpm);
	EXPORTED_CMD_FUNCTION(VirtualContainer, AddExistingItemCtx, _("/{20}--Add/{10}Existing Item..."), MT_NMAIN, "add_existing_item")
	{
		String wildcard;
		std::set<String> allSupportedExtensions;
		std::vector<std::string> types;

		const StdStringList& items = GetChildrenTypes();
		BOOST_FOREACH(const std::string& classId, items)
		{
			StringList extensions =
				Call<StringList (const std::string&)>(this, classId, "GetSupportedExtensions")(classId);
			BOOST_FOREACH(const String& ext, extensions)
			{
				allSupportedExtensions.insert(ext);
				wildcard += util::std2str(classId) + _T(" ") + _T("(*") + ext + _T(")")
					+ _T("|*") + ext + _T("|");
				types.push_back(classId);
			}
		}
		String supportedExtensionsStr;
		BOOST_FOREACH(const String& ext, allSupportedExtensions) {
			if (!supportedExtensionsStr.empty())
				supportedExtensionsStr += _T(";");
			supportedExtensionsStr += _T("*") + ext;
		}
		wildcard += _("All supported types (") + supportedExtensionsStr + _T(")|") + supportedExtensionsStr;
		types.push_back("");	//empty for auto-detect

		FileDialog fileDlg(0, _("Add Existing Item"), String(), String(), wildcard,
			wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
		fileDlg.SetFilterIndex(types.size() - 1);
		if (fileDlg.ShowModal() != wxID_OK)
			return;

		wxArrayString files;
		fileDlg.GetPaths(files);
		const std::string selectedType = types[fileDlg.GetFilterIndex()];
		for(unsigned i = 0; i < files.size(); ++i) {
			String uri = files[i];
			std::string type;
			if (selectedType.empty()) {	//auto-detect
				const StdStringList extensionTypes = 
					TreeItemComponent::GetExtensionTypes(util::str2std(_T(".") + wxFileName(uri).GetExt()));

				StdStringList validTypes;
				BOOST_FOREACH(const std::string& extensionType, extensionTypes)
				if (std::find(items.begin(), items.end(), extensionType) != items.end())
					validTypes.push_back(extensionType);
				
				if (validTypes.size() == 1)
					type = validTypes.front();
				else {
					using namespace conf;
					PropertyTable table;
					table.AddProperty("file", new FileProperty(_("File"), uri, _("Path for imported file")));
					StringVec options;
					BOOST_FOREACH(const std::string& str, validTypes)
						options.push_back(util::std2str(str));
					options.push_back(_T("GenericFile"));
					options.push_back(_T("TextFile"));
					table.AddProperty("type", new EnumStringProperty(_("Type"), options, 0, _("Type for imported file")));
					if (DefaultGUIGenerator().ShowDialogFromProperties(0, table, PropertyIdVec(), _("Import file as..."))) {
						type = util::str2std(safe_prop_cast<EnumStringProperty>(table.GetProperty("type"))->GetValue());
						uri = safe_prop_cast<FileProperty>(table.GetProperty("file"))->GetValue();
					}
				}
			}
			else
				type = selectedType;
			if (!type.empty())
				AddExistingItem(type, uri);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(VirtualContainer, "add_filter", add_filter_xpm);
	EXPORTED_CMD_FUNCTION(VirtualContainer, AddFilterCtx, _("/{20}--Add/{20}New Filter"), MT_NMAIN, "add_filter")
	{
		String name = NewFilterName();
		Component* item = ComponentFactory::Instance().CreateComponent("Filter");
		if (!item)
			return;
		item->SetParent(this);

		//-- Append item in the treeview
		assert(treeview);
		Call<void (const String&)>(this, item, "SetName")(name);
		Call<void (const String&)>(this, item, "SetURI")(String());
		Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);
		Call<void (const Handle&)>(this, treeview, "SortChildren")(this);

		//-- Save new changes
		Save();
		SaveComponent(item);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, NewItem, (const std::string& type, const String& name, const String& uri))
	{
		if (HasChildrenNamingConflict(name, uri)) {
			gui::displayMessage(0, _("Illegal Operation"),
				_("There is already an item with the same Name/URI in the container!"));
			return false;
		}

		Component* item = ComponentFactory::Instance().CreateComponent(type);
		if (!item)
			return false;
		item->SetParent(this);

		//-- Append item in the treeview
		assert(treeview);
		Call<void (const String&)>(this, item, "SetName")(name);
		Call<void (const String&)>(this, item, "SetURI")(uri);
		Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);
		Call<void (void)>(this, item, "CreateDefaultSymbolicURI")();
		Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		SetChildProperties(item);

		//-- Save new changes
		Save();
		SaveComponent(item);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, AddExistingItem, (const std::string& type, const String& uri))
	{
		if (HasChildrenNamingConflict(String(), uri)) {
			gui::displayMessage(0, _("Illegal Operation"),
				_("There is already an item with the same Name/URI in the container!"));
			return false;
		}

		//-- Create & Load existing item
		Component* item = ComponentFactory::Instance().CreateComponent(type);
		if (!item)
			return false;
		item->SetParent(this);

		assert(treeview);
		Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);
		Call<void (const String&)>(this, item, "Load")(uri);
		Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		SetChildProperties(item);

		//-- Save new changes
		Save();
		SaveComponent(item);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, bool, SetStartupResource, (const String& uri))
	{
		if (GetChildByURI(uri)) {
			ResetStartupResource();
			
			wxFileName filename(uri);
			if (filename.IsAbsolute())
				filename.MakeRelativeTo(GetPath());	
			conf::set_prop_value<conf::StringProperty>(
				const_cast<conf::Property*>(GetInstanceProperty("startup")),
				filename.GetFullPath()
			);

			Call<void (const Handle&, bool)>(this, treeview, "SetStartupResource")(GetChildByURI(uri), true);
			Save();
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, const String, GetStartupResource, (void))
	{
		String resource = conf::get_prop_value<conf::StringProperty>(GetInstanceProperty("startup"), String());
		if (!resource.empty()) {
			wxFileName filename(resource);
			if (!filename.IsAbsolute())
				resource = GetPath() + resource;
		}
		return util::normalizepath(resource);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, void, ResetStartupResource, (void))
	{
		const String startup = GetStartupResource();
		if (!startup.empty())
			Call<void (const Handle&, bool)>(this, treeview, "SetStartupResource")(GetChildByURI(startup), false);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, void, SetChildProperties, (const Handle& handle))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(VirtualContainer, int, OnCompareItems, (const Handle& handle1, const Handle& handle2))
	{
		char *priorities[] = { "Filter", "AspectProject", "Project", "Aspect", "Script", "TextFile", "GenericFile" };
		assert(handle1.Resolve() && handle2.Resolve());
		if (handle1->GetClassId() != handle2->GetClassId()) {
			char **pos1 = std::find(priorities, priorities + SIZEOF_ARRAY(priorities), handle1->GetClassId());
			char **pos2 = std::find(priorities, priorities + SIZEOF_ARRAY(priorities), handle2->GetClassId());
			if (pos1 < pos2)
				return -1;
			else if (pos2 < pos1)
				return 1;
		}
		return	wxStricmp(
					Call<const String& (void)>(this, handle1, "GetName")(),
					Call<const String& (void)>(this, handle2, "GetName")()
				);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(VirtualContainer, void, OnResourceWorkCompleted, (const Handle& resource,
		const String& task, const UIntList& workId), "ResourceWorkCompleted")
	{
		Component *comp = resource.Resolve();
		if (comp && comp->GetParent() == this) {
			bool lastResourceBuilt = false;
			{
				boost::recursive_mutex::scoped_lock lock(m_workingChildrenMutex);
				WorkMap::iterator i = m_workingChildren.find(comp);
				if (i != m_workingChildren.end()) {
					UIntList& id = i->second.second;	//Now check that we actually spawned the child with this workId
					if (id.size() == workId.size() && std::equal(id.begin(), id.end(), workId.begin())) {
						m_workingChildren.erase(i);
						lastResourceBuilt = m_workingChildren.empty();
					}
				}
			}
			if (lastResourceBuilt) {
				timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, task, m_workId));
				m_workId.clear();
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(VirtualContainer, void, OnWorkCanceled, (const std::string& caller, const Handle& root,
		const String& task), "WorkCanceled")
	{
		ClearWorkingChildren();
		StopWorker();
	}

	//-----------------------------------------------------------------------

	bool VirtualContainer::HasChildrenNamingConflict(const String& name, const String& uri) {
		BOOST_FOREACH(Component* child, GetChildren())
			if (Call<bool (const String&, const String&)>(this, child, "HasNamingConflict")(name, uri))
				return true;
		return false;
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		if (!changed.empty())
			SaveProperties();
		TreeItemComponent::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::ChildRemoved(Component* component)
	{
		if (GetStartupResource() == Call<const String& (void)>(this, component, "GetURI")())
			conf::set_prop_value<conf::StringProperty>(
				const_cast<conf::Property*>(GetInstanceProperty("startup")), String()
			);
		Save();
		TreeItemComponent::ChildRemoved(component);
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::ChildDestroyed(Component* component)
	{
		if (!GetInDestruction()) {
			if (GetStartupResource() == Call<const String& (void)>(this, component, "GetURI")())
				conf::set_prop_value<conf::StringProperty>(
					const_cast<conf::Property*>(GetInstanceProperty("startup")), String()
				);
			Save();
		}
		TreeItemComponent::ChildDestroyed(component);
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::WorkThread(const std::string& task, const UIntList& workId)
	{
		m_workId = workId;
		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkStarted, this, util::std2str(task), m_workId));

		List childrenForWork;
		BOOST_FOREACH(Component* child, GetChildren())
			if (ComponentRegistry::Instance().GetComponentEntry(child->GetClassId()).HasFunction(task)) {
				boost::recursive_mutex::scoped_lock lock(m_workingChildrenMutex);
				m_workingChildren[child] = WorkInfo(0, UIntList());
				childrenForWork.push_back(child);
			}
		if (childrenForWork.empty()) {
			timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnResourceWorkCompleted, this, util::std2str(task), m_workId));
			m_workId.clear();
		}
		else {
			unsigned id = 0;
			BOOST_FOREACH(Component* child, childrenForWork) {
				UIntList list = workId;
				list.push_back(++id);
				{
					boost::recursive_mutex::scoped_lock lock(m_workingChildrenMutex);
					if (m_workingChildren.find(child) != m_workingChildren.end())
						m_workingChildren[child].second = list;
				}
				unsigned long pid = Call<unsigned long (const UIntList&)>(this, child, task)(list);
				{
					boost::recursive_mutex::scoped_lock lock(m_workingChildrenMutex);
					if (m_workingChildren.find(child) != m_workingChildren.end())	//if not already removed
						m_workingChildren[child].first = pid;
				}
				if (m_stopWorker)
					return;
			}
		}
		{
			boost::mutex::scoped_lock lock(m_mutex);
			if (!m_stopWorker)
				delete m_worker.first, m_worker.first = 0;
		}
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::ClearWorkingChildren(void)
	{
		boost::recursive_mutex::scoped_lock lock(m_workingChildrenMutex);
		BOOST_FOREACH(WorkMap::value_type& pair, m_workingChildren) {
			if (pair.second.first)
				util::ConsoleHost::TerminateProcess(pair.second.first);
			pair.second.second.clear();
		}
		m_workingChildren.clear();
		m_workId.clear();
	}

	//-----------------------------------------------------------------------

	const String VirtualContainer::NewFilterName(void)
	{
		int count = 0;
		String name;
		while(true) {
			bool found = true;
			name = wxString::Format(_T("NewFilter%d"), ++count);
			BOOST_FOREACH(Component* child, GetChildren())
				if (child->GetClassId() == "Filter" && Call<const String& (void)>(this, child, "GetName")() == name) {
					found = false;
					break;
				}
			if (found)
				break;
		}
		return name;
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::StopWorker(void)
	{
		bool hasWorker = false;
		{
			boost::mutex::scoped_lock lock(m_mutex);
			if (m_worker.first)
				hasWorker = m_stopWorker = true;
		}

		if (hasWorker) {
			assert(m_worker.first);
			m_worker.first->join();
			delete m_worker.first, m_worker.first = 0;
			m_stopWorker = false;
		}
	}

	//-----------------------------------------------------------------------

	bool VirtualContainer::SaveComponent(Component* comp)
	{
		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
		assert(entry);
		if (entry.HasFunction("Save"))
			return Call<bool (void)>(this, comp, "Save")();
		else
			return false;
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::OnResourceWorkStarted(Component* component, const String& task, const UIntList& workId)
	{
		sigResourceWorkStarted(component, task, workId);
	}

	//-----------------------------------------------------------------------

	void VirtualContainer::OnResourceWorkCompleted(Component* component, const String& task, const UIntList& workId)
	{
		sigResourceWorkCompleted(component, task, workId);
	}

	//-----------------------------------------------------------------------
}
