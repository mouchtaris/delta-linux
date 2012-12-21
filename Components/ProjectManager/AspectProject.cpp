/**
 *	AspectProject.cpp
 *
 *	-- IDE Aspect Project Component --
 *
 *	AspectProject component is the component representing
 *	a collection of aspect files in the Project Manager's tree view.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2012
 */
#include "AspectProject.h"
#include "IDEDialogs.h"

#include "ComponentEntry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"

#include "BitmapRegistry.h"
#include "ProjectManagerCommon.h"

#include <wx/filename.h>
#include <boost/foreach.hpp>

#include "Icons/run_script.xpm"
#include "Icons/debug_script.xpm"
#include "Icons/debug_script_console.xpm"
#include "Icons/build.xpm"
#include "Icons/delete.xpm"

namespace ide
{
	static const char *defaultChildren[] = { "Aspect" };

	//-------------------------------------------------------//
	//---- class AspectProject ------------------------------//

	COMPONENT_METADATA(
		AspectProject,
		_("AspectProject"),
		_("AspectProject component is the component representing a collection of \
		  aspect files in the Project Manager's tree view"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("0.1")
	);
	IMPLEMENT_COMPONENT_(AspectProject, Container);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(AspectProject, table)
	{
		StringVec options;
		options.push_back(String(_T("AspectProject")));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of project in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	AspectProject::AspectProject(void)
	{
		conf::AddScriptExecutionProperties(this);
		conf::AddProjectBuildProperties(this);
	}

	//-----------------------------------------------------------------------

	AspectProject::~AspectProject(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(AspectProject, void, Initialize, (void))
	{
		Call<void (const std::string&, const std::string&)>
			Register(s_classId, "TreeItemComponent", "RegisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Register(s_classId, child);

		extensions.insert(std::make_pair(s_classId, ".aproj"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(AspectProject, void, CleanUp, (void))
	{
		extensions.erase(s_classId);

		Call<void (const std::string&, const std::string&), SafeCall>
			Unregister(s_classId, "TreeItemComponent", "UnregisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Unregister(s_classId, child);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, void, DefaultAction, (void))
	{
		PropertiesCtx();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, const HandleList, GetPreTransformations, (void))
		{ return GetTransformations(_("Pre")); }

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, const HandleList, GetInterimTransformations, (void))
		{ return GetTransformations(_("Interim")); }

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, const HandleList, GetPostTransformations, (void))
		{ return GetTransformations(_("Post")); }

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, void, MoveChildUp, (const Handle& child))
	{
		List::iterator i = std::find(orderedChildren.begin(), orderedChildren.end(), child.Resolve());
		assert(i != orderedChildren.end());
		if (i != orderedChildren.begin())
		{
			List::iterator pos = i;
			orderedChildren.splice(--pos, orderedChildren, i);
			Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, void, MoveChildDown, (const Handle& child))
	{
		List::iterator i = std::find(orderedChildren.begin(), orderedChildren.end(), child.Resolve());
		assert(i != orderedChildren.end());
		if (i != --orderedChildren.end())
		{
			List::iterator pos = ++i;
			orderedChildren.splice(--pos, orderedChildren, i);
			Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(AspectProject, UpdateScriptProperties, _("/{18}--Update Script Properties"), MT_CTX, "")
	{
		const String message = _("This action will overwrite all build and execution properties for scripts within this project.\nAre you sure you want to proceed?");
		if (gui::getConfirmation(0, _T("AspectProject"), message)) {
			HandleList scripts = CollectChildren(_T("Script"));
			BOOST_FOREACH(const Handle& script, scripts)
				SetChildProperties(script);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(AspectProject, "delete", delete_xpm);
	EXPORTED_CMD_FUNCTION(AspectProject, RemoveCtx, _("/{50}--Remove"), MT_CTX, "delete")
	{
		assert(treeview);
		const Handle& parent = Call<const Handle& (const Handle&)>(this, treeview, "GetParent")(this);
		Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(this);
		Destroy();

		if (parent)
			Call<void (void)>(s_classId, parent, "Save")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, void, SetChildProperties, (const Handle& handle))
	{
		using namespace conf;
		if (Component* child = handle.Resolve()) {
			for (const char** p = conf::GetProjectBuildPropertyIdsForScripts(); *p; ++p)
				if (const Property* prop = GetInstanceProperty(*p))
					child->AddInstanceProperty(*p, prop->Clone());
			for (const char** p = conf::GetScriptExecutionPropertyIds(); *p; ++p)
				if (const Property* prop = GetInstanceProperty(*p))
					child->AddInstanceProperty(*p, prop->Clone());

			if (child->GetClassId() == "Aspect") {
				conf::EnumStringProperty* prop = conf::GenerateScriptDeploymentProperty();
				prop->SetValue(_T(DELTA_SCRIPT_DEPLOYMENT_LIBRARY));
				prop->SetEnabled(false);
				child->AddInstanceProperty(conf::GetDeploymentPropertyId(), prop->Clone());
				delete prop;

				conf::Property* output = const_cast<conf::Property*>(child->GetInstanceProperty("output"));
				if (output && conf::get_prop_value<conf::StringProperty>(output, _T("")).empty()) {
					String name = Call<const String& (void)>(this, child, "GetName")();
					name = name.substr(0, name.find_last_of(_T(".")));
					conf::set_prop_value<conf::StringProperty>(output, name);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(AspectProject, int, OnCompareItems, (const Handle& handle1, const Handle& handle2))
	{
		assert(handle1.Resolve() && handle2.Resolve());
		List::const_iterator begin = orderedChildren.begin(), end = orderedChildren.end();
		List::const_iterator i1 = std::find(begin, end, handle1.Resolve());
		List::const_iterator i2 = std::find(begin, end, handle2.Resolve());
		assert(i1 != end && i2 != end);
		return std::distance(begin, i1) - std::distance(begin, i2);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(AspectProject, void, OnLibraryDefinitionsChanged,
		(const std::string& classId, const StringList newDefinitions), "LibraryDefinitionsChanged")
	{
		using namespace conf;
		if (const Property *property = GetInstanceProperty("libs")) {
			const PropertyTable old = GetInstancePropertyTable();
			MultiChoiceProperty* p = safe_prop_cast<MultiChoiceProperty>(const_cast<Property*>(property));
			BOOST_FOREACH(const String& prevChoice, p->GetAllChoices())	//Remove old choices
				if (std::find(newDefinitions.begin(), newDefinitions.end(), prevChoice) == newDefinitions.end())
					p->RemoveChoice(prevChoice);
			BOOST_FOREACH(const String& def, newDefinitions)			//Add new choices
				p->AddChoice(def);
			this->ComponentAppliedChangedProperties(old, PropertyIdVec(1, "libs"));
		}
	}

	//-----------------------------------------------------------------------

	void AspectProject::ChildAdded (Component* component)
	{
		orderedChildren.push_back(component);
	}

	//-----------------------------------------------------------------------

	void AspectProject::ChildRemoved (Component* component)
	{
		orderedChildren.remove(component);
		Save();
		TreeItemComponent::ChildRemoved(component);
	}

	//-----------------------------------------------------------------------

	void AspectProject::ChildDestroyed (Component* component)
	{
		orderedChildren.remove(component);
		TreeItemComponent::ChildDestroyed(component);
	}

	//-----------------------------------------------------------------------

	const HandleList AspectProject::GetTransformations(const String& type) {
		HandleList result;
		BOOST_FOREACH(Component* child, GetChildren()) {
			const conf::MultiChoiceProperty* p = static_cast<const conf::MultiChoiceProperty*>(child->GetInstanceProperty("type"));
			const conf::MultiChoiceProperty::ChoiceMap& choices = p->GetChoiceMap();
			conf::MultiChoiceProperty::ChoiceMap::const_iterator i = choices.find(type);
			assert(i != choices.end());
			if (i->second)
				result.push_back(child);
		}
		return result;
	}
}
