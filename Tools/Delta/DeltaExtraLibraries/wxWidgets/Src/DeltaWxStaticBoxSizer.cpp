#include "DeltaWxStaticBoxSizer.h"
#include "DeltaWxBoxSizer.h"
#include "DeltaWxStaticBox.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(staticboxsizer, name)
#define WX_FUNC(name) WX_FUNC1(staticboxsizer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getstaticbox)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getstaticbox)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getstaticbox")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StaticBoxSizer, "staticboxsizer", BoxSizer)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StaticBoxSizer, val, staticboxsizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxBoxSizer *_parent = DLIB_WXTYPECAST_BASE(BoxSizer, val, boxsizer);
	DeltaWxBoxSizer *parent = DNEWCLASS(DeltaWxBoxSizer, (_parent));
	WX_SETOBJECT_EX(*at, BoxSizer, parent)
	return true;
}

static bool GetStaticBox (void* val, DeltaValue* at) 
{
	wxStaticBoxSizer *sizer = DLIB_WXTYPECAST_BASE(StaticBoxSizer, val, staticboxsizer);
	wxStaticBox *staticbox = sizer->GetStaticBox();
	DeltaWxStaticBox *retval = staticbox ? DNEWCLASS(DeltaWxStaticBox, (staticbox)) : (DeltaWxStaticBox*) 0;
	WX_SETOBJECT_EX(*at, StaticBox, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "BoxSizer",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "staticBox",			&GetStaticBox,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StaticBoxSizer,staticboxsizer)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(staticboxsizer_construct, 2, 3, Nil)
	wxStaticBoxSizer *wxsizer = (wxStaticBoxSizer*) 0;
	DeltaWxStaticBoxSizer *sizer = (DeltaWxStaticBoxSizer*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(staticbox, StaticBox, statbox)
		WX_GETDEFINE(orient)
		wxsizer = new wxStaticBoxSizer(statbox, orient);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number ||
			   DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETDEFINE(orient)
		DLIB_WXGET_BASE(window, Window, parent)
		wxString label = wxEmptyString;
		if (n >= 3) { WX_GETSTRING_DEFINED(label) }
		wxsizer = new wxStaticBoxSizer(orient, parent, label);
	}
	if (wxsizer) sizer = DNEWCLASS(DeltaWxStaticBoxSizer, (wxsizer));
	WX_SETOBJECT(StaticBoxSizer, sizer)
}

DLIB_FUNC_START(staticboxsizer_destruct, 1, Nil)
	DLIB_WXDELETE(staticboxsizer, StaticBoxSizer, sizer)
}

DLIB_FUNC_START(staticboxsizer_getstaticbox, 1, Nil)
	DLIB_WXGET_BASE(staticboxsizer, StaticBoxSizer, sizer)
	DeltaWxStaticBox *retval = DNEWCLASS(DeltaWxStaticBox, (sizer->GetStaticBox()));
	WX_SETOBJECT(StaticBox, retval)
}
