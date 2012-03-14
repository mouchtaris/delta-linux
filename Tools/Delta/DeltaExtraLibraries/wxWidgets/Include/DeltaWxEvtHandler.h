#ifndef DELTAWXEVTHANDLER_H
#define DELTAWXEVTHANDLER_H
#include "wx/event.h"
#include "uvalidatable.h"
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxNotifyEvent.h"
#include "DeltaWxNotebookEvent.h"
#include "DeltaWxUpdateUIEvent.h"
#include "DeltaWxScrollEvent.h"
#include "DeltaWxScrollWinEvent.h"
#include "DeltaWxKeyEvent.h"
#include "DeltaWxDropFilesEvent.h"
#include "wx/textctrl.h"//Remove when DeltaWxTextURLEvent is ready
#include "DeltaWxSysColourChangedEvent.h"
#include "DeltaWxWizardEvent.h"
#include "DeltaWxCloseEvent.h"
#include "DeltaWxFindDialogEvent.h"
#include "DeltaWxDateEvent.h"
#include "DeltaWxCalendarEvent.h"
#include "DeltaWxListbookEvent.h"
#include "DeltaWxTreebookEvent.h"
#include "DeltaWxChoicebookEvent.h"
#include "DeltaWxTreeEvent.h"
#include "DeltaWxSpinEvent.h"
#include "DeltaWxSplitterEvent.h"
#include "DeltaWxColourPickerEvent.h"
#include "DeltaWxFileDirPickerEvent.h"
#include "DeltaWxFontPickerEvent.h"
#include "DeltaWxMenuEvent.h"
#include "DeltaWxMouseEvent.h"
#include "DeltaWxMoveEvent.h"
#include "DeltaWxSizeEvent.h"
#include "DeltaWxWindowCreateEvent.h"
#include "DeltaWxWindowDestroyEvent.h"
#include "DeltaWxCollapsiblePaneEvent.h"
#include "DeltaWxSetCursorEvent.h"
#include "DeltaWxFocusEvent.h"
#include "DeltaWxChildFocusEvent.h"
#include "DeltaWxActivateEvent.h"
#include "DeltaWxInitDialogEvent.h"
#include "DeltaWxShowEvent.h"
#include "DeltaWxIconizeEvent.h"
#include "DeltaWxMaximizeEvent.h"
#include "DeltaWxMouseCaptureChangedEvent.h"
#include "DeltaWxMouseCaptureLostEvent.h"
#include "DeltaWxDisplayChangedEvent.h"
#include "DeltaWxPaletteChangedEvent.h"
#include "DeltaWxQueryNewPaletteEvent.h"
#include "DeltaWxJoystickEvent.h"
#include "DeltaWxNavigationKeyEvent.h"
#include "DeltaWxHelpEvent.h"
#include "DeltaWxClipboardTextEvent.h"
#include "DeltaWxContextMenuEvent.h"
#include "DeltaWxIdleEvent.h"
#include "DeltaWxEraseEvent.h"
#include "DeltaWxTimerEvent.h"
#include "DeltaWxToolbookEvent.h"
#include "DeltaWxPaintEvent.h"
//
#include "DeltaWxObject.h"

class eventdata: wxObject {
public:
	DeltaValue self;
	std::list<DeltaValue> funcs;
	eventdata(DeltaValue _self, std::list<DeltaValue> _funcs): wxObject()
	{ self = _self; funcs = _funcs; }
};

#define WX_EVTFUNCTION(_class)													\
	void _class##EvtFunction(wx##_class##Event& evt)							\
	{																			\
		DeltaWx##_class##Event *e = DNEWCLASS(DeltaWx##_class##Event, (&evt));	\
		DeltaValue arg;															\
		WX_SETOBJECT_EX(arg, _class##Event, e)									\
		eventdata* data = (eventdata*)evt.m_callbackUserData;					\
		std::list<DeltaValue> args;												\
		args.push_front(data->self);											\
		args.push_front(arg);													\
		std::list<DeltaValue>::iterator it = data->funcs.begin();				\
		DASSERT(data->self.Type() == DeltaValue_ExternId);						\
		util_ui32 sn = (util_ui32)data->self.ToExternId();						\
		for (; it != data->funcs.end(); ++it) {									\
			(*it)(args);														\
			if (!VALIDATABLE_VGET_INST(sn)) {									\
				break;															\
			}																	\
		}																		\
		DDELETE(e);																\
	}

class wxEvtHandlerDerived : public wxEvtHandler
{
public:
	WX_EVTFUNCTION(Command)
	WX_EVTFUNCTION(Notify)
	WX_EVTFUNCTION(Notebook)
	WX_EVTFUNCTION(UpdateUI)
	WX_EVTFUNCTION(Scroll)
	WX_EVTFUNCTION(ScrollWin)
	WX_EVTFUNCTION(Key)
	WX_EVTFUNCTION(DropFiles)
	WX_EVTFUNCTION(SysColourChanged)
	WX_EVTFUNCTION(Wizard)
	WX_EVTFUNCTION(Close)
	WX_EVTFUNCTION(FindDialog)
	WX_EVTFUNCTION(Date)
	WX_EVTFUNCTION(Calendar)
	WX_EVTFUNCTION(Listbook)
	WX_EVTFUNCTION(Treebook)
	WX_EVTFUNCTION(Choicebook)
	WX_EVTFUNCTION(Tree)
	WX_EVTFUNCTION(Spin)
	WX_EVTFUNCTION(Splitter)
	WX_EVTFUNCTION(ColourPicker)
	WX_EVTFUNCTION(FileDirPicker)
	WX_EVTFUNCTION(FontPicker)
	WX_EVTFUNCTION(Menu)
	WX_EVTFUNCTION(Mouse)
	WX_EVTFUNCTION(Move)
	WX_EVTFUNCTION(Size)
	WX_EVTFUNCTION(WindowCreate)
	WX_EVTFUNCTION(WindowDestroy)
	WX_EVTFUNCTION(CollapsiblePane)
	WX_EVTFUNCTION(SetCursor)
	WX_EVTFUNCTION(Focus)
	WX_EVTFUNCTION(ChildFocus)
	WX_EVTFUNCTION(Activate)
	WX_EVTFUNCTION(InitDialog)
	WX_EVTFUNCTION(Show)
	WX_EVTFUNCTION(Iconize)
	WX_EVTFUNCTION(Maximize)
	WX_EVTFUNCTION(MouseCaptureChanged)
	WX_EVTFUNCTION(MouseCaptureLost)
	WX_EVTFUNCTION(DisplayChanged)
	WX_EVTFUNCTION(PaletteChanged)
	WX_EVTFUNCTION(QueryNewPalette)
	WX_EVTFUNCTION(Joystick)
	WX_EVTFUNCTION(NavigationKey)
	WX_EVTFUNCTION(Help)
	WX_EVTFUNCTION(ClipboardText)
	WX_EVTFUNCTION(ContextMenu)
	WX_EVTFUNCTION(Idle)
	WX_EVTFUNCTION(Erase)
	WX_EVTFUNCTION(Timer)
	WX_EVTFUNCTION(Toolbook)
	WX_EVTFUNCTION(Paint)

	wxEvtHandlerDerived() : wxEvtHandler() { }
};

WX_WRAPPER_DEFINITION(EvtHandler, evthandler, Object)

#endif	// Do not add stuff beyond this point.
