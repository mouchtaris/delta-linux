#include "DeltaWxGlobalFunctions.h"
#include "DeltaWxAboutDialogInfo.h"
#include "DeltaWxCursor.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxWindow.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxRect.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxIcon.h"
#include "wx/defs.h"
#include "wx/msgdlg.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/choicdlg.h"
#include "wx/numdlg.h"
#include "wx/textdlg.h"
#include "wx/log.h"
#include "wx/image.h"
#include "wx/artprov.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) static void wx_##name##_LibFunc (DeltaVirtualMachine* vm);
#define WX_FUNC(name) { #name, wx_##name##_LibFunc }

////////////////////////////////////////////////////////////////

WX_FUNC_DEF(flags)
WX_FUNC_DEF(messagebox)
WX_FUNC_DEF(beginbusycursor)
WX_FUNC_DEF(endbusycursor)
WX_FUNC_DEF(isbusy)
WX_FUNC_DEF(bell)
WX_FUNC_DEF(aboutbox)
WX_FUNC_DEF(dirselector)
WX_FUNC_DEF(fileselector)
WX_FUNC_DEF(getcolourfromuser)
WX_FUNC_DEF(getfontfromuser)
WX_FUNC_DEF(getmultiplechoices)
WX_FUNC_DEF(getnumberfromuser)
WX_FUNC_DEF(getpasswordfromuser)
WX_FUNC_DEF(gettextfromuser)
WX_FUNC_DEF(getsinglechoice)
WX_FUNC_DEF(getsinglechoiceindex)
WX_FUNC_DEF(clientdisplayrect)
WX_FUNC_DEF(colourdisplay)
WX_FUNC_DEF(displaydepth)
WX_FUNC_DEF(displaysize)
WX_FUNC_DEF(displaysizemm)
WX_FUNC_DEF(setcursor)
WX_FUNC_DEF(initallimagehandlers)
WX_FUNC_DEF(artprovider_getbitmap)
WX_FUNC_DEF(artprovider_geticon)
WX_FUNC_DEF(artprovider_getsizehint)

WX_FUNCS_START
	WX_FUNC(flags),
	WX_FUNC(messagebox),
	WX_FUNC(beginbusycursor),
	WX_FUNC(endbusycursor),
	WX_FUNC(isbusy),
	WX_FUNC(bell),
	WX_FUNC(aboutbox),
	WX_FUNC(dirselector),
	WX_FUNC(fileselector),
	WX_FUNC(getcolourfromuser),
	WX_FUNC(getfontfromuser),
	WX_FUNC(getmultiplechoices),
	WX_FUNC(getnumberfromuser),
	WX_FUNC(getpasswordfromuser),
	WX_FUNC(gettextfromuser),
	WX_FUNC(getsinglechoice),
	WX_FUNC(getsinglechoiceindex),
	WX_FUNC(clientdisplayrect),
	WX_FUNC(colourdisplay),
	WX_FUNC(displaydepth),
	WX_FUNC(displaysize),
	WX_FUNC(displaysizemm),
	WX_FUNC(setcursor),
	WX_FUNC(initallimagehandlers),
	WX_FUNC(artprovider_getbitmap),
	WX_FUNC(artprovider_geticon),
	WX_FUNC(artprovider_getsizehint)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaWxGlobalFunctions, "globalfunctions", "wx::")

////////////////////////////////////////////////////////////////

static void wx_flags_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("wx_flags");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY)
	std::string _sig1, _sig2;
	int n = DPTR(vm)->TotalActualArgs();
	int flags = 0;
	for (int i = 0; i < n; ++i) {
		WX_GETDEFINE(flag)
		flags |= flag;
	}
	WX_SETNUMBER(flags)
}

WX_FUNC_ARGRANGE_START(wx_messagebox, 1, 6, Nil)
	WX_GETSTRING(message)
	wxString caption = wxMessageBoxCaptionStr;
	long style = wxOK | wxCENTRE;
	wxWindow *parent = NULL;
	int x = wxDefaultCoord, y = wxDefaultCoord;
	if (n >= 2) { WX_GETSTRING_DEFINED(caption) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 4) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(x) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(y) }
	WX_SETNUMBER(wxMessageBox(message, caption, style, parent, x, y))
}

WX_FUNC_ARGRANGE_START(wx_beginbusycursor, 0, 1, Nil)
	wxCursor *cursor = (wxCursor *)wxHOURGLASS_CURSOR;
	if (n >= 1) { DLIB_WXGET_BASE(cursor, Cursor, _cursor) cursor = _cursor; }
	::wxBeginBusyCursor(cursor);
}

WX_FUNC_START(wx_endbusycursor, 0, Nil)
	::wxEndBusyCursor();
}

WX_FUNC_START(wx_isbusy, 0, Nil)
	WX_SETBOOL(::wxIsBusy())
}

WX_FUNC_START(wx_bell, 0, Nil)
	::wxBell();
}

WX_FUNC_START(wx_aboutbox, 1, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	::wxAboutBox(*info);
}

WX_FUNC_ARGRANGE_START(wx_dirselector, 0, 5, Nil)
	wxString message = wxDirSelectorPromptStr, defaultPath = wxEmptyString;
	long style = wxDD_DEFAULT_STYLE;
	wxPoint pos = wxDefaultPosition;
	wxWindow *parent = NULL;
	if (n >= 1) { WX_GETSTRING_DEFINED(message) }
	if (n >= 2) { WX_GETSTRING_DEFINED(defaultPath) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 4) { DLIB_WXGETPOINT_BASE(point) pos = *point; }
	if (n >= 5) { DLIB_WXGET_BASE(window, Window, win) parent = win; }
	WX_SETSTRING(::wxDirSelector(message, defaultPath, style, pos, parent))
}

#define STRING_TO_CHAR_ARRAY(str)	(const wxChar*) str.c_str()

WX_FUNC_ARGRANGE_START(wx_fileselector, 0, 9, Nil)
	const wxChar *message = (wxChar*)wxFileSelectorPromptStr, *default_path = NULL, *default_filename = NULL;
	const wxChar *default_extension = NULL, *wildcard = (wxChar*)wxFileSelectorDefaultWildcardStr;
	int flags = 0;
	wxWindow *parent = NULL;
	int x = wxDefaultCoord, y = wxDefaultCoord;
	if (n >= 1) { WX_GETSTRING(_message) message = STRING_TO_CHAR_ARRAY(_message); }
	if (n >= 2) { WX_GETSTRING(_default_path) default_path = STRING_TO_CHAR_ARRAY(_default_path); }
	if (n >= 3) { WX_GETSTRING(_default_filename) default_filename = STRING_TO_CHAR_ARRAY(_default_filename); }
	if (n >= 4) { WX_GETSTRING(_default_extension) default_extension = STRING_TO_CHAR_ARRAY(_default_extension); }
	if (n >= 5) { WX_GETSTRING(_wildcard) wildcard = STRING_TO_CHAR_ARRAY(_wildcard); }
	if (n >= 6) { WX_GETDEFINE_DEFINED(flags) }
	if (n >= 7) { DLIB_WXGET_BASE(window, Window, win) parent = win; }
	if (n >= 8) { WX_GETNUMBER_DEFINED(x) }
	if (n >= 9) { WX_GETNUMBER_DEFINED(y) }
	WX_SETSTRING(::wxFileSelector(message, default_path, default_filename, default_extension,
		wildcard, flags, parent, x, y))
}

WX_FUNC_ARGRANGE_START(wx_getcolourfromuser, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	DLIB_WXGET_BASE(colour, Colour, colInit)
	wxString caption = wxEmptyString;
	if (n >= 3) { WX_GETSTRING_DEFINED(caption) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(::wxGetColourFromUser(parent, *colInit, caption)))
}

WX_FUNC_ARGRANGE_START(wx_getfontfromuser, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	DLIB_WXGET_BASE(font, Font, fontInit)
	wxString caption = wxEmptyString;
	if (n >= 3) { WX_GETSTRING_DEFINED(caption) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(::wxGetFontFromUser(parent, *fontInit, caption)))
}

WX_FUNC_ARGRANGE_START(wx_getmultiplechoices, 4, 10, Nil)
	WX_GETTABLE(intarray)
	wxArrayInt selections;
	for (int i = 0, num = intarray->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		intarray->Get(index, &content);
		if (content.Type() == DeltaValue_Number)
			selections.Add(content.ToNumber());
	}
	WX_GETSTRING(message)
	WX_GETSTRING(caption)
	WX_GETTABLE(strarray)
	wxArrayString aChoices;
	for (int i = 0, num = strarray->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		strarray->Get(index, &content);
		if (content.Type() == DeltaValue_String)
			aChoices.Add(wxString(content.ToString().c_str(), wxConvUTF8));
	}
	wxWindow *parent = NULL;
	int x = wxDefaultCoord, y = wxDefaultCoord;
	bool centre = true;
	int width = wxCHOICE_WIDTH, height = wxCHOICE_HEIGHT;
	if (n >= 5) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 6) { WX_GETNUMBER_DEFINED(x) }
	if (n >= 7) { WX_GETNUMBER_DEFINED(y) }
	if (n >= 8) { WX_GETBOOL_DEFINED(centre) }
	if (n >= 9) { WX_GETDEFINE_DEFINED(width) }
	if (n >= 10) { WX_GETDEFINE_DEFINED(height) }
	WX_SETNUMBER(::wxGetMultipleChoices(selections, message, caption, aChoices, parent,
		x, y, centre, width, height))
}

WX_FUNC_ARGRANGE_START(wx_getnumberfromuser, 3, 8, Nil)
	WX_GETSTRING(message)
	WX_GETSTRING(prompt)
	WX_GETSTRING(caption)
	long value = 0, min = 0, max = 100;
	wxWindow *parent = (wxWindow *)NULL;
	wxPoint pos = wxDefaultPosition;
	if (n >= 4) { WX_GETNUMBER_DEFINED(value) }
	if (n >= 5) { WX_GETNUMBER_DEFINED(min) }
	if (n >= 6) { WX_GETNUMBER_DEFINED(max) }
	if (n >= 7) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 8) { DLIB_WXGETPOINT_BASE(point) pos = *point; }
	WX_SETNUMBER(::wxGetNumberFromUser(message, prompt, caption, value, min, max, parent, pos))
}

WX_FUNC_ARGRANGE_START(wx_getpasswordfromuser, 1, 7, Nil)
	WX_GETSTRING(message)
	wxString caption = wxGetPasswordFromUserPromptStr, default_value = wxEmptyString;
	wxWindow *parent = (wxWindow *) NULL;
	wxCoord x = wxDefaultCoord, y = wxDefaultCoord;
	bool centre = true;
	if (n >= 2) { WX_GETSTRING_DEFINED(caption) }
	if (n >= 3) { WX_GETSTRING_DEFINED(default_value) }
	if (n >= 4) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(x) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(y) }
	if (n >= 7) { WX_GETBOOL_DEFINED(centre) }
	WX_SETSTRING(::wxGetPasswordFromUser(message, caption, default_value, parent, x, y, centre))
}

WX_FUNC_ARGRANGE_START(wx_gettextfromuser, 1, 7, Nil)
	WX_GETSTRING(message)
	wxString caption = wxGetTextFromUserPromptStr, default_value = wxEmptyString;
	wxWindow *parent = (wxWindow *) NULL;
	wxCoord x = wxDefaultCoord, y = wxDefaultCoord;
	bool centre = true;
	if (n >= 2) { WX_GETSTRING_DEFINED(caption) }
	if (n >= 3) { WX_GETSTRING_DEFINED(default_value) }
	if (n >= 4) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(x) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(y) }
	if (n >= 7) { WX_GETBOOL_DEFINED(centre) }
	WX_SETSTRING(::wxGetTextFromUser(message, caption, default_value, parent, x, y, centre))
}

WX_FUNC_ARGRANGE_START(wx_getsinglechoice, 3, 9, Nil)
	WX_GETSTRING(message)
	WX_GETSTRING(caption)
	WX_GETTABLE(strarray)
	wxArrayString aChoices;
	for (int i = 0, num = strarray->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		strarray->Get(index, &content);
		if (content.Type() == DeltaValue_String)
			aChoices.Add(wxString(content.ToString().c_str(), wxConvUTF8));
	}
	wxWindow *parent = NULL;
	int x = wxDefaultCoord, y = wxDefaultCoord;
	bool centre = true;
	int width = wxCHOICE_WIDTH, height = wxCHOICE_HEIGHT;
	if (n >= 4) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(x) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(y) }
	if (n >= 7) { WX_GETBOOL_DEFINED(centre) }
	if (n >= 8) { WX_GETDEFINE_DEFINED(width) }
	if (n >= 9) { WX_GETDEFINE_DEFINED(height) }
	WX_SETSTRING(::wxGetSingleChoice(message, caption, aChoices, parent, x, y, centre, width, height))
}

WX_FUNC_ARGRANGE_START(wx_getsinglechoiceindex, 3, 9, Nil)
	WX_GETSTRING(message)
	WX_GETSTRING(caption)
	WX_GETTABLE(strarray)
	wxArrayString choices;
	for (int i = 0, num = strarray->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		strarray->Get(index, &content);
		if (content.Type() == DeltaValue_String)
			choices.Add(wxString(content.ToString().c_str(), wxConvUTF8));
	}
	wxWindow *parent = NULL;
	int x = wxDefaultCoord, y = wxDefaultCoord;
	bool centre = true;
	int width = wxCHOICE_WIDTH, height = wxCHOICE_HEIGHT;
	if (n >= 4) { DLIB_WXGET_BASE(window, Window, _parent) parent = _parent; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(x) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(y) }
	if (n >= 7) { WX_GETBOOL_DEFINED(centre) }
	if (n >= 8) { WX_GETDEFINE_DEFINED(width) }
	if (n >= 9) { WX_GETDEFINE_DEFINED(height) }
	WX_SETNUMBER(::wxGetSingleChoiceIndex(message, caption, choices, parent, x, y, centre, width, height))
}

WX_FUNC_START(wx_clientdisplayrect, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(::wxGetClientDisplayRect()))
}

WX_FUNC_START(wx_colourdisplay, 0, Nil)
	WX_SETBOOL(::wxColourDisplay())
}

WX_FUNC_START(wx_displaydepth, 0, Nil)
	WX_SETNUMBER(::wxDisplayDepth())
}

WX_FUNC_START(wx_displaysize, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(::wxGetDisplaySize()))
}

WX_FUNC_START(wx_displaysizemm, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(::wxGetDisplaySizeMM()))
}

WX_FUNC_START(wx_setcursor, 1, Nil)
	DLIB_WXGET_BASE(cursor, Cursor, cursor)
	::wxSetCursor(*cursor);
}

WX_FUNC_START(wx_safeshowmessage, 2, Nil)
	WX_GETSTRING(title)
	WX_GETSTRING(text)
	::wxSafeShowMessage(title, text);
}

WX_FUNC_START(wx_syserrorcode, 0, Nil)
	WX_SETNUMBER(::wxSysErrorCode())
}

WX_FUNC_ARGRANGE_START(wx_syserrormsg, 0, 1, Nil)
	unsigned long nErrCode = 0;
	if (n >= 1) { WX_GETNUMBER_DEFINED(nErrCode) }
	WX_SETSTRING(wxString(::wxSysErrorMsg(nErrCode)))
}

WX_FUNC_START(wx_initallimagehandlers, 0, Nil)
	::wxInitAllImageHandlers();
}

WX_FUNC_ARGRANGE_START(wx_artprovider_getbitmap, 1, 3, Nil)
	WX_GETSTRING(id)
	wxArtClient client = wxART_OTHER;
	wxSize size = wxDefaultSize;
	if (n >= 2) { WX_GETSTRING(_client) client = _client; }
	if (n >= 3) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (id.Find(_T("wx")) != 0 && id.Length() > 0)
		id = id.Prepend(_T("wx"));
	if (client.Find(_T("wx")) != 0 && client.Length() > 0)
		client = client.Prepend(_T("wx"));
	wxART_MAKE_CLIENT_ID_FROM_STR(client);
	WX_SETOBJECT(Bitmap, new wxBitmap(wxArtProvider::GetBitmap(id, client, size)))
}

WX_FUNC_ARGRANGE_START(wx_artprovider_geticon, 1, 3, Nil)
	WX_GETSTRING(id)
	wxArtClient client = wxART_OTHER;
	wxSize size = wxDefaultSize;
	if (n >= 2) { WX_GETSTRING(_client) client = _client; }
	if (n >= 3) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (id.Find(_T("wx")) != 0 && id.Length() > 0)
		id = id.Prepend(_T("wx"));
	if (client.Find(_T("wx")) != 0 && client.Length() > 0)
		client = client.Prepend(_T("wx"));
	wxART_MAKE_CLIENT_ID_FROM_STR(client);
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Icon, new wxIcon(wxArtProvider::GetIcon(id, client, size)))
}

WX_FUNC_ARGRANGE_START(wx_artprovider_getsizehint, 1, 2, Nil)
	WX_GETSTRING(client)
	bool platform_dependent = false;
	if (n >= 2) { WX_GETBOOL_DEFINED(platform_dependent) }
	if (client.Find(_T("wx")) != 0 && client.Length() > 0)
		client = client.Prepend(_T("wx"));
	wxART_MAKE_CLIENT_ID_FROM_STR(client);
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(wxArtProvider::GetSizeHint(client, platform_dependent)))
}
