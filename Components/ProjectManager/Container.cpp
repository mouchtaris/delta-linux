/**
 *	Container.cpp
 *
 *	-- IDE Container Component --
 *
 *	A concrete (storage related) container for components in the ProjectManager hierarchy.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	June 2010
 */
#include "Container.h"

#include "StringUtils.h"
#include "Properties.h"
#include "PropertyUtils.h"
#include "XMLPropertyVisitor.h"
#include "GenericDialogs.h"
#include "FileChangeWatcher.h"

#include "ComponentFactory.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "DelayedCaller.h"

#include "PropertyTable.h"

#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <wx/filename.h>

#include "Icons/delete.xpm"
#include "Icons/properties.xpm"

namespace ide
{
	//-------------------------------------------------------//
	//---- class Container ----------------------------------//

	COMPONENT_METADATA(
		Container,
		_("Container"),
		_("Container for ProjectManager components"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("0.1")
		);
	IMPLEMENT_COMPONENT_(Container, VirtualContainer);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Container, table)
	{
	}


	//-----------------------------------------------------------------------

	Container::Container(void) : m_internalSaveCounter(0), m_changeWatcher(0),
		m_reloadPending(false), m_duringReload(false)
	{
	}

	//-----------------------------------------------------------------------

	Container::~Container(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Container, bool, Load, (const String& uri))
	{
		wxXmlDocument doc;
		if (!wxFileName::FileExists(uri) || !doc.Load(uri))
			return false;

		const std::string derivedType = GetClassId();

		//-- Start processing the XML file
		if (doc.GetRoot()->GetName() != util::std2str(derivedType))
 			return false;

		xml::Node root(doc.GetRoot());
		SetName(root[_T("name")]);

		const String oldUri = GetURI();	//no reference
		if (!m_duringReload) {
			SetURI(uri);
			if (IsTopLevelContainer())
				CreateDefaultSymbolicURI();
		}

		//-- Load properties
		conf::XMLValueLoader propertyLoader;
		propertyLoader.SetParent(root);
		const_cast<conf::PropertyTable&>(GetInstancePropertyTable()).Accept(derivedType + "Properties", &propertyLoader);

		bool collapsed = conf::get_prop_value<conf::BoolProperty>(GetInstanceProperty("collapsed"), false);

		if (!LoadChildrenForComponent(root, this))
			return false;
		Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		if (collapsed && !IsTopLevelContainer())
			Call<void (const Handle&)>(this, treeview, "Collapse")(this);

		const String startup = GetStartupResource();
		if(!startup.empty())
			Call<void (const Handle&, bool)>(this, treeview, "SetStartupResource")(GetChildByURI(startup), true);
		
		if (!m_duringReload) {
			CancelFileModificationWatcher();
			m_changeWatcher = util::FileChangeWatcher::Instance().Register(
				GetURI(),
				boost::bind(&Container::OnFileModification, this)
			);
		}
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Container, bool, Reload, (void))
	{
		const String& uri = GetURI();
		if (uri.empty())
			return false;
		else {
			m_duringReload = true;
			const List children = this->Component::GetChildren();	//no reference
			BOOST_FOREACH(Component *comp, children)
			{
				Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(comp);
				comp->Destroy();
			}
			bool retval = Load(uri);
			m_duringReload = false;
			return retval;
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Container, bool, Save, (void))
	{
		return SaveAs(GetURI());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Container, bool, SaveProperties, (void))
	{
		return Save();	//Save properties to disk
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Container, bool, SaveAs, (const String& uri))
	{
		//-- Check path existance
		const std::string path = util::str2std(uri.substr(0, uri.find_last_of('/')));
		if (!path.empty() && !boost::filesystem::exists(path))
			boost::filesystem::create_directory(path);

		const std::string derivedType = GetClassId();

		//-- Create XML document
		xml::Node root;
		root.Create(util::std2str(derivedType));

		root.SetProperty(_T("name"), GetName());
		root.SetProperty(_T("version"), _T("alpha"));

		//-- Save workspace properties
		conf::XMLValueSaver propertySaver(conf::MAKE_PATHS_RELATIVE, GetPath());
		propertySaver.SetParent(root);
		const_cast<conf::PropertyTable&>(GetInstancePropertyTable()).Accept(derivedType + "Properties", &propertySaver);
		SaveChildrenForComponent(root, this);

		++m_internalSaveCounter;
		wxXmlDocument doc;
		doc.SetFileEncoding(_T("utf-8"));
		doc.SetRoot(root.NativeType());
		return doc.Save(uri, 2);
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Container, Edit, _("/{0}Edit--"), MT_CTX, "")
	{
		Call<void (const std::string&)>(this, "DockableComponent", "EnsureVisibility")("EditorManager");
		if (const Handle editor = Call<const Handle (const String&)>(this, "EditorManager", "OpenDocument")(GetURI()))
			Call<void (const Handle&)>(this, editor, "SetAssociatedComponent")(this);
		else
			gui::displayMessage(0, _("Error"),
				_("Unable to open ") + util::std2str(GetClassId()) + _(" file '") + GetURI() + _("'\n")
			);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Container, "properties", properties_xpm);
	EXPORTED_CMD_FUNCTION(Container, PropertiesCtx, _("/{5000}--Properties"), MT_NMAIN, "properties")
	{
		ComponentConfigurationDialog().ConfigureInstance(this);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Container, void, Run, (void))
	{
		const String type = util::std2str(GetClassId());
		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(_("Running ") + type + _T("...\n"));
		const String startup = GetStartupResource();
		Handle resource;
		if (!startup.empty() && (resource = GetChildByURI(startup)))
			Call<void (void), SafeCall>(this, resource, "Run")();
		else
			Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
				(_("Execution Error"), _("Start-up resource could not be found!"));
	}

	//-----------------------------------------------------------------------

	void Container::ComponentDestroyed(void)
	{
		CancelFileModificationWatcher();
		VirtualContainer::ComponentDestroyed();
	}

	//-----------------------------------------------------------------------

	void Container::ChildDestroyed(Component* component)
	{
		if (!GetInDestruction() && !m_duringReload) {
			if (GetStartupResource() == Call<const String& (void)>(this, component, "GetURI")())
				conf::set_prop_value<conf::StringProperty>(
					const_cast<conf::Property*>(GetInstanceProperty("startup")), String()
				);
			Save();
		}
		TreeItemComponent::ChildDestroyed(component);
	}

	//-----------------------------------------------------------------------

	bool Container::LoadChildrenForComponent(const xml::Node& root, Component* comp)
	{
		BOOST_FOREACH(const xml::Node& node,
			boost::iterator_range<xml::Node::iterator>(
				root.ChildBegin(_T("Child")), root.ChildEnd(_T("Child"))
			)
		)
		{
			const std::string type = util::str2std(node[_T("type")]);
			Component* item = ComponentFactory::Instance().CreateComponent(type);
			if (!item)
				return false;
			item->SetParent(comp);
			assert(treeview);
			Call<bool (const Handle&, const Handle&)>(comp, treeview, "AddComponent")(comp, item);
			
			if (type == "Filter") {
				LoadChildrenForComponent(node, item);
				Call<void (const String&)>(comp, item, "SetName")(node[_T("name")]);
				Call<void (const String&)>(comp, item, "SetURI")(String());	//filters don't have valid uris
			}
			else {
				if (type == "Script" || type == "Aspect") {	// Get child properties (if any).
					conf::XMLValueLoader propertyLoader;
					propertyLoader.SetParent(node);
					const_cast<conf::PropertyTable&>(item->GetInstancePropertyTable()).Accept(type + "Properties", &propertyLoader);
				}

				String path = node[_T("name")];	// Bwd compatibility.
				if (path.empty())
					path = node.GetContent();

				wxFileName filename(path);
				if (!filename.IsAbsolute())
					path = GetPath() + path;
				Call<void (const String&)>(comp, item, "Load")(path);

				const String symbolic = node[_T("symbolic")];
				if (symbolic.empty())
					Call<void (void)>(comp, item, "CreateDefaultSymbolicURI")();
				else
					Call<void (const String&)>(comp, item, "SetSymbolicURI")(symbolic);
			}
		}
		return true;
	}

	//-----------------------------------------------------------------------

	void Container::SaveChildrenForComponent(xml::Node& root, Component* comp)
	{
		BOOST_FOREACH(Component* child, comp->GetChildren())
		{
			const std::string type = child->GetClassId();
			xml::Node node;
			node.Create(_T("Child"));
			node.SetProperty(_T("type"), util::std2str(type));

			if (type == "Filter") {
				node.SetProperty(_T("name"), Call<const String& (void)>(this, child, "GetName")());
				SaveChildrenForComponent(node, child);
			}
			else {
				wxFileName filename(Call<const String& (void)>(this, child, "GetURI")());
				if (type == "Script" || type == "Aspect") {	//-- Save child properties
					conf::XMLValueSaver propertySaver(conf::MAKE_PATHS_RELATIVE, filename.GetPath());
					propertySaver.SetParent(node); 
					const_cast<conf::PropertyTable&>(child->GetInstancePropertyTable()).Accept(type + "Properties", &propertySaver);
				}

				if (!Call<bool (void)>(this, child, "HasDefaultSymbolicURI")())
					node.SetProperty(_T("symbolic"), Call<const String& (void)>(this, child, "GetSymbolicURI")());

				if (filename.IsAbsolute())
					filename.MakeRelativeTo(GetPath());	
				node.SetProperty(_T("name"), filename.GetFullPath());
			}
			root.InsertChild(node);
		}
	}

	//-----------------------------------------------------------------------

	void Container::CancelFileModificationWatcher(void)
	{
		boost::mutex::scoped_lock lock(m_changeWatcherMutex);
		if (m_changeWatcher) {
			util::FileChangeWatcher::Instance().Cancel(GetURI(), m_changeWatcher);
			m_changeWatcher = 0;
		}
		m_internalSaveCounter = 0;
	}

	//-----------------------------------------------------------------------

	void Container::OnFileModification(void)
	{
		boost::mutex::scoped_lock lock(m_changeWatcherMutex);
		if (m_changeWatcher) 
			if (m_internalSaveCounter)
				--m_internalSaveCounter;
			else {
				boost::mutex::scoped_lock lock(m_reloadMutex);
				if (!m_reloadPending) {
					m_reloadPending = true;
					timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(&Container::AskForReload, this));
				}	
			}
	}

	//-----------------------------------------------------------------------

	void Container::AskForReload(void)
	{
		assert(m_reloadPending);
		const String message = _("The workspace '") + GetName() + _("' has been modified outside the environment.");
		const String question = _T("Do you want to reload the updated workspace from disk?");
		if (gui::getConfirmation(0, _T("File Modification Detected"), message + _T("\n") + question))
			Reload();
		{
			boost::mutex::scoped_lock lock(m_reloadMutex);
			m_reloadPending = false;
		}
	}

	//-----------------------------------------------------------------------
}
