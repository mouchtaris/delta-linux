#include "DeltaWxPrintData.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(printdata, name)
#define WX_FUNC(name) WX_FUNC1(printdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getcollate)
WX_FUNC_DEF(getbin)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(getduplex)
WX_FUNC_DEF(getnocopies)
WX_FUNC_DEF(getorientation)
WX_FUNC_DEF(getpaperid)
WX_FUNC_DEF(getprintername)
WX_FUNC_DEF(getquality)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setbin)
WX_FUNC_DEF(setcollate)
WX_FUNC_DEF(setcolour)
WX_FUNC_DEF(setduplex)
WX_FUNC_DEF(setnocopies)
WX_FUNC_DEF(setorientation)
WX_FUNC_DEF(setpaperid)
WX_FUNC_DEF(setprintername)
WX_FUNC_DEF(setquality)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getcollate),
	WX_FUNC(getbin),
	WX_FUNC(getcolour),
	WX_FUNC(getduplex),
	WX_FUNC(getnocopies),
	WX_FUNC(getorientation),
	WX_FUNC(getpaperid),
	WX_FUNC(getprintername),
	WX_FUNC(getquality),
	WX_FUNC(isok),
	WX_FUNC(setbin),
	WX_FUNC(setcollate),
	WX_FUNC(setcolour),
	WX_FUNC(setduplex),
	WX_FUNC(setnocopies),
	WX_FUNC(setorientation),
	WX_FUNC(setpaperid),
	WX_FUNC(setprintername),
	WX_FUNC(setquality)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setquality")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PrintData, "printdata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PrintData, val, printdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetBin (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetBin())
	return true;
}

static bool GetMedia (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetMedia())
	return true;
}

static bool GetPrintMode (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetPrintMode())
	return true;
}

static bool GetNumberOfCopies (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetNoCopies())
	return true;
}

static bool GetPrintOrientation (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetOrientation())
	return true;
}

static bool GetPrintOrientationReversed (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETBOOL_EX(*at, data->IsOrientationReversed())
	return true;
}

static bool GetPrintCollate (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETBOOL_EX(*at, data->GetCollate())
	return true;
}

static bool GetPrinterName (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETSTRING_EX(*at, data->GetPrinterName())
	return true;
}

static bool GetColour (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETBOOL_EX(*at, data->GetColour())
	return true;
}

static bool GetDuplexMode (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetDuplex())
	return true;
}

static bool GetPrintQuality (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetQuality())
	return true;
}

static bool GetPaperId (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETNUMBER_EX(*at, data->GetPaperId())
	return true;
}

static bool GetPaperSize (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(data->GetPaperSize())));
	WX_SETOBJECT_EX(*at, Size, retval)
	return true;
}

static bool GetFilename (void* val, DeltaValue* at) 
{
	wxPrintData *data = DLIB_WXTYPECAST_BASE(PrintData, val, printdata);
	WX_SETSTRING_EX(*at, data->GetFilename())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",					&GetKeys,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",					&GetBaseClass,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bin",					&GetBin,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "media",					&GetMedia,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printMode",				&GetPrintMode,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "numberOfCopies",			&GetNumberOfCopies,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printOrientation",		&GetPrintOrientation,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printOrientationReversed",&GetPrintOrientationReversed,DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printCollate",			&GetPrintCollate,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printerName",			&GetPrinterName,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colour",					&GetColour,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "duplexMode",				&GetDuplexMode,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printQuality",			&GetPrintQuality,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "paperId",				&GetPaperId,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "paperSize",				&GetPaperSize,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filename",				&GetFilename,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PrintData, printdata);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(printdata_construct, 0, Nil)
	DeltaWxPrintData *data = DNEWCLASS(DeltaWxPrintData, (new wxPrintData()));
	WX_SETOBJECT(PrintData, data)
}

DLIB_FUNC_START(printdata_destruct, 1, Nil)
	DLIB_WXDELETE(printdata, PrintData, data)
}

DLIB_FUNC_START(printdata_getcollate, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETBOOL(data->GetCollate())
}

DLIB_FUNC_START(printdata_getbin, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETNUMBER(data->GetBin())
}

DLIB_FUNC_START(printdata_getcolour, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETBOOL(data->GetColour())
}

DLIB_FUNC_START(printdata_getduplex, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETNUMBER(data->GetDuplex())
}

DLIB_FUNC_START(printdata_getnocopies, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETNUMBER(data->GetNoCopies())
}

DLIB_FUNC_START(printdata_getorientation, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETNUMBER(data->GetOrientation())
}

DLIB_FUNC_START(printdata_getpaperid, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETNUMBER(data->GetPaperId())
}

DLIB_FUNC_START(printdata_getprintername, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETSTRING(data->GetPrinterName())
}

DLIB_FUNC_START(printdata_getquality, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETNUMBER(data->GetQuality())
}

DLIB_FUNC_START(printdata_isok, 1, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_SETBOOL(data->IsOk())
}

DLIB_FUNC_START(printdata_setbin, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETDEFINE(flag)
	data->SetBin((wxPrintBin)flag);
}

DLIB_FUNC_START(printdata_setcollate, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETBOOL(flag)
	data->SetCollate(flag);
}

DLIB_FUNC_START(printdata_setcolour, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETBOOL(flag)
	data->SetColour(flag);
}

DLIB_FUNC_START(printdata_setduplex, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETDEFINE(mode)
	data->SetDuplex((wxDuplexMode)mode);
}

DLIB_FUNC_START(printdata_setnocopies, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETNUMBER(num)
	data->SetNoCopies(num);
}

DLIB_FUNC_START(printdata_setorientation, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETDEFINE(orientation)
	data->SetOrientation(orientation);
}

DLIB_FUNC_START(printdata_setpaperid, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETDEFINE(paperId)
	data->SetPaperId((wxPaperSize)paperId);
}

DLIB_FUNC_START(printdata_setprintername, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETSTRING(printerName)
	data->SetPrinterName(printerName);
}

DLIB_FUNC_START(printdata_setquality, 2, Nil)
	DLIB_WXGET_BASE(printdata, PrintData, data)
	WX_GETDEFINE(quality)
	data->SetQuality(quality);
}
