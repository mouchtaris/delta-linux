#include "DeltaWxPrintDialogData.h"
#include "DeltaWxObject.h"
#include "DeltaWxPrintData.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(printdialogdata, name)
#define WX_FUNC(name) WX_FUNC1(printdialogdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(enablehelp)
WX_FUNC_DEF(enablepagenumbers)
WX_FUNC_DEF(enableprinttofile)
WX_FUNC_DEF(enableselection)
WX_FUNC_DEF(getallpages)
WX_FUNC_DEF(getcollate)
WX_FUNC_DEF(getfrompage)
WX_FUNC_DEF(getmaxpage)
WX_FUNC_DEF(getminpage)
WX_FUNC_DEF(getnocopies)
WX_FUNC_DEF(getprintdata)
WX_FUNC_DEF(getprinttofile)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(gettopage)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setcollate)
WX_FUNC_DEF(setfrompage)
WX_FUNC_DEF(setmaxpage)
WX_FUNC_DEF(setminpage)
WX_FUNC_DEF(setnocopies)
WX_FUNC_DEF(setprintdata)
WX_FUNC_DEF(setprinttofile)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(settopage)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(enablehelp),
	WX_FUNC(enablepagenumbers),
	WX_FUNC(enableprinttofile),
	WX_FUNC(enableselection),
	WX_FUNC(getallpages),
	WX_FUNC(getcollate),
	WX_FUNC(getfrompage),
	WX_FUNC(getmaxpage),
	WX_FUNC(getminpage),
	WX_FUNC(getnocopies),
	WX_FUNC(getprintdata),
	WX_FUNC(getprinttofile),
	WX_FUNC(getselection),
	WX_FUNC(gettopage),
	WX_FUNC(isok),
	WX_FUNC(setcollate),
	WX_FUNC(setfrompage),
	WX_FUNC(setmaxpage),
	WX_FUNC(setminpage),
	WX_FUNC(setnocopies),
	WX_FUNC(setprintdata),
	WX_FUNC(setprinttofile),
	WX_FUNC(setselection),
	WX_FUNC(settopage)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "settopage")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PrintDialogData, "printdialogdata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetPrintFromPage (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETNUMBER_EX(*at, data->GetFromPage())
	return true;
}

static bool GetPrintToPage (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETNUMBER_EX(*at, data->GetToPage())
	return true;
}

static bool GetPrintMinPage (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETNUMBER_EX(*at, data->GetMinPage())
	return true;
}

static bool GetPrintMaxPage (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETNUMBER_EX(*at, data->GetMaxPage())
	return true;
}

static bool GetNumberOfCopies (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETNUMBER_EX(*at, data->GetNoCopies())
	return true;
}

static bool GetPrintAllPages (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetAllPages())
	return true;
}

static bool GetPrintCollate (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetCollate())
	return true;
}

static bool GetPrintToFile (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetPrintToFile())
	return true;
}

static bool GetPrintSelection (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetSelection())
	return true;
}

static bool GetPrintEnableSelection (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnableSelection())
	return true;
}

static bool GetPrintEnablePageNumbers (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnablePageNumbers())
	return true;
}

static bool GetPrintEnableHelp (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnableHelp())
	return true;
}

static bool GetPrintEnablePrintToFile (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	WX_SETBOOL_EX(*at, data->GetEnablePrintToFile())
	return true;
}

static bool GetPrintData (void* val, DeltaValue* at) 
{
	wxPrintDialogData *data = DLIB_WXTYPECAST_BASE(PrintDialogData, val, printdialogdata);
	DeltaWxPrintData *retval = DNEWCLASS(DeltaWxPrintData, (new wxPrintData(data->GetPrintData())));
	WX_SETOBJECT_EX(*at, PrintData, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",					&GetKeys,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",					&GetBaseClass,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printFromPage",			&GetPrintFromPage,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printToPage",			&GetPrintToPage,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printMinPage",			&GetPrintMinPage,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printMaxPage",			&GetPrintMaxPage,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "numberOfCopies",			&GetNumberOfCopies,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printAllPages",			&GetPrintAllPages,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printCollate",			&GetPrintCollate,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printToFile",			&GetPrintToFile,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printSelection",			&GetPrintSelection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printEnableSelection",	&GetPrintEnableSelection,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printEnablePageNumbers",	&GetPrintEnablePageNumbers,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printEnableHelp",		&GetPrintEnableHelp,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printEnablePrintToFile",	&GetPrintEnablePrintToFile,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printData",				&GetPrintData,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PrintDialogData,printdialogdata)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(printdialogdata_construct, 0, 1, Nil)
	wxPrintDialogData *wxdata = (wxPrintDialogData*) 0;
	DeltaWxPrintDialogData *data = (DeltaWxPrintDialogData*) 0;
	if (n == 0)
		wxdata = new wxPrintDialogData();
	else {
		DLIB_WXGET_BASE(printdata, PrintData, printData)
		wxdata = new wxPrintDialogData(*printData);
	}
	if (wxdata) data = DNEWCLASS(DeltaWxPrintDialogData, (wxdata));
	WX_SETOBJECT(PrintDialogData, data)
}

DLIB_FUNC_START(printdialogdata_destruct, 1, Nil)
	DLIB_WXDELETE(printdialogdata, PrintDialogData, data)
}

DLIB_FUNC_START(printdialogdata_enablehelp, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->EnableHelp(flag);
}

DLIB_FUNC_START(printdialogdata_enablepagenumbers, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->EnablePageNumbers(flag);
}

DLIB_FUNC_START(printdialogdata_enableprinttofile, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->EnablePrintToFile(flag);
}

DLIB_FUNC_START(printdialogdata_enableselection, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->EnableSelection(flag);
}

DLIB_FUNC_START(printdialogdata_getallpages, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETBOOL(data->GetAllPages())
}

DLIB_FUNC_START(printdialogdata_getcollate, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETBOOL(data->GetCollate())
}

DLIB_FUNC_START(printdialogdata_getfrompage, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETNUMBER(data->GetFromPage())
}

DLIB_FUNC_START(printdialogdata_getmaxpage, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETNUMBER(data->GetMaxPage())
}

DLIB_FUNC_START(printdialogdata_getminpage, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETNUMBER(data->GetMinPage())
}

DLIB_FUNC_START(printdialogdata_getnocopies, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETNUMBER(data->GetNoCopies())
}

DLIB_FUNC_START(printdialogdata_getprintdata, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	DeltaWxPrintData *retval = DNEWCLASS(DeltaWxPrintData, (new wxPrintData(data->GetPrintData())));
	WX_SETOBJECT(PrintData, retval)
}

DLIB_FUNC_START(printdialogdata_getprinttofile, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETBOOL(data->GetPrintToFile())
}

DLIB_FUNC_START(printdialogdata_getselection, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETBOOL(data->GetSelection())
}

DLIB_FUNC_START(printdialogdata_gettopage, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETNUMBER(data->GetToPage())
}

DLIB_FUNC_START(printdialogdata_isok, 1, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_SETBOOL(data->IsOk())
}

DLIB_FUNC_START(printdialogdata_setcollate, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->SetCollate(flag);
}

DLIB_FUNC_START(printdialogdata_setfrompage, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETNUMBER(flag)
	data->SetFromPage(flag);
}

DLIB_FUNC_START(printdialogdata_setmaxpage, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETNUMBER(flag)
	data->SetMaxPage(flag);
}

DLIB_FUNC_START(printdialogdata_setminpage, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETNUMBER(flag)
	data->SetMinPage(flag);
}

DLIB_FUNC_START(printdialogdata_setnocopies, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETNUMBER(num)
	data->SetNoCopies(num);
}

DLIB_FUNC_START(printdialogdata_setprintdata, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	DLIB_WXGET_BASE(printdata, PrintData, printData)
	data->SetPrintData(*printData);
}

DLIB_FUNC_START(printdialogdata_setprinttofile, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->SetPrintToFile(flag);
}

DLIB_FUNC_START(printdialogdata_setselection, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETBOOL(flag)
	data->SetSelection(flag);
}

DLIB_FUNC_START(printdialogdata_settopage, 2, Nil)
	DLIB_WXGET_BASE(printdialogdata, PrintDialogData, data)
	WX_GETNUMBER(page)
	data->SetToPage(page);
}
