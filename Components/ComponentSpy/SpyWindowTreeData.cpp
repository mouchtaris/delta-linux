/**
 *	SpyWindowTreeData.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "SpyWindowTreeData.h"
#include "Component.h"
#include "ComponentFunctionCaller.h"
#include "ComponentSignalRegistry.h"
#include "ComponentRegistry.h"
#include "ComponentLoader.h"
#include "InstanceValidator.h"
#include "Algorithms.h"

#include "ComponentDataTypeConverter.h"
#include "PropertyTable.h"
#include "ComponentConfigurationDialog.h"
#include "Encoding.h"
#include "GenericDialogs.h"

#include <wx/event.h>
#include <wx/menu.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#define SHOW_AUTHOR		0
#define SHOW_VERSION	0

namespace ide {

////////////////////////////////////////////////////////////////////////
// Static variables used in TreeData and TreeDataActionInvoker
//
static const TreeData*						s_instance;
static ide::SWCommandType					s_type;
static TreeData::ResultFunc					s_resultFunc;
static util::instance_validator<TreeData>	s_validator;

////////////////////////////////////////////////////////////////////////
// Class TreeDataActionInvoker
//
DEFINE_EVENT_TYPE(ACTION_EVENT);

//**********************************************************************

class TreeDataActionInvoker : public wxEvtHandler {
private:
	DECLARE_EVENT_TABLE();

	void onAction (wxCommandEvent& event)
	{
		if (s_instance && s_type != SWCMD_NONE && s_validator.contains(s_instance)) {
			bool result = s_instance->DoAction(s_type);
			if (s_resultFunc)
				s_resultFunc(result);
		}
	}
};

//**********************************************************************

BEGIN_EVENT_TABLE(TreeDataActionInvoker, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, ACTION_EVENT, TreeDataActionInvoker::onAction)
END_EVENT_TABLE();

//**********************************************************************

static TreeDataActionInvoker s_treeDataActionInvoker;

////////////////////////////////////////////////////////////////////////
// TreeData superclass
//
TreeData::TreeData (void)
{
	s_validator.add(this);
}

//**********************************************************************

TreeData::~TreeData (void)
{
	s_validator.remove(this);
}

//**********************************************************************

void TreeData::PostAction (SWCommandType type, const ResultFunc& func)
{
	s_instance = this;
	s_type = type;
	s_resultFunc = func;

	wxCommandEvent event(ACTION_EVENT, 0);
	s_treeDataActionInvoker.AddPendingEvent(event);	
}

////////////////////////////////////////////////////////////////////////
// ComponentTreeData
//
ComponentTreeData::~ComponentTreeData (void)
{
	//delete s_menu;
	//s_menu = 0;
}

//**********************************************************************

wxMenu*	ComponentTreeData::GetMenu (void) const
{
	if (s_menu == 0) {
		s_menu = new wxMenu;
		s_menu->Append(SWCMD_UNLOAD_COMPONENT, _("Unload"));
		s_menu->Append(SWCMD_CONFIGURE_COMPONENT, _("Configure..."));
	}
	return s_menu;
}

//**********************************************************************

bool ComponentTreeData::DoAction (SWCommandType type) const
{
	if (type == SWCMD_UNLOAD_COMPONENT) {

		const std::string id = m_id; // This action destroys this
		// ComponentRegistry::Instance().Unregister(id);
		if (!ComponentLoader::Instance().UnloadComponent(id))
			gui::displayMessage(0, _("Unloading component"), _("Cannot unload this component"));
	}
	else if (type == SWCMD_CONFIGURE_COMPONENT)
		return ComponentConfigurationDialog().Configure(m_id);

	return false;
}

//**********************************************************************

const String ComponentTreeData::GetFormattedSummary (void) const
{
	const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(m_id);
	const ComponentEntry::Metadata& metadata = entry.GetMetadata();

	String author = metadata.author;
	String::size_type ltPos = author.find(_T('<'));
	while (ltPos != String::npos) {
		String::size_type gtPos = author.find(_T('>'), ltPos);
		if (ltPos != String::npos) {
			const String mail = author.substr(ltPos + 1, gtPos - ltPos - 1);
			const String replacement =
				_T("<a href=\"mailto:") + mail + _T("\">") + mail + _T("</a>");
			author.replace(ltPos, gtPos - ltPos + 1, replacement);
			ltPos += replacement.length();
		}
		ltPos = author.find(_T('<'), ltPos);
	}
	const String compProp = _T("<compprop comp=\"") + util::std2str(m_id) + _T("\">");

	const String idStr = util::std2str(entry.GetClassId());
	const String compIdDesc = entry.GetBaseClassId().empty() ? idStr:
		idStr + _T(" </b> <i>[inherits]</i> <b> ") + util::std2str(entry.GetBaseClassId());

	std::string derivedDesc = "<ul>";
	BOOST_FOREACH(const std::string& derivedClassId, entry.GetDerivedClasses())
		derivedDesc += "<li> <font color=\"#0000FF\">" + derivedClassId + "</font>";
	derivedDesc += "</ul>";

	return _T("<b>") + compIdDesc + _T("</b> <br>") +
		_T("<p> <b>") + _("Component name:") + _T("</b> <br>") + metadata.name +
		_T("<p> <b>") + _("Description:") + _T("</b> <br>") + metadata.desc +
#if SHOW_AUTHOR
		_T("<p> <b>") + _("Author:") + _T("</b> <br>") + author +
#endif
#if SHOW_VERSION
		_T("<p> <b>") + _("Version:") + _T("</b> <br>") + metadata.version +
#endif
		_T("<p> <b>") + _("Derived classes:") + _T("</b> <br>") + util::std2str(derivedDesc) +
		_T("<p> <b>") + _("Properties:") + _T("</b> <br>") + compProp;
}

//**********************************************************************

wxMenu* ComponentTreeData::s_menu = 0;

////////////////////////////////////////////////////////////////////////
// Functions for the AbstractFunction classes
//
static const String GenerateFunctionContainerSummary(const ComponentFuncEntry& entry, const String typeStr)
{
	const ArgumentVec& args = entry.GetArgumentVec();
	const std::string& retSpec = entry.GetRetType();

	std::string argSpec = "<ul>";
	if (args.empty())
		argSpec += "<i> void </i>";
	else if (args.size() == 1 && args.front().first == "...")
		argSpec += "<font color=\"#0000FF\">undefined</font>";
	else
		BOOST_FOREACH(const Argument& arg, args)
			argSpec += "<li> <font color=\"#0000FF\">" + arg.first + "</font> " + arg.second;
	argSpec += "</ul>";

	const String linkage = (entry.IsStaticFunc() ? _T("<i>[static]</i> ") : String());

	return _T("<i>") + entry.GetDocumentation() + _T("</i>") +
		_T("<p> <b>") + typeStr + _(":") + _T("</b> <br>") + linkage + util::std2str(entry.GetId()) +
		_T("<p> <b>") + _("Return type:") + _T("</b> <br>") +
		_T("<font color=\"#0000FF\">") + util::std2str(retSpec) + _T("</font>") +
		_T("<p> <b>") + _("Arguments:") + _T("</b> <br>") +
		util::std2str(argSpec);
}

static bool GenerateFunctionArgs(const ArgumentVec& argVec, Buffer *args)
{
	if (!argVec.empty()) {
		conf::PropertyTable table;
		conf::PropertyList properties;	//use to maintain correct ordering of arguments

		ArgumentVec::const_iterator iter = argVec.begin();
		for (; iter != argVec.end(); ++iter) {
			conf::Property* prop =
				ComponentDataTypeConverter::TypeToPropertyPlaceholder(iter->first);
			if (!prop)
				return false;

			const std::string cargName = iter->second.empty() ? "(unnamed)" : iter->second;
			const String argName = util::std2str(cargName);
			prop->SetLabel(argName);
			prop->SetDescription(_("Function argument: ") + argName);
			table.AddProperty(cargName, prop);
			properties.push_back(prop);
		}

		conf::DefaultGUIGenerator dgg;
		if (!dgg.ShowDialogFromProperties(0, table, conf::PropertyIdVec(), _("Function arguments")))
			return false;

		comm::encoder enc;
		conf::PropertyList::iterator propIter = properties.begin();
		ArgumentVec::const_iterator argIter = argVec.begin();
		for (; propIter != properties.end(); ++propIter, ++argIter)
			ComponentDataTypeConverter::EncodeType(enc, argIter->first, *propIter);
		*args = enc;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// AbstractFunctionTreeData
//
AbstractFunctionTreeData::~AbstractFunctionTreeData (void)
{
	//delete s_menu;
	//s_menu = 0;
}

//**********************************************************************

wxMenu*	AbstractFunctionTreeData::GetMenu (void) const
{
	if (s_menu == 0) {
		s_menu = new wxMenu;
		s_menu->Append(SWCMD_INVOKE_FUNC, _("Invoke"));
		s_menu->Append(SWCMD_REMOVE_FUNC, _("Remove"));
	}
	return s_menu;
}

//**********************************************************************

bool AbstractFunctionTreeData::DoAction (SWCommandType type) const
{
	if (type == SWCMD_INVOKE_FUNC) {
		const ArgumentVec& argVec = ComponentRegistry::Instance().GetComponentEntry(m_compId).
			GetFuncEntry(m_id).GetArgumentVec();
		MessageVec results;
		Buffer args;
		if (!GenerateFunctionArgs(argVec, &args))
			return false;
		try { ComponentFunctionCaller("ComponentSpy", m_compId, m_id).Invoke(results, args); }
		catch (...) {}
	}
	else if (type == SWCMD_REMOVE_FUNC)
		ComponentRegistry::Instance().GetComponentEntry(m_compId).RemoveFunction(m_id);
	return false;
}

//**********************************************************************

const String AbstractFunctionTreeData::GetFormattedSummary (void) const
{
	return GenerateFunctionContainerSummary(
		ComponentRegistry::Instance().GetComponentEntry(m_compId).GetFuncEntry(m_id),
		util::std2str(m_type)
	);
}

//**********************************************************************

wxMenu* AbstractFunctionTreeData::s_menu = 0;

////////////////////////////////////////////////////////////////////////
// AbstractDynamicFunctionTreeData
//
AbstractDynamicFunctionTreeData::~AbstractDynamicFunctionTreeData (void)
{
	//delete s_menu;
	//s_menu = 0;
}

//**********************************************************************

wxMenu*	AbstractDynamicFunctionTreeData::GetMenu (void) const
{
	if (s_menu == 0) {
		s_menu = new wxMenu;
		s_menu->Append(SWCMD_INVOKE_FUNC, _("Invoke"));
		s_menu->Append(SWCMD_REMOVE_FUNC, _("Remove"));
	}
	return s_menu;
}

//**********************************************************************

bool AbstractDynamicFunctionTreeData::DoAction (SWCommandType type) const
{
	if (type == SWCMD_INVOKE_FUNC) {
		Message result;
		Buffer args;
		if (!GenerateFunctionArgs(m_comp->GetDynamicFuncEntry(m_id).GetArgumentVec(), &args))
			return false;
		try { ComponentFunctionCaller("ComponentSpy", m_comp, m_id).Invoke(result, args); }
		catch (...) {}
	}
	else if (type == SWCMD_REMOVE_FUNC)
		m_comp->RemoveDynamicFunction(m_id);
	return false;
}

//**********************************************************************

const String AbstractDynamicFunctionTreeData::GetFormattedSummary (void) const
{ return GenerateFunctionContainerSummary(m_comp->GetDynamicFuncEntry(m_id), util::std2str(m_type)); }

//**********************************************************************

wxMenu* AbstractDynamicFunctionTreeData::s_menu = 0;

////////////////////////////////////////////////////////////////////////
// InstanceTreeData
//
InstanceTreeData::~InstanceTreeData (void)
{
	//delete s_menu;
	//s_menu = 0;
}

//**********************************************************************

wxMenu*	InstanceTreeData::GetMenu (void) const
{
	if (s_menu == 0) {
		s_menu = new wxMenu;
		s_menu->Append(SWCMD_DELETE_INSTANCE, _("Delete"));
	}
	return s_menu;
}

//**********************************************************************

bool InstanceTreeData::DoAction (SWCommandType type) const
{
	Component* comp = ComponentRegistry::Instance().GetComponentEntry(m_compId).
		GetInstance(m_serial);

	if (type == SWCMD_DELETE_INSTANCE)
		ComponentRegistry::Instance().GetComponentEntry(m_compId).Destroy(comp);

	return false;
}

//**********************************************************************

const String InstanceTreeData::GetFormattedSummary (void) const
{
	Component* comp = ComponentRegistry::Instance().GetComponentEntry(m_compId).
		GetInstance(m_serial);

	const String classId = util::std2str(comp->GetClassId());
	const String serial = boost::lexical_cast<String>(comp->GetSerial());
	const String children = boost::lexical_cast<String>((uint) comp->GetChildren().size());

	const String compProp = _T("<compprop comp=\"") + classId +
		_T("\" serial=\"") + serial + _T("\">");

	return _T("<b>") + String(_("Class:")) + _T("</b> <br>") + classId +
		_T("<p> <b>") + _("Serial number:") + _T("</b> <br>") + serial +
		_T("<p> <b>") + _("Children:") + _T("</b> <br>") + children +
		_T("<p> <b>") + _("Properties:") + _T("</b> <br>") + compProp;
}

//**********************************************************************

wxMenu* InstanceTreeData::s_menu = 0;

////////////////////////////////////////////////////////////////////////
// SignalTreeData
//
SignalTreeData::~SignalTreeData (void)
{
	//delete s_menu;
	//s_menu = 0;
}

//**********************************************************************

wxMenu*	SignalTreeData::GetMenu (void) const
{
	if (s_menu == 0) {
		s_menu = new wxMenu;
		s_menu->Append(SWCMD_REMOVE_SIGNAL, _("Remove"));
	}
	return s_menu;
}

//**********************************************************************

bool SignalTreeData::DoAction (SWCommandType type) const
{
	//if (type == SWCMD_REMOVE_SIGNAL)
	//	ComponentRegistry::Instance().GetComponentEntry(m_compId).RemoveSignal(m_id);
	return false;
}

//**********************************************************************

const String SignalTreeData::GetFormattedSummary (void) const
{
	const ComponentSignal& sig = ComponentSignalRegistry::Instance().GetSignal(m_id);

	const String linkage = (sig.IsStatic() ? _T("<i>[static]</i> ") : String());

	std::string memberSlots = "<ul>";
	BOOST_FOREACH(const ComponentSlot& slot, sig.GetMemberSlotMap())
		memberSlots += "<li> <font color=\"#0000FF\">" + slot.classId + "</font>." + slot.funcId;
	memberSlots += "</ul>";

	std::string staticSlots = "<ul>";
	BOOST_FOREACH(const ComponentSlot& slot, sig.GetStaticSlotList())
		staticSlots += "<li> <font color=\"#0000FF\">" + slot.classId + "</font>." + slot.funcId;
	staticSlots += "</ul>";

	std::string instanceSlots = "<ul>";
	BOOST_FOREACH(const ComponentSignal::InstanceSlotMap::value_type& pair, sig.GetInstanceSlotMap()) {
		const Handle& handle = pair.first;
		const std::string id = handle.GetClassId() + "#" + boost::lexical_cast<std::string>(handle->GetSerial());
		instanceSlots += "<li> <font color=\"#0000FF\">" + id + "</font>." + pair.second;
	}
	instanceSlots += "</ul>";

	return _T("<b>") + String(_("Signal:")) + _T("</b> <br>") + linkage + util::std2str(m_id)			+
		_T("<p> <b>") + String(_("Member slots:")) + _T("</b> <br>") + util::std2str(memberSlots)		+
		_T("<p> <b>") + _("Static slots:") + _T("</b> <br>") + util::std2str(staticSlots)				+
		_T("<p> <b>") + _("Dynamic instance slots:") + _T("</b> <br>") + util::std2str(instanceSlots)	;
}

//**********************************************************************

wxMenu* SignalTreeData::s_menu = 0;

////////////////////////////////////////////////////////////////////////
// Classes and templates for the function and handler containers
//
struct MemberFunctionTester {
	bool operator()(const ComponentFuncEntry& entry) const { return !entry.IsSlot() && entry.IsMemberFunc(); }
	const String TypeStr(void) const { return _T("member functions"); }
};

struct StaticFunctionTester {
	bool operator()(const ComponentFuncEntry& entry) const { return !entry.IsSlot() && entry.IsStaticFunc(); }
	const String TypeStr(void) const { return _T("static functions"); }
};

struct FunctionTester {
	bool operator()(const ComponentFuncEntry& entry) const { return !entry.IsSlot(); }
	const String TypeStr(void) const { return _T("functions"); }
};

//**********************************************************************

struct MemberHandlerTester {
	bool operator()(const ComponentFuncEntry& entry) const { return entry.IsSlot() && entry.IsMemberFunc(); }
	const String TypeStr(void) const { return _T("member handlers"); }
};

struct StaticHandlerTester {
	bool operator()(const ComponentFuncEntry& entry) const { return entry.IsSlot() && entry.IsStaticFunc(); }
	const String TypeStr(void) const { return _T("static handlers"); }
};

struct HandlerTester {
	bool operator()(const ComponentFuncEntry& entry) const { return entry.IsSlot(); }
	const String TypeStr(void) const { return _T("handlers"); }
};

//**********************************************************************

template<class Tester> const String GenerateFunctionContainerSummary(const std::string& compId)
{
	const Tester tester;
	const ComponentEntry::ComponentFuncMap& functions =
		ComponentRegistry::Instance().GetComponentEntry(compId).GetFunctionMap();
	uint total = 0;
	BOOST_FOREACH(const ComponentFuncEntry& entry, functions)
		if (tester(entry))
			++total;
	return _T("<b>") + String(_("Component:")) + _T("</b> <br>") +
		util::std2str(compId) +
		_T("<p> <b>") + _("Total number of ") + tester.TypeStr() + _T(":") + _T("</b> <br>") +
		boost::lexical_cast<String>(total);
}

template<class Tester> const String GenerateDynamicFunctionContainerSummary(const std::string& compId)
{
	const Tester tester;
	const ComponentEntry::InstanceList instances = 
		ComponentRegistry::Instance().GetComponentEntry(compId).GetInstances();
	
	uint instancesWithDynamicFuncs = 0;
	uint total = 0;
	BOOST_FOREACH(Component* comp, instances) {
		uint size = 0;
		BOOST_FOREACH(const ComponentFuncEntry& entry, comp->GetDynamicFunctionMap())
			if (tester(entry))
				++size;
		if (size > 0)
			++instancesWithDynamicFuncs, total += size;
	}
	const String typeStr = tester.TypeStr();
	return _T("<b>") + String(_("Component:")) + _T("</b> <br>") +
		util::std2str(compId) +
		_T("<p> <b>") + _("Instances with dynamic ") + typeStr + _T(":") + _T("</b> <br>") +
		boost::lexical_cast<String>(instancesWithDynamicFuncs) + 
		_T("<p> <b>") + _("Total dynamic ") + typeStr + _T(":") + _T("</b> <br>") +
		boost::lexical_cast<String>(total);
}

////////////////////////////////////////////////////////////////////////
// MemberFunctionsTreeData
//
const String MemberFunctionsTreeData::GetFormattedSummary (void) const
	{ return GenerateFunctionContainerSummary<MemberFunctionTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// StaticFunctionsTreeData
//
const String StaticFunctionsTreeData::GetFormattedSummary (void) const
	{ return GenerateFunctionContainerSummary<StaticFunctionTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// DynamicFunctionsTreeData
//
const String DynamicFunctionsTreeData::GetFormattedSummary (void) const
	{ return GenerateDynamicFunctionContainerSummary<FunctionTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// AllFunctionsTreeData
//
const String AllFunctionsTreeData::GetFormattedSummary (void) const
	{ return GenerateFunctionContainerSummary<FunctionTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// MemberHandlerTreeData
//
const String MemberHandlersTreeData::GetFormattedSummary (void) const
	{ return GenerateFunctionContainerSummary<MemberHandlerTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// StaticHandlerTreeData
//
const String StaticHandlersTreeData::GetFormattedSummary (void) const
	{ return GenerateFunctionContainerSummary<StaticHandlerTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// DynamicHandlerTreeData
//
const String DynamicHandlersTreeData::GetFormattedSummary (void) const
	{ return GenerateDynamicFunctionContainerSummary<HandlerTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// AllHandlerTreeData
//
const String AllHandlersTreeData::GetFormattedSummary (void) const
	{ return GenerateFunctionContainerSummary<HandlerTester>(m_compId); }

////////////////////////////////////////////////////////////////////////
// AllInstancesTreeData
//
const String AllInstancesTreeData::GetFormattedSummary (void) const
{
	ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(m_compId);
	const ComponentEntry::InstanceList::size_type total = entry.GetInstances(true).size();
	const ComponentEntry::InstanceList::size_type base = entry.GetInstances().size();
	const ComponentEntry::InstanceList::size_type derived = total - base;

	String str =	_T("<b>") + String(_("Component:")) + _T("</b> <br>") +
					util::std2str(m_compId) +
					_T("<p> <b>") + _("Total number of instances:") + _T("</b> <br>") +
					boost::lexical_cast<String>(total);
	if (derived)
		str +=	_T(" (") + boost::lexical_cast<String>(base) + _T(" base and ") +
				boost::lexical_cast<String>(derived) + _T(" derived instances)");
	return str;
}

////////////////////////////////////////////////////////////////////////
// MemberSignalsTreeData
//
const String MemberSignalsTreeData::GetFormattedSummary (void) const
{
	const ComponentEntry::SignalList& signals =
		ComponentRegistry::Instance().GetComponentEntry(m_compId).GetSignalList();
	uint total = 0;
	BOOST_FOREACH(const ComponentEntry::SignalList::value_type& signal, signals)
		if (!ComponentSignalRegistry::Instance().GetSignal(signal).IsStatic())
			++total;

	return _T("<b>") + String(_("Component:")) + _T("</b> <br>") +
		util::std2str(m_compId) +
		_T("<p> <b>") + _("Total number of member signals:") + _T("</b> <br>") +
		boost::lexical_cast<String>(total);
}

////////////////////////////////////////////////////////////////////////
// StaticSignalsTreeData
//
const String StaticSignalsTreeData::GetFormattedSummary (void) const
{
	const ComponentEntry::SignalList& signals =
		ComponentRegistry::Instance().GetComponentEntry(m_compId).GetSignalList();
	uint total = 0;
	BOOST_FOREACH(const ComponentEntry::SignalList::value_type& signal, signals)
		if (ComponentSignalRegistry::Instance().GetSignal(signal).IsStatic())
			++total;

	return _T("<b>") + String(_("Component:")) + _T("</b> <br>") +
		util::std2str(m_compId) +
		_T("<p> <b>") + _("Total number of static signals:") + _T("</b> <br>") +
		boost::lexical_cast<String>(total);
}

////////////////////////////////////////////////////////////////////////
// AllSignalsTreeData
//
const String AllSignalsTreeData::GetFormattedSummary (void) const
{
	ComponentEntry::SignalList::size_type number = ComponentRegistry::Instance().
		GetComponentEntry(m_compId).GetSignalList().size();
	const String numberStr = boost::lexical_cast<String>(number);

	return _T("<b>") + String(_("Component:")) + _T("</b> <br>") +
		util::std2str(m_compId) +
		_T("<p> <b>") + _("Total number of signals:") + _T("</b> <br>") +
		numberStr;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
