#include "DeltaWxPageSetupDialogData.h"
#include "DeltaWxObject.h"
#include "DeltaWxPrintData.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(pagesetupdialogdata, name)
#define WX_FUNC(name) WX_FUNC1(pagesetupdialogdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(enablehelp)
WX_FUNC_DEF(enablemargins)
WX_FUNC_DEF(enableorientation)
WX_FUNC_DEF(enablepaper)
WX_FUNC_DEF(enableprinter)
WX_FUNC_DEF(getdefaultminmargins)
WX_FUNC_DEF(getenablemargins)
WX_FUNC_DEF(getenableorientation)
WX_FUNC_DEF(getenablepaper)
WX_FUNC_DEF(getenableprinter)
WX_FUNC_DEF(getenablehelp)
WX_FUNC_DEF(getdefaultinfo)
WX_FUNC_DEF(getmargintopleft)
WX_FUNC_DEF(getmarginbottomright)
WX_FUNC_DEF(getminmargintopleft)
WX_FUNC_DEF(getminmarginbottomright)
WX_FUNC_DEF(getpaperid)
WX_FUNC_DEF(getpapersize)
WX_FUNC_DEF(getprintdata)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setdefaultinfo)
WX_FUNC_DEF(setdefaultminmargins)
WX_FUNC_DEF(setmargintopleft)
WX_FUNC_DEF(setmarginbottomright)
WX_FUNC_DEF(setminmargintopleft)
WX_FUNC_DEF(setminmarginbottomright)
WX_FUNC_DEF(setpaperid)
WX_FUNC_DEF(setpapersize)
WX_FUNC_DEF(setprintdata)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(enablehelp),
	WX_FUNC(enablemargins),
	WX_FUNC(enableorientation),
	WX_FUNC(enablepaper),
	WX_FUNC(enableprinter),
	WX_FUNC(getdefaultminmargins),
	WX_FUNC(getenablemargins),
	WX_FUNC(getenableorientation),
	WX_FUNC(getenablepaper),
	WX_FUNC(getenableprinter),
	WX_FUNC(getenablehelp),
	WX_FUNC(getdefaultinfo),
	WX_FUNC(getmargintopleft),
	WX_FUNC(getmarginbottomright),
	WX_FUNC(getminmargintopleft),
	WX_FUNC(getminmarginbottomright),
	WX_FUNC(getpaperid),
	WX_FUNC(getpapersize),
	WX_FUNC(getprintdata),
	WX_FUNC(isok),
	WX_FUNC(setdefaultinfo),
	WX_FUNC(setdefaultminmargins),
	WX_FUNC(setmargintopleft),
	WX_FUNC(setmarginbottomright),
	WX_FUNC(setminmargintopleft),
	WX_FUNC(setminmarginbottomright),
	WX_FUNC(setpaperid),
	WX_FUNC(setpapersize),
	WX_FUNC(setprintdata)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setprintdata")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PageSetupDialogData, "pagesetupdialogdata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetPaperSize (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(data->GetPaperSize())));
	WX_SETOBJECT_EX(*at, Size, retval)
	return true;
}

static bool GetMinMarginTopLeft (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMinMarginTopLeft())));
	WX_SETOBJECT_EX(*at, Point, retval)
	return true;
}

static bool GetMinMarginBottomRight (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMinMarginBottomRight())));
	WX_SETOBJECT_EX(*at, Point, retval)
	return true;
}

static bool GetMarginTopLeft (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMarginTopLeft())));
	WX_SETOBJECT_EX(*at, Point, retval)
	return true;
}

static bool GetMarginBottomRight (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMarginBottomRight())));
	WX_SETOBJECT_EX(*at, Point, retval)
	return true;
}

static bool GetDefaultMinMargins (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetDefaultMinMargins())
	return true;
}

static bool GetEnableMargins (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnableMargins())
	return true;
}

static bool GetEnableOrientation (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnableOrientation())
	return true;
}

static bool GetEnablePaper (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnablePaper())
	return true;
}

static bool GetEnablePrinter (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnablePrinter())
	return true;
}

static bool GetDefaultInfo (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetDefaultInfo())
	return true;
}

static bool GetEnableHelp (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnableHelp())
	return true;
}

static bool GetPrintData (void* val, DeltaValue* at) 
{
	wxPageSetupDialogData *data = DLIB_WXTYPECAST_BASE(PageSetupDialogData, val, pagesetupdialogdata);
	DeltaWxPrintData *retval = DNEWCLASS(DeltaWxPrintData, (new wxPrintData(data->GetPrintData())));
	WX_SETOBJECT_EX(*at, PrintData, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",					&GetKeys,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",					&GetBaseClass,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "paperSize",				&GetPaperSize,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minMarginTopLeft",		&GetMinMarginTopLeft,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minMarginBottomRight",	&GetMinMarginBottomRight,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "marginTopLeft",			&GetMarginTopLeft,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "marginBottomRight",		&GetMarginBottomRight,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultMinMargins",		&GetDefaultMinMargins,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enableMargins",			&GetEnableMargins,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enableOrientation",		&GetEnableOrientation,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enablePaper",			&GetEnablePaper,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enablePrinter",			&GetEnablePrinter,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "getDefaultInfo",			&GetDefaultInfo,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enableHelp",				&GetEnableHelp,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printData",				&GetPrintData,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PageSetupDialogData, pagesetupdialogdata);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(pagesetupdialogdata_construct, 0, 1, Nil)
	wxPageSetupDialogData *wxdata = (wxPageSetupDialogData*) 0;
	DeltaWxPageSetupDialogData *data = (DeltaWxPageSetupDialogData*) 0;
	if (n == 0)
		wxdata = new wxPageSetupDialogData();
	else {
		DLIB_WXGET_BASE(printdata, PrintData, printData)
		wxdata = new wxPageSetupDialogData(*printData);
	}
	if (wxdata) data = DNEWCLASS(DeltaWxPageSetupDialogData, (wxdata));
	WX_SETOBJECT(PageSetupDialogData, data)
}

DLIB_FUNC_START(pagesetupdialogdata_destruct, 1, Nil)
	DLIB_WXDELETE(pagesetupdialogdata, PageSetupDialogData, data)
}

DLIB_FUNC_START(pagesetupdialogdata_enablehelp, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->EnableHelp(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_enablemargins, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->EnableMargins(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_enableorientation, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->EnableOrientation(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_enablepaper, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->EnablePaper(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_enableprinter, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->EnablePrinter(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_getdefaultminmargins, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetDefaultMinMargins())
}

DLIB_FUNC_START(pagesetupdialogdata_getenablemargins, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetEnableMargins())
}

DLIB_FUNC_START(pagesetupdialogdata_getenableorientation, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetEnableOrientation())
}

DLIB_FUNC_START(pagesetupdialogdata_getenablepaper, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetEnablePaper())
}

DLIB_FUNC_START(pagesetupdialogdata_getenableprinter, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetEnablePrinter())
}

DLIB_FUNC_START(pagesetupdialogdata_getenablehelp, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetEnableHelp())
}

DLIB_FUNC_START(pagesetupdialogdata_getdefaultinfo, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->GetDefaultInfo())
}

DLIB_FUNC_START(pagesetupdialogdata_getmargintopleft, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMarginTopLeft())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(pagesetupdialogdata_getmarginbottomright, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMarginBottomRight())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(pagesetupdialogdata_getminmargintopleft, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMinMarginTopLeft())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(pagesetupdialogdata_getminmarginbottomright, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(data->GetMinMarginBottomRight())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(pagesetupdialogdata_getpaperid, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETNUMBER(data->GetPaperId())
}

DLIB_FUNC_START(pagesetupdialogdata_getpapersize, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(data->GetPaperSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(pagesetupdialogdata_getprintdata, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DeltaWxPrintData *retval = DNEWCLASS(DeltaWxPrintData, (new wxPrintData(data->GetPrintData())));
	WX_SETOBJECT(PrintData, retval)
}

DLIB_FUNC_START(pagesetupdialogdata_isok, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_SETBOOL(data->IsOk())
}

DLIB_FUNC_START(pagesetupdialogdata_setdefaultinfo, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->SetDefaultInfo(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_setdefaultminmargins, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETBOOL(flag)
	data->SetDefaultMinMargins(flag);
}

DLIB_FUNC_START(pagesetupdialogdata_setmargintopleft, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DLIB_WXGET_BASE(point, Point, pt)
	data->SetMarginTopLeft(*pt);
}

DLIB_FUNC_START(pagesetupdialogdata_setmarginbottomright, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DLIB_WXGET_BASE(point, Point, pt)
	data->SetMarginBottomRight(*pt);
}

DLIB_FUNC_START(pagesetupdialogdata_setminmargintopleft, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DLIB_WXGET_BASE(point, Point, pt)
	data->SetMinMarginTopLeft(*pt);
}

DLIB_FUNC_START(pagesetupdialogdata_setminmarginbottomright, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DLIB_WXGET_BASE(point, Point, pt)
	data->SetMinMarginBottomRight(*pt);
}

DLIB_FUNC_START(pagesetupdialogdata_setpaperid, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	WX_GETDEFINE(id)
	data->SetPaperId((wxPaperSize)id);
}

DLIB_FUNC_START(pagesetupdialogdata_setpapersize, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DLIB_WXGET_BASE(size, Size, size)
	data->SetPaperSize(*size);
}

DLIB_FUNC_START(pagesetupdialogdata_setprintdata, 2, Nil)
	DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, data)
	DLIB_WXGET_BASE(printdata, PrintData, printData)
	data->SetPrintData(*printData);
}
