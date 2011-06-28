#include "DeltaWxDC.h"
#include "DeltaWxObject.h"
#include "DeltaWxSize.h"
#include "DeltaWxRect.h"
#include "DeltaWxPoint.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxRegion.h"
#include "DeltaWxPalette.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxPen.h"
#include "DeltaWxBrush.h"
#include "DeltaWxIcon.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dc, name)
#define WX_FUNC(name) WX_FUNC1(dc, name)

WX_FUNC_DEF(destruct)
WX_FUNC_DEF(blit)
WX_FUNC_DEF(calcboundingbox)
WX_FUNC_DEF(clear)
WX_FUNC_DEF(computescaleandorigin)
WX_FUNC_DEF(crosshair)
WX_FUNC_DEF(destroyclippingregion)
WX_FUNC_DEF(devicetologicalx)
WX_FUNC_DEF(devicetologicalxrel)
WX_FUNC_DEF(devicetologicaly)
WX_FUNC_DEF(devicetologicalyrel)
WX_FUNC_DEF(drawarc)
WX_FUNC_DEF(drawbitmap)
WX_FUNC_DEF(drawcheckmark)
WX_FUNC_DEF(drawcircle)
WX_FUNC_DEF(drawellipse)
WX_FUNC_DEF(drawellipticarc)
WX_FUNC_DEF(drawicon)
WX_FUNC_DEF(drawlabel)
WX_FUNC_DEF(drawline)
WX_FUNC_DEF(drawlines)
WX_FUNC_DEF(drawpolygon)
WX_FUNC_DEF(drawpolypolygon)
WX_FUNC_DEF(drawpoint)
WX_FUNC_DEF(drawrectangle)
WX_FUNC_DEF(drawrotatedtext)
WX_FUNC_DEF(drawroundedrectangle)
WX_FUNC_DEF(drawspline)
WX_FUNC_DEF(drawtext)
WX_FUNC_DEF(enddoc)
WX_FUNC_DEF(endpage)
WX_FUNC_DEF(floodfill)
WX_FUNC_DEF(getbackground)
WX_FUNC_DEF(getbackgroundmode)
WX_FUNC_DEF(getbrush)
WX_FUNC_DEF(getcharheight)
WX_FUNC_DEF(getcharwidth)
WX_FUNC_DEF(getclippingbox)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(getlayoutdirection)
WX_FUNC_DEF(getlogicalfunction)
WX_FUNC_DEF(getmapmode)
WX_FUNC_DEF(getmultilinetextextent)
WX_FUNC_DEF(getpartialtextextents)
WX_FUNC_DEF(getpen)
WX_FUNC_DEF(getpixel)
WX_FUNC_DEF(getppi)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(getsizemm)
WX_FUNC_DEF(gettextbackground)
WX_FUNC_DEF(gettextextent)
WX_FUNC_DEF(gettextforeground)
WX_FUNC_DEF(getuserscale)
WX_FUNC_DEF(gradientfillconcentric)
WX_FUNC_DEF(gradientfilllinear)
WX_FUNC_DEF(logicaltodevicex)
WX_FUNC_DEF(logicaltodevicexrel)
WX_FUNC_DEF(logicaltodevicey)
WX_FUNC_DEF(logicaltodeviceyrel)
WX_FUNC_DEF(maxx)
WX_FUNC_DEF(maxy)
WX_FUNC_DEF(minx)
WX_FUNC_DEF(miny)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(resetboundingbox)
WX_FUNC_DEF(setaxisorientation)
WX_FUNC_DEF(setbackground)
WX_FUNC_DEF(setbackgroundmode)
WX_FUNC_DEF(setbrush)
WX_FUNC_DEF(setclippingregion)
WX_FUNC_DEF(setdeviceorigin)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(setlayoutdirection)
WX_FUNC_DEF(setlogicalfunction)
WX_FUNC_DEF(setlogicalorigin)
WX_FUNC_DEF(setmapmode)
WX_FUNC_DEF(setpalette)
WX_FUNC_DEF(setpen)
WX_FUNC_DEF(settextbackground)
WX_FUNC_DEF(settextforeground)
WX_FUNC_DEF(setuserscale)
WX_FUNC_DEF(startdoc)
WX_FUNC_DEF(startpage)

WX_FUNCS_START
	WX_FUNC(destruct),
	WX_FUNC(blit),
	WX_FUNC(calcboundingbox),
	WX_FUNC(clear),
	WX_FUNC(computescaleandorigin),
	WX_FUNC(crosshair),
	WX_FUNC(destroyclippingregion),
	WX_FUNC(devicetologicalx),
	WX_FUNC(devicetologicalxrel),
	WX_FUNC(devicetologicaly),
	WX_FUNC(devicetologicalyrel),
	WX_FUNC(drawarc),
	WX_FUNC(drawbitmap),
	WX_FUNC(drawcheckmark),
	WX_FUNC(drawcircle),
	WX_FUNC(drawellipse),
	WX_FUNC(drawellipticarc),
	WX_FUNC(drawicon),
	WX_FUNC(drawlabel),
	WX_FUNC(drawline),
	WX_FUNC(drawlines),
	WX_FUNC(drawpolygon),
	WX_FUNC(drawpolypolygon),
	WX_FUNC(drawpoint),
	WX_FUNC(drawrectangle),
	WX_FUNC(drawrotatedtext),
	WX_FUNC(drawroundedrectangle),
	WX_FUNC(drawspline),
	WX_FUNC(drawtext),
	WX_FUNC(enddoc),
	WX_FUNC(endpage),
	WX_FUNC(floodfill),
	WX_FUNC(getbackground),
	WX_FUNC(getbackgroundmode),
	WX_FUNC(getbrush),
	WX_FUNC(getcharheight),
	WX_FUNC(getcharwidth),
	WX_FUNC(getclippingbox),
	WX_FUNC(getfont),
	WX_FUNC(getlayoutdirection),
	WX_FUNC(getlogicalfunction),
	WX_FUNC(getmapmode),
	WX_FUNC(getmultilinetextextent),
	WX_FUNC(getpartialtextextents),
	WX_FUNC(getpen),
	WX_FUNC(getpixel),
	WX_FUNC(getppi),
	WX_FUNC(getsize),
	WX_FUNC(getsizemm),
	WX_FUNC(gettextbackground),
	WX_FUNC(gettextextent),
	WX_FUNC(gettextforeground),
	WX_FUNC(getuserscale),
	WX_FUNC(gradientfillconcentric),
	WX_FUNC(gradientfilllinear),
	WX_FUNC(logicaltodevicex),
	WX_FUNC(logicaltodevicexrel),
	WX_FUNC(logicaltodevicey),
	WX_FUNC(logicaltodeviceyrel),
	WX_FUNC(maxx),
	WX_FUNC(maxy),
	WX_FUNC(minx),
	WX_FUNC(miny),
	WX_FUNC(isok),
	WX_FUNC(resetboundingbox),
	WX_FUNC(setaxisorientation),
	WX_FUNC(setbackground),
	WX_FUNC(setbackgroundmode),
	WX_FUNC(setbrush),
	WX_FUNC(setclippingregion),
	WX_FUNC(setdeviceorigin),
	WX_FUNC(setfont),
	WX_FUNC(setlayoutdirection),
	WX_FUNC(setlogicalfunction),
	WX_FUNC(setlogicalorigin),
	WX_FUNC(setmapmode),
	WX_FUNC(setpalette),
	WX_FUNC(setpen),
	WX_FUNC(settextbackground),
	WX_FUNC(settextforeground),
	WX_FUNC(setuserscale),
	WX_FUNC(startdoc),
	WX_FUNC(startpage)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "destruct", "startpage")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DC, "dc", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DC, val, dc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetOk (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETBOOL_EX(*at, dc->Ok())
	return true;
}

static bool GetLogicalOriginX (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetLogicalOrigin().x)
	return true;
}

static bool GetLogicalOriginY (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetLogicalOrigin().y)
	return true;
}

static bool GetDeviceOriginX (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetDeviceOrigin().x)
	return true;
}

static bool GetDeviceOriginY (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetDeviceOrigin().y)
	return true;
}

static bool GetLogicalScaleX (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	double x;
	dc->GetLogicalScale(&x, NULL);
	WX_SETNUMBER_EX(*at, x)
	return true;
}

static bool GetLogicalScaleY (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	double y;
	dc->GetLogicalScale(NULL, &y);
	WX_SETNUMBER_EX(*at, y)
	return true;
}

static bool GetUserScaleX (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	double x;
	dc->GetUserScale(&x, NULL);
	WX_SETNUMBER_EX(*at, x)
	return true;
}

static bool GetUserScaleY (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	double y;
	dc->GetUserScale(NULL, &y);
	WX_SETNUMBER_EX(*at, y)
	return true;
}

static bool GetMinX (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->MinX())
	return true;
}

static bool GetMinY (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->MinY())
	return true;
}

static bool GetMaxX (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->MaxX())
	return true;
}

static bool GetMaxY (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->MaxY())
	return true;
}

static bool GetClipX1 (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	int clipX1;
	dc->GetClippingBox(&clipX1, NULL, NULL, NULL);
	WX_SETNUMBER_EX(*at, clipX1)
	return true;
}

static bool GetClipY1 (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	int clipY1;
	dc->GetClippingBox(NULL, &clipY1, NULL, NULL);
	WX_SETNUMBER_EX(*at, clipY1)
	return true;
}

static bool GetClipX2 (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	int clipX1, clipX2;
	dc->GetClippingBox(&clipX1, NULL, &clipX2, NULL);
	WX_SETNUMBER_EX(*at, clipX1 + clipX2)
	return true;
}

static bool GetClipY2 (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	int clipY1, clipY2;
	dc->GetClippingBox(NULL, &clipY1, NULL, &clipY2);
	WX_SETNUMBER_EX(*at, clipY1 + clipY2)
	return true;
}

static bool GetLogicalFunction (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetLogicalFunction())
	return true;
}

static bool GetBackgroundMode (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetBackgroundMode())
	return true;
}

static bool GetMapMode (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	WX_SETNUMBER_EX(*at, dc->GetMapMode())
	return true;
}

static bool GetPen (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxPen *retval = DNEWCLASS(DeltaWxPen, (new wxPen(dc->GetPen())));
	WX_SETOBJECT_EX(*at, Pen, retval)
	return true;
}

static bool GetBrush (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxBrush *retval = DNEWCLASS(DeltaWxBrush, (new wxBrush(dc->GetBrush())));
	WX_SETOBJECT_EX(*at, Brush, retval)
	return true;
}

static bool GetBackgroundBrush (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxBrush *retval = DNEWCLASS(DeltaWxBrush, (new wxBrush(dc->GetBackground())));
	WX_SETOBJECT_EX(*at, Brush, retval)
	return true;
}

static bool GetTextForegroundColour (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(dc->GetTextForeground())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetTextBackgroundColour (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(dc->GetTextBackground())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetFont (void* val, DeltaValue* at) 
{
	wxDC *dc = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(dc->GetFont())));
	WX_SETOBJECT_EX(*at, Font, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ok",					&GetOk,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "logicalOriginX",		&GetLogicalOriginX,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "logicalOriginY",		&GetLogicalOriginY,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "deviceOriginX",		&GetDeviceOriginX,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "deviceOriginY",		&GetDeviceOriginY,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "logicalScaleX",		&GetLogicalScaleX,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "logicalScaleY",		&GetLogicalScaleY,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "userScaleX",			&GetUserScaleX,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "userScaleY",			&GetUserScaleY,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minX",				&GetMinX,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minY",				&GetMinY,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxX",				&GetMaxX,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxY",				&GetMaxY,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "clipX1",				&GetClipX1,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "clipY1",				&GetClipY1,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "clipX2",				&GetClipX2,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "clipY2",				&GetClipY2,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "logicalFunction",	&GetLogicalFunction,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundMode",		&GetBackgroundMode,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "mappingMode",		&GetMapMode,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pen",				&GetPen,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "brush",				&GetBrush,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundBrush",	&GetBackgroundBrush,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textForegroundColour",&GetTextForegroundColour,DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textBackgroundColour",&GetTextBackgroundColour,DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DC, dc);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(dc_destruct, 1, Nil)
	DLIB_WXDELETE(dc, DC, dc)
}

WX_FUNC_ARGRANGE_START(dc_blit, 5, 12, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(xdest)
		WX_GETNUMBER(ydest)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		DLIB_WXGET_BASE(dc, DC, source)
		WX_GETNUMBER(xsrc)
		WX_GETNUMBER(ysrc)
		int rop = wxCOPY, xsrcMask = wxDefaultCoord, ysrcMask = wxDefaultCoord;
		bool useMask = false;
		if (n >= 9) { WX_GETDEFINE_DEFINED(rop) }
		if (n >= 10) { WX_GETBOOL_DEFINED(useMask) }
		if (n >= 11) { WX_GETDEFINE_DEFINED(xsrcMask) }
		if (n >= 12) { WX_GETDEFINE_DEFINED(ysrcMask) }
		WX_SETBOOL(dc->Blit(xdest, ydest, width, height, (wxDC*) source, xsrc, ysrc, rop, useMask, xsrcMask, ysrcMask))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(point, Point, destPt)
		DLIB_WXGET_BASE(size, Size, sz)
		DLIB_WXGET_BASE(dc, DC, source)
		DLIB_WXGET_BASE(point, Point, srcPt)
		int rop = wxCOPY;
		bool useMask = false;
		wxPoint srcPtMask = wxDefaultPosition;
		if (n >= 6) { WX_GETDEFINE_DEFINED(rop) }
		if (n >= 7) { WX_GETBOOL_DEFINED(useMask) }
		if (n >= 8) { DLIB_WXGET_BASE(point, Point, pt) srcPtMask = *pt; }
		WX_SETBOOL(dc->Blit(*destPt, *sz, (wxDC*) source, *srcPt, rop, useMask, srcPtMask))
	}
}

DLIB_FUNC_START(dc_calcboundingbox, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	dc->CalcBoundingBox(x, y);
}

DLIB_FUNC_START(dc_clear, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->Clear();
}

DLIB_FUNC_START(dc_computescaleandorigin, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->ComputeScaleAndOrigin();
}

WX_FUNC_ARGRANGE_START(dc_crosshair, 2, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		DLIB_WXGET_BASE(point, Point, pt)
		dc->CrossHair(*pt);
	} else if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		dc->CrossHair(x, y);
	}
}

DLIB_FUNC_START(dc_destroyclippingregion, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->DestroyClippingRegion();
}

DLIB_FUNC_START(dc_devicetologicalx, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_SETNUMBER(dc->DeviceToLogicalX(x))
}

DLIB_FUNC_START(dc_devicetologicalxrel, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_SETNUMBER(dc->DeviceToLogicalXRel(x))
}

DLIB_FUNC_START(dc_devicetologicaly, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(y)
	WX_SETNUMBER(dc->DeviceToLogicalY(y))
}

DLIB_FUNC_START(dc_devicetologicalyrel, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(y)
	WX_SETNUMBER(dc->DeviceToLogicalYRel(y))
}

WX_FUNC_ARGRANGE_START(dc_drawarc, 4, 7, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 4) {
		DLIB_WXGET_BASE(point, Point, pt1)
		DLIB_WXGET_BASE(point, Point, pt2)
		DLIB_WXGET_BASE(point, Point, centre)
		dc->DrawArc(*pt1, *pt2, *centre);
	} else if (n == 7) {
		WX_GETNUMBER(x1)
		WX_GETNUMBER(y1)
		WX_GETNUMBER(x2)
		WX_GETNUMBER(y2)
		WX_GETNUMBER(xc)
		WX_GETNUMBER(yc)
		dc->DrawArc(x1, y1, x2, y2, xc, yc);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawbitmap, 3, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(point, Point, pt)
		bool useMask = false;
		if (n >= 4) { WX_GETBOOL_DEFINED(useMask) }
		dc->DrawBitmap(*bmp, *pt, useMask);
	} else if (n >= 4 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		bool useMask = false;
		if (n >= 5) { WX_GETBOOL_DEFINED(useMask) }
		dc->DrawBitmap(*bmp, x, y, useMask);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawcheckmark, 2, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		DLIB_WXGET_BASE(rect, Rect, rect)
		dc->DrawCheckMark(*rect);
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		dc->DrawCheckMark(x, y, width, height);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawcircle, 3, 4, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 3) {
		DLIB_WXGET_BASE(point, Point, pt)
		WX_GETNUMBER(radius)
		dc->DrawCircle(*pt, radius);
	} else if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(radius)
		dc->DrawCircle(x, y, radius);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawellipse, 2, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		DLIB_WXGET_BASE(rect, Rect, rect)
		dc->DrawEllipse(*rect);
	} else if (n == 3) {
		DLIB_WXGET_BASE(point, Point, pt)
		DLIB_WXGET_BASE(size, Size, size)
		dc->DrawEllipse(*pt, *size);
	} else if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		dc->DrawEllipse(x, y, width, height);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawellipticarc, 5, 7, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 5) {
		DLIB_WXGET_BASE(point, Point, pt)
		DLIB_WXGET_BASE(size, Size, sz)
		WX_GETNUMBER(sa)
		WX_GETNUMBER(ea)
		dc->DrawEllipticArc(*pt, *sz, sa, ea);
	} else if (n == 7) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_GETNUMBER(start)
		WX_GETNUMBER(end)
		dc->DrawEllipticArc(x, y, width, height, start, end);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawicon, 3, 4, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 4) {
		DLIB_WXGET_BASE(icon, Icon, icon)
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		dc->DrawIcon(*icon, x, y);
	} else if (n == 3) {
		DLIB_WXGET_BASE(icon, Icon, icon)
		DLIB_WXGET_BASE(point, Point, pt)
		dc->DrawIcon(*icon, *pt);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawlabel, 3, 7, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(text)
	util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
	if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bitmap)) {
		if (n >= 4) {
			wxBitmap *image = (wxBitmap*) bitmap->GetCastToNativeInstance();
			DLIB_WXGET_BASE(rect, Rect, rect)
			int alignment = wxALIGN_LEFT | wxALIGN_TOP, indexAccel = -1;
			wxRect rectBounding;
			if (n >= 5) { WX_GETDEFINE_DEFINED(alignment) }
			if (n >= 6) { WX_GETNUMBER_DEFINED(indexAccel) }
			dc->DrawLabel(text, *image, *rect, alignment, indexAccel, &rectBounding);
			if (n >= 7) {
				WX_GETTABLE(rect_table)
				DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(rectBounding)));
				DeltaValue value;
				WX_SETOBJECT_EX(value, Rect, retval)
				WX_SETTABLE_RETVAL(rect_table, value)
			}
		}
	} else
	if (DLIB_WXISBASE(Rect, serial_no, rect, rect_wr)) {
		wxRect *rect = (wxRect*) rect_wr->GetCastToNativeInstance();
		int alignment = wxALIGN_LEFT | wxALIGN_TOP, indexAccel = -1;
		if (n >= 4) { WX_GETDEFINE_DEFINED(alignment) }
		if (n >= 5) { WX_GETNUMBER_DEFINED(indexAccel) }
		dc->DrawLabel(text, *rect, alignment, indexAccel);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawline, 3, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 3) {
		DLIB_WXGET_BASE(point, Point, pt1)
		DLIB_WXGET_BASE(point, Point, pt2)
		dc->DrawLine(*pt1, *pt2);
	} else if (n == 5) {
		WX_GETNUMBER(x1)
		WX_GETNUMBER(y1)
		WX_GETNUMBER(x2)
		WX_GETNUMBER(y2)
		dc->DrawLine(x1, y1, x2, y2);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawlines, 3, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(points_size)
	WX_GETTABLE(points_table)
	wxPoint *points = DNEWARR(wxPoint, points_size);
	for (int i = 0; i < points_size; ++i) {
		DeltaValue value;
		points_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
		if (value.Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)value.ToExternId();
			if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
				wxPoint *point = (wxPoint*) point_wr->GetCastToNativeInstance();
				points[i] = *point;
			}
		}
	}
	int xoffset = 0, yoffset = 0;
	if (n >= 4) { WX_GETDEFINE_DEFINED(xoffset) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(yoffset) }
	dc->DrawLines(points_size, points, xoffset, yoffset);
	DDELARR(points);
}

WX_FUNC_ARGRANGE_START(dc_drawpolygon, 3, 6, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(points_size)
	WX_GETTABLE(points_table)
	wxPoint *points = DNEWARR(wxPoint, points_size);
	for (int i = 0; i < points_size; ++i) {
		DeltaValue value;
		points_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
		if (value.Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)value.ToExternId();
			if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
				wxPoint *point = (wxPoint*) point_wr->GetCastToNativeInstance();
				points[i] = *point;
			}
		}
	}
	int xoffset = 0, yoffset = 0, fillStyle = wxODDEVEN_RULE;
	if (n >= 4) { WX_GETNUMBER_DEFINED(xoffset) }
	if (n >= 5) { WX_GETNUMBER_DEFINED(yoffset) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(fillStyle) }
	dc->DrawPolygon(points_size, points, xoffset, yoffset, fillStyle);
	DDELARR(points);
}

WX_FUNC_ARGRANGE_START(dc_drawpolypolygon, 4, 7, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(size)
	WX_GETTABLE(count_table)
	WX_GETTABLE(points_table)
	int *count = DNEWARR(int, count_table->Total());
	wxPoint *points = DNEWARR(wxPoint, points_table->Total());
	for (int i = 0, max = count_table->Total(); i < max; ++i) {
		DeltaValue value;
		count_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
		if (value.Type() == DeltaValue_Number) {
			count[i] = value.ToNumber();
		}
	}
	for (int i = 0, max = points_table->Total(); i < max; ++i) {
		DeltaValue value;
		points_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
		if (value.Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)value.ToExternId();
			if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
				wxPoint *point = (wxPoint*) point_wr->GetCastToNativeInstance();
				points[i] = *point;
			}
		}
	}
	int xoffset = 0, yoffset = 0, fillStyle = wxODDEVEN_RULE;
	if (n >= 4) { WX_GETNUMBER_DEFINED(xoffset) }
	if (n >= 5) { WX_GETNUMBER_DEFINED(yoffset) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(fillStyle) }
	dc->DrawPolyPolygon(size, count, points, xoffset, yoffset, fillStyle);
	DDELARR(count);
	DDELARR(points);
}

WX_FUNC_ARGRANGE_START(dc_drawpoint, 2, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		DLIB_WXGET_BASE(point, Point, point)
		dc->DrawPoint(*point);
	} else {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		dc->DrawPoint(x, y);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawrectangle, 2, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		DLIB_WXGET_BASE(rect, Rect, rect)
		dc->DrawRectangle(*rect);
	} else if (n == 3) {
		DLIB_WXGET_BASE(point, Point, point)
		DLIB_WXGET_BASE(size, Size, size)
		dc->DrawRectangle(*point, *size);
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		dc->DrawRectangle(x, y, width, height);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawrotatedtext, 4, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(text)
	if (n == 4) {
		DLIB_WXGET_BASE(point, Point, point)
		WX_GETNUMBER(angle)
		dc->DrawRotatedText(text, *point, angle);
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(angle)
		dc->DrawRotatedText(text, x, y, angle);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawroundedrectangle, 3, 6, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		DLIB_WXGET_BASE(rect, Rect, rect)
		WX_GETNUMBER(radius)
		dc->DrawRoundedRectangle(*rect, radius);
	} else if (n == 3) {
		DLIB_WXGET_BASE(point, Point, point)
		DLIB_WXGET_BASE(size, Size, size)
		WX_GETNUMBER(radius)
		dc->DrawRoundedRectangle(*point, *size, radius);
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_GETNUMBER(radius)
		dc->DrawRoundedRectangle(x, y, width, height, radius);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawspline, 3, 7, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 3) {
		WX_GETNUMBER(points_size)
		WX_GETTABLE(points_table)
		wxPoint *points = DNEWARR(wxPoint, points_size);
		for (int i = 0; i < points_size; ++i) {
			DeltaValue value;
			points_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_ExternId) {
				util_ui32 serial_no = (util_ui32)value.ToExternId();
				if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
					wxPoint *point = (wxPoint*) point_wr->GetCastToNativeInstance();
					points[i] = *point;
				}
			}
		}
		dc->DrawSpline(points_size, points);
		DDELARR(points);
	} else if (n == 7) {
		WX_GETNUMBER(x1)
		WX_GETNUMBER(y1)
		WX_GETNUMBER(x2)
		WX_GETNUMBER(y2)
		WX_GETNUMBER(x3)
		WX_GETNUMBER(y3)
		dc->DrawSpline(x1, y1, x2, y2, x3, y3);
	}
}

WX_FUNC_ARGRANGE_START(dc_drawtext, 3, 4, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(text)
	if (n == 3) {
		DLIB_WXGET_BASE(point, Point, point)
		dc->DrawText(text, *point);
	} else if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		dc->DrawText(text, x, y);
	}
}

DLIB_FUNC_START(dc_enddoc, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->EndDoc();
}

DLIB_FUNC_START(dc_endpage, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->EndPage();
}

WX_FUNC_ARGRANGE_START(dc_floodfill, 3, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(point, Point, pt)
		DLIB_WXGET_BASE(colour, Colour, col)
		int style = wxFLOOD_SURFACE;
		if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
		WX_SETBOOL(dc->FloodFill(*pt, *col, style))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		DLIB_WXGET_BASE(colour, Colour, col)
		int style = wxFLOOD_SURFACE;
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		WX_SETBOOL(dc->FloodFill(x, y, *col, style))
	}
}

DLIB_FUNC_START(dc_getbackground, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxBrush *retval = DNEWCLASS(DeltaWxBrush, (new wxBrush(dc->GetBackground())));
	WX_SETOBJECT(Brush, retval)
}

DLIB_FUNC_START(dc_getbackgroundmode, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->GetBackgroundMode())
}

DLIB_FUNC_START(dc_getbrush, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxBrush *retval = DNEWCLASS(DeltaWxBrush, (new wxBrush(dc->GetBrush())));
	WX_SETOBJECT(Brush, retval)
}

DLIB_FUNC_START(dc_getcharheight, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->GetCharHeight())
}

DLIB_FUNC_START(dc_getcharwidth, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->GetCharWidth())
}

WX_FUNC_ARGRANGE_START(dc_getclippingbox, 2, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		WX_GETTABLE(rect_table)
		wxRect rect;
		dc->GetClippingBox(rect);
		DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(rect)));
		DeltaValue value;
		WX_SETOBJECT_EX(value, Rect, retval)
		WX_SETTABLE_RETVAL(rect_table, value)
	} else if (n == 4) {
		WX_GETTABLE(x_table)
		WX_GETTABLE(y_table)
		WX_GETTABLE(w_table)
		WX_GETTABLE(h_table)
		int x, y, w, h;
		dc->GetClippingBox(&x, &y, &w, &h);
		WX_SETTABLE_RETVAL(x_table, DeltaValue((DeltaNumberValueType)x))
		WX_SETTABLE_RETVAL(y_table, DeltaValue((DeltaNumberValueType)y))
		WX_SETTABLE_RETVAL(w_table, DeltaValue((DeltaNumberValueType)w))
		WX_SETTABLE_RETVAL(h_table, DeltaValue((DeltaNumberValueType)h))
	}
}

DLIB_FUNC_START(dc_getfont, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(dc->GetFont())));
	WX_SETOBJECT(Font, retval)
}

DLIB_FUNC_START(dc_getlayoutdirection, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->GetLayoutDirection())
}

DLIB_FUNC_START(dc_getlogicalfunction, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->GetLogicalFunction())
}

DLIB_FUNC_START(dc_getmapmode, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->GetMapMode())
}

WX_FUNC_ARGRANGE_START(dc_getmultilinetextextent, 2, 6, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(string)
	if (n == 2) {
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(dc->GetMultiLineTextExtent(string))));
		WX_SETOBJECT(Size, retval)
	} else if (n >= 4) {
		int width, height, heightLine;
		wxFont font;
		dc->GetMultiLineTextExtent(string, &width, &height, &heightLine, &font);
		WX_GETTABLE(width_table)
		WX_SETTABLE_RETVAL(width_table, DeltaValue((DeltaNumberValueType)width))
		WX_GETTABLE(height_table)
		WX_SETTABLE_RETVAL(height_table, DeltaValue((DeltaNumberValueType)height))
		if (n >= 5) {
			WX_GETTABLE(heightLine_table)
			WX_SETTABLE_RETVAL(heightLine_table, DeltaValue((DeltaNumberValueType)heightLine))
		}
		if (n >= 6) {
			WX_GETTABLE(font_table)
			DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(font)));
			DeltaValue value;
			WX_SETOBJECT_EX(value, Font, retval)
			WX_SETTABLE_RETVAL(font_table, value)
		}
	}
}

DLIB_FUNC_START(dc_getpartialtextextents, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(text)
	WX_GETTABLE(widths_table)
	wxArrayInt widths;
	WX_SETBOOL(dc->GetPartialTextExtents(text, widths))
	for (int i = 0, n = (int)widths.capacity(); i < n; ++i) {
		DeltaValue value;
		value.FromNumber((DeltaNumberValueType)widths[i]);
		widths_table->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
}

DLIB_FUNC_START(dc_getpen, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxPen *retval = DNEWCLASS(DeltaWxPen, (new wxPen(dc->GetPen())));
	WX_SETOBJECT(Pen, retval)
}

WX_FUNC_ARGRANGE_START(dc_getpixel, 3, 4, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 3) {
		DLIB_WXGET_BASE(point, Point, pt)
		DLIB_WXGET_BASE(colour, Colour, col)
		WX_SETBOOL(dc->GetPixel(*pt, col))
	} else if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		DLIB_WXGET_BASE(colour, Colour, col)
		WX_SETBOOL(dc->GetPixel(x, y, col))
	}
}

DLIB_FUNC_START(dc_getppi, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(dc->GetPPI())));
	WX_SETOBJECT(Size, retval)
}

WX_FUNC_ARGRANGE_START(dc_getsize, 1, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 1) {
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(dc->GetSize())));
		WX_SETOBJECT(Size, retval)
	} else if (n == 3) {
		WX_GETTABLE(x_table)
		WX_GETTABLE(y_table)
		int x, y;
		dc->GetSize(&x, &y);
		WX_SETTABLE_RETVAL(x_table, ((DeltaNumberValueType)x))
		WX_SETTABLE_RETVAL(y_table, ((DeltaNumberValueType)y))
	}
}

WX_FUNC_ARGRANGE_START(dc_getsizemm, 1, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 1) {
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(dc->GetSizeMM())));
		WX_SETOBJECT(Size, retval)
	} else if (n == 3) {
		WX_GETTABLE(x_table)
		WX_GETTABLE(y_table)
		int x, y;
		dc->GetSizeMM(&x, &y);
		WX_SETTABLE_RETVAL(x_table, ((DeltaNumberValueType)x))
		WX_SETTABLE_RETVAL(y_table, ((DeltaNumberValueType)y))
	}
}

DLIB_FUNC_START(dc_gettextbackground, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(dc->GetTextBackground())));
	WX_SETOBJECT(Colour, retval)
}

WX_FUNC_ARGRANGE_START(dc_gettextextent, 2, 7, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(string)
	if (n == 2) {
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(dc->GetTextExtent(string))));
		WX_SETOBJECT(Size, retval)
	} else if (n == 4) {
		int x, y, descent, externalLeading;
		wxFont font;
		dc->GetTextExtent(string, &x, &y, &descent, &externalLeading, &font);
		WX_GETTABLE(x_table)
		WX_SETTABLE_RETVAL(x_table, DeltaValue((DeltaNumberValueType)x))
		WX_GETTABLE(y_table)
		WX_SETTABLE_RETVAL(y_table, DeltaValue((DeltaNumberValueType)y))
		if (n >= 5) {
			WX_GETTABLE(descent_table)
			WX_SETTABLE_RETVAL(descent_table, DeltaValue((DeltaNumberValueType)descent))
		}
		if (n >= 6) {
			WX_GETTABLE(externalLeading_table)
			WX_SETTABLE_RETVAL(externalLeading_table, DeltaValue((DeltaNumberValueType)externalLeading))
		}
		if (n >= 7) {
			WX_GETTABLE(font_table)
			DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(font)));
			DeltaValue value;
			WX_SETOBJECT_EX(value, Font, retval)
			WX_SETTABLE_RETVAL(font_table, value)
		}
	}
}

DLIB_FUNC_START(dc_gettextforeground, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(dc->GetTextForeground())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(dc_getuserscale, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	double x, y;
	dc->GetUserScale(&x, &y);
	WX_GETTABLE(x_table)
	WX_SETTABLE_RETVAL(x_table, ((DeltaNumberValueType)x))
	WX_GETTABLE(y_table)
	WX_SETTABLE_RETVAL(y_table, ((DeltaNumberValueType)y))
}

WX_FUNC_ARGRANGE_START(dc_gradientfillconcentric, 4, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(colour, Colour, initialColour)
	DLIB_WXGET_BASE(colour, Colour, destColour)
	if (n == 4) {
		dc->GradientFillConcentric(*rect, *initialColour, *destColour);
	} else {
		DLIB_WXGET_BASE(point, Point, circleCenter)
		dc->GradientFillConcentric(*rect, *initialColour, *destColour, *circleCenter);
	}
}

WX_FUNC_ARGRANGE_START(dc_gradientfilllinear, 4, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(colour, Colour, initialColour)
	DLIB_WXGET_BASE(colour, Colour, destColour)
	int nDirection = wxEAST;
	if (n >= 5) { WX_GETDEFINE_DEFINED(nDirection) }
	dc->GradientFillLinear(*rect, *initialColour, *destColour, (wxDirection)nDirection);
}

DLIB_FUNC_START(dc_logicaltodevicex, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_SETNUMBER(dc->LogicalToDeviceX(x))
}

DLIB_FUNC_START(dc_logicaltodevicexrel, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_SETNUMBER(dc->LogicalToDeviceXRel(x))
}

DLIB_FUNC_START(dc_logicaltodevicey, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(y)
	WX_SETNUMBER(dc->LogicalToDeviceY(y))
}

DLIB_FUNC_START(dc_logicaltodeviceyrel, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(y)
	WX_SETNUMBER(dc->LogicalToDeviceYRel(y))
}

DLIB_FUNC_START(dc_maxx, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->MaxX())
}

DLIB_FUNC_START(dc_maxy, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->MaxY())
}

DLIB_FUNC_START(dc_minx, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->MinX())
}

DLIB_FUNC_START(dc_miny, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETNUMBER(dc->MinY())
}

DLIB_FUNC_START(dc_isok, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETBOOL(dc->IsOk())
}

DLIB_FUNC_START(dc_resetboundingbox, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->ResetBoundingBox();
}

DLIB_FUNC_START(dc_setaxisorientation, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETBOOL(xLeftRight)
	WX_GETBOOL(yBottomUp)
	dc->SetAxisOrientation(xLeftRight, yBottomUp);
}

DLIB_FUNC_START(dc_setbackground, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(brush, Brush, brush)
	dc->SetBackground(*brush);
}

DLIB_FUNC_START(dc_setbackgroundmode, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETDEFINE(mode)
	dc->SetBackgroundMode(mode);
}

DLIB_FUNC_START(dc_setbrush, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(brush, Brush, brush)
	dc->SetBrush(*brush);
}

WX_FUNC_ARGRANGE_START(dc_setclippingregion, 2, 5, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		dc->SetClippingRegion(x, y, width, height);
	} else if (n == 3) {
		DLIB_WXGET_BASE(point, Point, pt)
		DLIB_WXGET_BASE(size, Size, sz)
		dc->SetClippingRegion(*pt, *sz);
	} else if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Rect, serial_no, rect, rect_wr)) {
				wxRect *rect = (wxRect*) rect_wr->GetCastToNativeInstance();
				dc->SetClippingRegion(*rect);
			} else if (DLIB_WXISBASE(Region, serial_no, region, region_wr)) {
				wxRegion *region = (wxRegion*) region_wr->GetCastToNativeInstance();
				dc->SetClippingRegion(*region);
			}
		}
	}
}

DLIB_FUNC_START(dc_setdeviceorigin, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	dc->SetDeviceOrigin(x, y);
}

DLIB_FUNC_START(dc_setfont, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(font, Font, font)
	dc->SetFont(*font);
}

DLIB_FUNC_START(dc_setlayoutdirection, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETDEFINE(dir)
	dc->SetLayoutDirection((wxLayoutDirection)dir);
}

DLIB_FUNC_START(dc_setlogicalfunction, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETDEFINE(function)
	dc->SetLogicalFunction(function);
}

DLIB_FUNC_START(dc_setlogicalorigin, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	dc->SetLogicalOrigin(x, y);
}

DLIB_FUNC_START(dc_setmapmode, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETDEFINE(mode)
	dc->SetMapMode(mode);
}

DLIB_FUNC_START(dc_setpalette, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(palette, Palette, palette)
	dc->SetPalette(*palette);
}

DLIB_FUNC_START(dc_setpen, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(pen, Pen, pen)
	dc->SetPen(*pen);
}

DLIB_FUNC_START(dc_settextbackground, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(colour, Colour, colour)
	dc->SetTextBackground(*colour);
}

DLIB_FUNC_START(dc_settextforeground, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	DLIB_WXGET_BASE(colour, Colour, colour)
	dc->SetTextForeground(*colour);
}

DLIB_FUNC_START(dc_setuserscale, 3, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(xScale)
	WX_GETNUMBER(yScale)
	dc->SetUserScale(xScale, yScale);
}

DLIB_FUNC_START(dc_startdoc, 2, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETSTRING(message)
	WX_SETBOOL(dc->StartDoc(message))
}

DLIB_FUNC_START(dc_startpage, 1, Nil)
	DLIB_WXGET_BASE(dc, DC, dc)
	dc->StartPage();
}
