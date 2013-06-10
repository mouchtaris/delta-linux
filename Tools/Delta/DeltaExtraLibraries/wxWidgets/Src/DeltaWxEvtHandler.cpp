#include "DeltaWxEvtHandler.h"
#include "DeltaWxObject.h"
#include "DeltaWxEvent.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//
#include "wx/tglbtn.h"
#include "wx/srchctrl.h"
//
using namespace std;

map<int, wxObjectEventFunction> eventTypeFunctionMap;
map<string, pair<int, wxObjectEventFunction> > eventTypeStrFunctionMap;
void EventTypeFunctionMapInit();
void EventTypeFunctionMapExit();
wxObjectEventFunction EventTypeFunctionMapSearch(int eventType);
pair<int, wxObjectEventFunction> *EventTypeFunctionMapSearch(string eventType);

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(evthandler, name)
#define WX_FUNC(name) WX_FUNC1(evthandler, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(addpendingevent)
WX_FUNC_DEF(connect)
WX_FUNC_DEF(disconnect)
WX_FUNC_DEF(getevthandlerenabled)
WX_FUNC_DEF(getnexthandler)
WX_FUNC_DEF(getprevioushandler)
WX_FUNC_DEF(processevent)
WX_FUNC_DEF(setevthandlerenabled)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(addpendingevent),
	WX_FUNC(connect),
	WX_FUNC(disconnect),
	WX_FUNC(getevthandlerenabled),
	WX_FUNC(getnexthandler),
	WX_FUNC(getprevioushandler),
	WX_FUNC(processevent),
	WX_FUNC(setevthandlerenabled)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "addpendingevent", "setevthandlerenabled")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(EvtHandler, "evthandler", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(EvtHandler, val, evthandler) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetNextHandler (void* val, DeltaValue* at) 
{
	wxEvtHandler *handler = DLIB_WXTYPECAST_BASE(EvtHandler, val, evthandler);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, EvtHandler, handler->GetNextHandler())
	return true;
}

static bool GetPreviousHandler (void* val, DeltaValue* at) 
{
	wxEvtHandler *handler = DLIB_WXTYPECAST_BASE(EvtHandler, val, evthandler);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, EvtHandler, handler->GetPreviousHandler())
	return true;
}

static bool GetEnabled (void* val, DeltaValue* at) 
{
	wxEvtHandler *handler = DLIB_WXTYPECAST_BASE(EvtHandler, val, evthandler);
	WX_SETBOOL_EX(*at, handler->GetEvtHandlerEnabled())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "nextHandler",		&GetNextHandler,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "previousHandler",	&GetPreviousHandler,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enabled",			&GetEnabled,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(EvtHandler, evthandler,
								   wxEvtHandlerInstallLibraryMethods();
								   EventTypeFunctionMapInit();,
								   EventTypeFunctionMapExit();)

////////////////////////////////////////////////////////////////

WX_FUNC_START(evthandler_construct, 0, Nil)
	WX_SETOBJECT(EvtHandler, new wxEvtHandler())
}

WX_FUNC_START(evthandler_addpendingevent, 2, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	DLIB_WXGET_BASE(event, Event, evt)
	handler->AddPendingEvent(*evt);
}

void ConnectHelper(
	wxEvtHandler *handler, DeltaValue *self,
	int id, int eventType,
	wxObjectEventFunction objectEventFunction,
	DeltaValue* function
)
{
	if (function->IsCallable()) {
		bool foundEntry = false;
		wxList *dtable = handler->GetDynamicEventTable();
		if (dtable) {
			wxList::compatibility_iterator node = dtable->GetFirst();
			while (node) {
				wxDynamicEventTableEntry *entry = (wxDynamicEventTableEntry*) node->GetData();
				if (entry->m_id == id				&&
					entry->m_eventType == eventType	&&
					entry->m_callbackUserData)
				{
					eventdata *data = (eventdata*) entry->m_callbackUserData;
					data->funcs.push_back(function);
					foundEntry = true;
				}
				node = node->GetNext();
			}
		}
		if (!foundEntry && objectEventFunction) {
			list<DeltaValue> *funclist = new list<DeltaValue>();
			funclist->push_back(function);
			eventdata *data = new eventdata(*self, *(funclist));
			handler->Connect(id, (wxEventType)eventType, objectEventFunction,
							 (wxObject*)data);
		}
	}
}

WX_FUNC_ARGRANGE_START(evthandler_connect, 3, 4, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	int id = wxID_ANY;
	if (n > 3) { WX_GETDEFINE_DEFINED(id) }
	int eventType = 0;
	string eventStr;
	wxObjectEventFunction objectEventFunction = (wxObjectEventFunction)0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		eventType = DPTR(vm)->GetActualArg(_argNo++)->ToNumber();
		objectEventFunction = EventTypeFunctionMapSearch(eventType);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		eventStr = DPTR(vm)->GetActualArg(_argNo++)->ToString();
		pair<int, wxObjectEventFunction> *eventData = EventTypeFunctionMapSearch(eventStr);
		if (eventData) {
			eventType = eventData->first;
			objectEventFunction = eventData->second;
		}
	} else {
		DLIB_ERROR_CHECK(
			true,
			uconstructstr(
				"invalid argument passed (%s). Expected string or number!",
				DPTR(vm)->GetActualArg(_argNo++)->TypeStr()
			)
		)
	}
	DeltaValue* function = DPTR(vm)->GetActualArg(_argNo++);
	DeltaValue *self = DPTR(vm)->GetActualArg(0);

	if (eventStr == "EVT_SCROLL" || eventStr == "EVT_COMMAND_SCROLL")
	{	//Special case (wxEVT_SCROLL matches to all wxEVT_SCROLL*)
		objectEventFunction = wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_TOP, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_BOTTOM, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_LINEUP, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_LINEDOWN, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_PAGEUP, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_PAGEDOWN, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_THUMBTRACK, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_THUMBRELEASE, objectEventFunction, function);
		ConnectHelper(handler, self, id, wxEVT_SCROLL_CHANGED, objectEventFunction, function);
	}
	else
		ConnectHelper(handler, self, id, eventType, objectEventFunction, function);
}

bool DisconnectHelper(
	wxEvtHandler *handler,
	int id, int eventType,
	DeltaValue* function
)
{
	bool retval = false;
	wxList *dtable = handler->GetDynamicEventTable();
	if (dtable && function) {
		wxList::compatibility_iterator node = dtable->GetFirst();
		while (node) {
			wxDynamicEventTableEntry *entry = (wxDynamicEventTableEntry*) node->GetData();
			if (entry->m_id == id				&&
				entry->m_eventType == eventType	&&
				entry->m_callbackUserData)
			{
				eventdata *data = (eventdata*) entry->m_callbackUserData;
				list<DeltaValue>::iterator it = data->funcs.begin();
				while(it != data->funcs.end()) {
					if (it->Equal(function)) {
						it = data->funcs.erase(it);
					} else {
						++it;
					}
				}
				if (data->funcs.empty()) {
					node	= node->GetNext();
					retval	= handler->Disconnect(id, (wxEventType)eventType);
					continue;
				}
			}
			node = node->GetNext();
		}
	} else if (!function) {
		retval = handler->Disconnect(id, (wxEventType)eventType);
	}
	return retval;
}

WX_FUNC_ARGRANGE_START(evthandler_disconnect, 1, 4, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	int id = wxID_ANY;
	int eventType = wxEVT_NULL;
	string eventStr;
	DeltaValue* function = (DeltaValue*) 0;
	if (n == 1) {}
	else if (n == 2) { WX_GETDEFINE_DEFINED(id) }
	else if (n == 3) {
		if (DPTR(vm)->GetActualArg(2)->IsCallable()) {
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String)
				eventStr = DPTR(vm)->GetActualArg(_argNo)->ToString();
			WX_GETDEFINE_DEFINED(eventType)
			function = DPTR(vm)->GetActualArg(_argNo);
			++_argNo;
		} else {
			WX_GETDEFINE_DEFINED(id)
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String)
				eventStr = DPTR(vm)->GetActualArg(_argNo)->ToString();
			WX_GETDEFINE_DEFINED(eventType)
		}
	} else {
		WX_GETDEFINE_DEFINED(id)
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String)
			eventStr = DPTR(vm)->GetActualArg(_argNo)->ToString();
		WX_GETDEFINE_DEFINED(eventType)
		function = DPTR(vm)->GetActualArg(_argNo);
		++_argNo;
	}

	if (eventStr == "EVT_SCROLL" || eventStr == "EVT_COMMAND_SCROLL")
	{	//Special case (wxEVT_SCROLL matches to all wxEVT_SCROLL*)
		bool retval;
		retval =	DisconnectHelper(handler, id, wxEVT_SCROLL_TOP, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_BOTTOM, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_LINEUP, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_LINEDOWN, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_PAGEUP, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_PAGEDOWN, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_THUMBTRACK, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_THUMBRELEASE, function) &&
					DisconnectHelper(handler, id, wxEVT_SCROLL_CHANGED, function);
		WX_SETBOOL(retval);
	}
	else
		WX_SETBOOL(DisconnectHelper(handler, id, eventType, function))
}

WX_FUNC_START(evthandler_getevthandlerenabled, 1, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	handler->GetEvtHandlerEnabled();
}

WX_FUNC_START(evthandler_getnexthandler, 1, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	WX_SETOBJECT(EvtHandler, handler->GetNextHandler())
}

WX_FUNC_START(evthandler_getprevioushandler, 1, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	WX_SETOBJECT(EvtHandler, handler->GetPreviousHandler())
}

WX_FUNC_START(evthandler_processevent, 2, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETBOOL(handler->ProcessEvent(*evt))
}

WX_FUNC_START(evthandler_setevthandlerenabled, 2, Nil)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	WX_GETBOOL(enabled)
	handler->SetEvtHandlerEnabled(enabled);
}

////////////////////////////////////////////////////////////////
#define WX_SET_EVENTFUNCTION(eventType, function)											\
	eventTypeFunctionMap.insert(pair<int, wxObjectEventFunction>(eventType, function));
#define WX_SET_EVENTFUNCTION_STR(eventStr, eventType, function)												\
	eventTypeStrFunctionMap.insert(pair<string, pair<int, wxObjectEventFunction> >(eventStr,	\
	pair<int, wxObjectEventFunction>(eventType, function)));

void EventTypeFunctionMapInit()
{
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SLIDER_UPDATED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SCROLLBAR_UPDATED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_VLBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TOOL_RCLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TOOL_ENTER, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_LEFT_CLICK, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_LEFT_DCLICK, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_RIGHT_CLICK, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_RIGHT_DCLICK, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SET_FOCUS, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_KILL_FOCUS, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_ENTER, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TEXT_MAXLEN, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(wxEvtHandlerDerived::NotebookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler(wxEvtHandlerDerived::NotebookEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(wxEvtHandlerDerived::UpdateUIEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_TOP, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_TOP, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_BOTTOM, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_LINEUP, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_LINEDOWN, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_PAGEUP, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_PAGEDOWN, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SCROLLWIN_THUMBRELEASE, wxScrollWinEventHandler(wxEvtHandlerDerived::ScrollWinEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_CHAR, wxCharEventHandler(wxEvtHandlerDerived::KeyEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_KEY_DOWN, wxKeyEventHandler(wxEvtHandlerDerived::KeyEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_KEY_UP, wxKeyEventHandler(wxEvtHandlerDerived::KeyEvtFunction))
#if wxUSE_HOTKEY
	WX_SET_EVENTFUNCTION(wxEVT_HOTKEY, wxCharEventHandler(wxEvtHandlerDerived::KeyEvtFunction))
#endif
	WX_SET_EVENTFUNCTION(wxEVT_CHAR_HOOK, wxCharEventHandler(wxEvtHandlerDerived::KeyEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_DROP_FILES, wxDropFilesEventHandler(wxEvtHandlerDerived::DropFilesEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SYS_COLOUR_CHANGED, wxSysColourChangedEventHandler(wxEvtHandlerDerived::SysColourChangedEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler(wxEvtHandlerDerived::WizardEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_WIZARD_PAGE_CHANGING, wxWizardEventHandler(wxEvtHandlerDerived::WizardEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_WIZARD_CANCEL, wxWizardEventHandler(wxEvtHandlerDerived::WizardEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_WIZARD_FINISHED, wxWizardEventHandler(wxEvtHandlerDerived::WizardEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_WIZARD_HELP, wxWizardEventHandler(wxEvtHandlerDerived::WizardEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(wxEvtHandlerDerived::CloseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_END_SESSION, wxCloseEventHandler(wxEvtHandlerDerived::CloseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_QUERY_END_SESSION, wxCloseEventHandler(wxEvtHandlerDerived::CloseEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FIND, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FIND_NEXT, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FIND_REPLACE, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FIND_REPLACE_ALL, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FIND_CLOSE, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_DATE_CHANGED, wxDateEventHandler(wxEvtHandlerDerived::DateEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_CALENDAR_SEL_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_CALENDAR_DAY_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_CALENDAR_MONTH_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_CALENDAR_YEAR_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_CALENDAR_DOUBLECLICKED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_CALENDAR_WEEKDAY_CLICKED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler(wxEvtHandlerDerived::ListbookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, wxListbookEventHandler(wxEvtHandlerDerived::ListbookEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler(wxEvtHandlerDerived::ChoicebookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, wxChoicebookEventHandler(wxEvtHandlerDerived::ChoicebookEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_BEGIN_RDRAG, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_GET_INFO, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_SET_INFO, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler(wxEvtHandlerDerived::SplitterEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, wxSplitterEventHandler(wxEvtHandlerDerived::SplitterEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, wxSplitterEventHandler(wxEvtHandlerDerived::SplitterEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_SPLITTER_UNSPLIT, wxSplitterEventHandler(wxEvtHandlerDerived::SplitterEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(wxEvtHandlerDerived::ColourPickerEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(wxEvtHandlerDerived::FileDirPickerEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(wxEvtHandlerDerived::FileDirPickerEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler(wxEvtHandlerDerived::FontPickerEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_MENU_OPEN, wxMenuEventHandler(wxEvtHandlerDerived::MenuEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MENU_CLOSE, wxMenuEventHandler(wxEvtHandlerDerived::MenuEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(wxEvtHandlerDerived::MenuEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_LEFT_UP, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MIDDLE_UP, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_RIGHT_DOWN, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_RIGHT_UP, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MOTION, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_LEFT_DCLICK, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_ENTER_WINDOW, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MOUSEWHEEL, wxMouseEventHandler(wxEvtHandlerDerived::MouseEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_MOVE, wxMoveEventHandler(wxEvtHandlerDerived::MoveEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_MOVING, wxMoveEventHandler(wxEvtHandlerDerived::MoveEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SIZE, wxSizeEventHandler(wxEvtHandlerDerived::SizeEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_SIZING, wxSizeEventHandler(wxEvtHandlerDerived::SizeEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_CREATE, wxWindowCreateEventHandler(wxEvtHandlerDerived::WindowCreateEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_DESTROY, wxWindowDestroyEventHandler(wxEvtHandlerDerived::WindowDestroyEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_COLLPANE_CHANGED, wxCollapsiblePaneEventHandler(wxEvtHandlerDerived::CollapsiblePaneEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SET_CURSOR, wxSetCursorEventHandler(wxEvtHandlerDerived::SetCursorEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SET_FOCUS, wxFocusEventHandler(wxEvtHandlerDerived::FocusEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_KILL_FOCUS, wxFocusEventHandler(wxEvtHandlerDerived::FocusEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_CHILD_FOCUS, wxChildFocusEventHandler(wxEvtHandlerDerived::ChildFocusEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_ACTIVATE, wxActivateEventHandler(wxEvtHandlerDerived::ActivateEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_ACTIVATE_APP, wxActivateEventHandler(wxEvtHandlerDerived::ActivateEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_HIBERNATE, wxActivateEventHandler(wxEvtHandlerDerived::ActivateEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(wxEvtHandlerDerived::InitDialogEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_SHOW, wxShowEventHandler(wxEvtHandlerDerived::ShowEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_ICONIZE, wxIconizeEventHandler(wxEvtHandlerDerived::IconizeEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_MAXIMIZE, wxMaximizeEventHandler(wxEvtHandlerDerived::MaximizeEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_MOUSE_CAPTURE_CHANGED, wxMouseCaptureChangedEventHandler(wxEvtHandlerDerived::MouseCaptureChangedEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_MOUSE_CAPTURE_LOST, wxMouseCaptureLostEventHandler(wxEvtHandlerDerived::MouseCaptureLostEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_DISPLAY_CHANGED, wxDisplayChangedEventHandler(wxEvtHandlerDerived::DisplayChangedEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_PALETTE_CHANGED, wxPaletteChangedEventHandler(wxEvtHandlerDerived::PaletteChangedEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_QUERY_NEW_PALETTE, wxQueryNewPaletteEventHandler(wxEvtHandlerDerived::QueryNewPaletteEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_JOY_BUTTON_DOWN, wxJoystickEventHandler(wxEvtHandlerDerived::JoystickEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_JOY_BUTTON_UP, wxJoystickEventHandler(wxEvtHandlerDerived::JoystickEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_JOY_MOVE, wxJoystickEventHandler(wxEvtHandlerDerived::JoystickEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_JOY_ZMOVE, wxJoystickEventHandler(wxEvtHandlerDerived::JoystickEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_NAVIGATION_KEY, wxNavigationKeyEventHandler(wxEvtHandlerDerived::NavigationKeyEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_HELP, wxHelpEventHandler(wxEvtHandlerDerived::HelpEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_DETAILED_HELP, wxHelpEventHandler(wxEvtHandlerDerived::HelpEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TEXT_CUT, wxClipboardTextEventHandler(wxEvtHandlerDerived::ClipboardTextEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TEXT_COPY, wxClipboardTextEventHandler(wxEvtHandlerDerived::ClipboardTextEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TEXT_PASTE, wxClipboardTextEventHandler(wxEvtHandlerDerived::ClipboardTextEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(wxEvtHandlerDerived::ContextMenuEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_IDLE, wxIdleEventHandler(wxEvtHandlerDerived::IdleEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(wxEvtHandlerDerived::EraseEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_TIMER, wxTimerEventHandler(wxEvtHandlerDerived::TimerEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED, wxToolbookEventHandler(wxEvtHandlerDerived::ToolbookEvtFunction))
	WX_SET_EVENTFUNCTION(wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING, wxToolbookEventHandler(wxEvtHandlerDerived::ToolbookEvtFunction))

	WX_SET_EVENTFUNCTION(wxEVT_PAINT, wxPaintEventHandler(wxEvtHandlerDerived::PaintEvtFunction))

////////////////////////////////////////////////////////////////
	WX_SET_EVENTFUNCTION_STR("EVT_CLOSE", wxEVT_CLOSE_WINDOW, wxCloseEventHandler(wxEvtHandlerDerived::CloseEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_WINDOW_CREATE", wxEVT_CREATE, wxWindowCreateEventHandler(wxEvtHandlerDerived::WindowCreateEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_WINDOW_DESTROY", wxEVT_DESTROY, wxWindowDestroyEventHandler(wxEvtHandlerDerived::WindowDestroyEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_TOP", wxEVT_SCROLL_TOP, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_BOTTOM", wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_LINEUP", wxEVT_SCROLL_LINEUP, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_LINEDOWN", wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_PAGEUP", wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_PAGEDOWN", wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_THUMBTRACK", wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_THUMBRELEASE", wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_SCROLL_CHANGED", wxEVT_SCROLL_CHANGED, wxScrollEventHandler(wxEvtHandlerDerived::ScrollEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CHECKBOX", wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CHOICE", wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_LISTBOX", wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_LISTBOX_DCLICK", wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_MENU", wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_BUTTON", wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SLIDER", wxEVT_COMMAND_SLIDER_UPDATED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_RADIOBOX", wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_RADIOBUTTON", wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SCROLLBAR", wxEVT_COMMAND_SCROLLBAR_UPDATED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_VLBOX", wxEVT_COMMAND_VLBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMBOBOX", wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TOOL", wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TOOL_RCLICKED", wxEVT_COMMAND_TOOL_RCLICKED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TOOL_ENTER", wxEVT_COMMAND_TOOL_ENTER, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CHECKLISTBOX", wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COMMAND_CONTEXT_MENU", wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(wxEvtHandlerDerived::ContextMenuEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TEXT_CUT", wxEVT_COMMAND_TEXT_CUT, wxClipboardTextEventHandler(wxEvtHandlerDerived::ClipboardTextEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TEXT_COPY", wxEVT_COMMAND_TEXT_COPY, wxClipboardTextEventHandler(wxEvtHandlerDerived::ClipboardTextEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TEXT_PASTE", wxEVT_COMMAND_TEXT_PASTE, wxClipboardTextEventHandler(wxEvtHandlerDerived::ClipboardTextEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CALENDAR", wxEVT_CALENDAR_DOUBLECLICKED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CALENDAR_DAY", wxEVT_CALENDAR_DAY_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CALENDAR_MONTH", wxEVT_CALENDAR_MONTH_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_CALENDAR_YEAR", wxEVT_CALENDAR_YEAR_CHANGED, wxCalendarEventHandler(wxEvtHandlerDerived::CalendarEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FIND", wxEVT_COMMAND_FIND, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FIND_NEXT", wxEVT_COMMAND_FIND_NEXT, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FIND_REPLACE", wxEVT_COMMAND_FIND_REPLACE, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FIND_REPLACE_ALL", wxEVT_COMMAND_FIND_REPLACE_ALL, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FIND_CLOSE", wxEVT_COMMAND_FIND_CLOSE, wxFindDialogEventHandler(wxEvtHandlerDerived::FindDialogEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_LISTBOOK_PAGE_CHANGED", wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler(wxEvtHandlerDerived::ListbookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_LISTBOOK_PAGE_CHANGING", wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, wxListbookEventHandler(wxEvtHandlerDerived::ListbookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_NOTEBOOK_PAGE_CHANGED", wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(wxEvtHandlerDerived::NotebookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_NOTEBOOK_PAGE_CHANGING", wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler(wxEvtHandlerDerived::NotebookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SPIN_UP", wxEVT_SCROLL_LINEUP, wxSpinEventHandler(wxEvtHandlerDerived::SpinEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SPIN_DOWN", wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(wxEvtHandlerDerived::SpinEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SPIN", wxEVT_SCROLL_THUMBTRACK, wxSpinEventHandler(wxEvtHandlerDerived::SpinEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREEBOOK_PAGE_CHANGED", wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREEBOOK_PAGE_CHANGING", wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREEBOOK_NODE_COLLAPSED", wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREEBOOK_NODE_EXPANDED", wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED, wxTreebookEventHandler(wxEvtHandlerDerived::TreebookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_BEGIN_DRAG", wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_BEGIN_RDRAG", wxEVT_COMMAND_TREE_BEGIN_RDRAG, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_END_DRAG", wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_BEGIN_LABEL_EDIT", wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_END_LABEL_EDIT", wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_GET_INFO", wxEVT_COMMAND_TREE_GET_INFO, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_SET_INFO", wxEVT_COMMAND_TREE_SET_INFO, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_EXPANDED", wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_EXPANDING", wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_COLLAPSED", wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_COLLAPSING", wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_SEL_CHANGED", wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_SEL_CHANGING", wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_KEY_DOWN", wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_DELETE_ITEM", wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_ACTIVATED", wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_MENU", wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_RIGHT_CLICK", wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_MIDDLE_CLICK", wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_STATE_IMAGE_CLICK", wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TREE_ITEM_GETTOOLTIP", wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler(wxEvtHandlerDerived::TreeEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SEARCHCTRL_SEARCH_BTN", wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_SEARCHCTRL_CANCEL_BTN", wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler(wxEvtHandlerDerived::CommandEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COLOURPICKER_CHANGED", wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(wxEvtHandlerDerived::ColourPickerEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FILEPICKER_CHANGED", wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(wxEvtHandlerDerived::FileDirPickerEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_DIRPICKER_CHANGED", wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(wxEvtHandlerDerived::FileDirPickerEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_FONTPICKER_CHANGED", wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler(wxEvtHandlerDerived::FontPickerEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_MENU_HIGHLIGHT_ALL", wxEVT_MENU_HIGHLIGHT, wxMenuEventHandler(wxEvtHandlerDerived::MenuEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_COLLAPSIBLEPANE_CHANGED", wxEVT_COMMAND_COLLPANE_CHANGED, wxCollapsiblePaneEventHandler(wxEvtHandlerDerived::CollapsiblePaneEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TOOLBOOK_PAGE_CHANGED", wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED, wxToolbookEventHandler(wxEvtHandlerDerived::ToolbookEvtFunction))
	WX_SET_EVENTFUNCTION_STR("EVT_TOOLBOOK_PAGE_CHANGING", wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING, wxToolbookEventHandler(wxEvtHandlerDerived::ToolbookEvtFunction))
}

void EventTypeFunctionMapExit()
{
	eventTypeFunctionMap.clear();
}

wxObjectEventFunction EventTypeFunctionMapSearch(int eventType)
{
	map<int, wxObjectEventFunction>::iterator it;
	it = eventTypeFunctionMap.find(eventType);
	if (it != eventTypeFunctionMap.end())
		return it->second;
	return (wxObjectEventFunction)0;
}

pair<int, wxObjectEventFunction> *EventTypeFunctionMapSearch(string str)
{
	if (str.find("wx") == 0 && str.length() > 0)
		str = str.substr(2);
	map<string, pair<int, wxObjectEventFunction> >::iterator it;
	it = eventTypeStrFunctionMap.find(str);
	if (it != eventTypeStrFunctionMap.end())
		return &(it->second);
	bool found;
	int eventType = wxWidgets::DefineMapGet().Search(str, &found);
	if (found)
		return new pair<int, wxObjectEventFunction>(eventType, EventTypeFunctionMapSearch(eventType));
	return NULL;
}
