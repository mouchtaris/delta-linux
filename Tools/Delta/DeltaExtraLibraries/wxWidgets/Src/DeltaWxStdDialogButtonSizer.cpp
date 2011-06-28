#include "DeltaWxStdDialogButtonSizer.h"
#include "DeltaWxBoxSizer.h"
#include "DeltaWxButton.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "ListLibP.h"
#include "wxWrapperUtilFunctions.h"
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(stddialogbuttonsizer, name)
#define WX_FUNC(name) WX_FUNC1(stddialogbuttonsizer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addbutton)
WX_FUNC_DEF(realize)
WX_FUNC_DEF(getaffirmativebutton)
WX_FUNC_DEF(getapplybutton)
WX_FUNC_DEF(getnegativebutton)
WX_FUNC_DEF(getcancelbutton)
WX_FUNC_DEF(gethelpbutton)
WX_FUNC_DEF(setaffirmativebutton)
WX_FUNC_DEF(setcancelbutton)
WX_FUNC_DEF(setnegativebutton)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(addbutton),
	WX_FUNC(realize),
	WX_FUNC(getaffirmativebutton),
	WX_FUNC(getapplybutton),
	WX_FUNC(getnegativebutton),
	WX_FUNC(getcancelbutton),
	WX_FUNC(gethelpbutton),
	WX_FUNC(setaffirmativebutton),
	WX_FUNC(setcancelbutton),
	WX_FUNC(setnegativebutton)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setnegativebutton")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StdDialogButtonSizer, "stddialogbuttonsizer", BoxSizer)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StdDialogButtonSizer, val, stddialogbuttonsizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxBoxSizer *_parent = DLIB_WXTYPECAST_BASE(BoxSizer, val, boxsizer);
	DeltaWxBoxSizer *parent = DNEWCLASS(DeltaWxBoxSizer, (_parent));
	WX_SETOBJECT_EX(*at, BoxSizer, parent)
	return true;
}

static bool GetAffirmativeButton (void* val, DeltaValue* at) 
{
	wxStdDialogButtonSizer *sizer = DLIB_WXTYPECAST_BASE(StdDialogButtonSizer, val, stddialogbuttonsizer);
	wxButton *button = sizer->GetAffirmativeButton();
	DeltaWxButton *retval = button ? DNEWCLASS(DeltaWxButton, (button)) : (DeltaWxButton*) 0;
	WX_SETOBJECT_EX(*at, Button, retval)
	return true;
}

static bool GetApplyButton (void* val, DeltaValue* at) 
{
	wxStdDialogButtonSizer *sizer = DLIB_WXTYPECAST_BASE(StdDialogButtonSizer, val, stddialogbuttonsizer);
	wxButton *button = sizer->GetApplyButton();
	DeltaWxButton *retval = button ? DNEWCLASS(DeltaWxButton, (button)) : (DeltaWxButton*) 0;
	WX_SETOBJECT_EX(*at, Button, retval)
	return true;
}

static bool GetNegativeButton (void* val, DeltaValue* at) 
{
	wxStdDialogButtonSizer *sizer = DLIB_WXTYPECAST_BASE(StdDialogButtonSizer, val, stddialogbuttonsizer);
	wxButton *button = sizer->GetNegativeButton();
	DeltaWxButton *retval = button ? DNEWCLASS(DeltaWxButton, (button)) : (DeltaWxButton*) 0;
	WX_SETOBJECT_EX(*at, Button, retval)
	return true;
}

static bool GetCancelButton (void* val, DeltaValue* at) 
{
	wxStdDialogButtonSizer *sizer = DLIB_WXTYPECAST_BASE(StdDialogButtonSizer, val, stddialogbuttonsizer);
	wxButton *button = sizer->GetCancelButton();
	DeltaWxButton *retval = button ? DNEWCLASS(DeltaWxButton, (button)) : (DeltaWxButton*) 0;
	WX_SETOBJECT_EX(*at, Button, retval)
	return true;
}

static bool GetHelpButton (void* val, DeltaValue* at) 
{
	wxStdDialogButtonSizer *sizer = DLIB_WXTYPECAST_BASE(StdDialogButtonSizer, val, stddialogbuttonsizer);
	wxButton *button = sizer->GetHelpButton();
	DeltaWxButton *retval = button ? DNEWCLASS(DeltaWxButton, (button)) : (DeltaWxButton*) 0;
	WX_SETOBJECT_EX(*at, Button, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "BoxSizer",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "affirmativeButton",	&GetAffirmativeButton,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "applyButton",		&GetApplyButton,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "negativeButton",		&GetNegativeButton,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "cancelButton",		&GetCancelButton,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "helpButton",			&GetHelpButton,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StdDialogButtonSizer, stddialogbuttonsizer);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(stddialogbuttonsizer_construct, 0, Nil)
	DeltaWxStdDialogButtonSizer *sizer = DNEWCLASS(DeltaWxStdDialogButtonSizer,
		(new wxStdDialogButtonSizer));
	WX_SETOBJECT(StdDialogButtonSizer, sizer)
}

DLIB_FUNC_START(stddialogbuttonsizer_destruct, 1, Nil)
	DLIB_WXDELETE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
}

DLIB_FUNC_START(stddialogbuttonsizer_addbutton, 2, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DLIB_WXGET_BASE(button, Button, button)
	sizer->AddButton(button);
}

DLIB_FUNC_START(stddialogbuttonsizer_realize, 1, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	sizer->Realize();
}

DLIB_FUNC_START(stddialogbuttonsizer_getaffirmativebutton, 1, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DeltaWxButton *retval = DNEWCLASS(DeltaWxButton, (sizer->GetAffirmativeButton()));
	WX_SETOBJECT(Button, retval)
}

DLIB_FUNC_START(stddialogbuttonsizer_getapplybutton, 1, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DeltaWxButton *retval = DNEWCLASS(DeltaWxButton, (sizer->GetApplyButton()));
	WX_SETOBJECT(Button, retval)
}

DLIB_FUNC_START(stddialogbuttonsizer_getnegativebutton, 1, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DeltaWxButton *retval = DNEWCLASS(DeltaWxButton, (sizer->GetNegativeButton()));
	WX_SETOBJECT(Button, retval)
}

DLIB_FUNC_START(stddialogbuttonsizer_getcancelbutton, 1, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DeltaWxButton *retval = DNEWCLASS(DeltaWxButton, (sizer->GetCancelButton()));
	WX_SETOBJECT(Button, retval)
}

DLIB_FUNC_START(stddialogbuttonsizer_gethelpbutton, 1, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DeltaWxButton *retval = DNEWCLASS(DeltaWxButton, (sizer->GetHelpButton()));
	WX_SETOBJECT(Button, retval)
}

DLIB_FUNC_START(stddialogbuttonsizer_setaffirmativebutton, 2, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DLIB_WXGET_BASE(button, Button, button)
	sizer->SetAffirmativeButton(button);
}

DLIB_FUNC_START(stddialogbuttonsizer_setcancelbutton, 2, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DLIB_WXGET_BASE(button, Button, button)
	sizer->SetCancelButton(button);
}

DLIB_FUNC_START(stddialogbuttonsizer_setnegativebutton, 2, Nil)
	DLIB_WXGET_BASE(stddialogbuttonsizer, StdDialogButtonSizer, sizer)
	DLIB_WXGET_BASE(button, Button, button)
	sizer->SetNegativeButton(button);
}
