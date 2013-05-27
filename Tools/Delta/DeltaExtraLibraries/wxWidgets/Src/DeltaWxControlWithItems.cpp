#include "DeltaWxControlWithItems.h"
#include "DeltaWxControl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(controlwithitems, name)
#define WX_FUNC(name) WX_FUNC1(controlwithitems, name)

WX_FUNC_DEF(destruct)
WX_FUNC_DEF(append)
WX_FUNC_DEF(clear)
WX_FUNC_DEF(delete)
WX_FUNC_DEF(findstring)
WX_FUNC_DEF(getcount)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getstring)
WX_FUNC_DEF(getstrings)
WX_FUNC_DEF(getstringselection)
WX_FUNC_DEF(insert)
WX_FUNC_DEF(isempty)
WX_FUNC_DEF(select)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(setstring)
WX_FUNC_DEF(setstringselection)

WX_FUNCS_START
	WX_FUNC(destruct),
	WX_FUNC(append),
	WX_FUNC(clear),
	WX_FUNC(delete),
	WX_FUNC(findstring),
	WX_FUNC(getcount),
	WX_FUNC(getselection),
	WX_FUNC(getstring),
	WX_FUNC(getstrings),
	WX_FUNC(getstringselection),
	WX_FUNC(insert),
	WX_FUNC(isempty),
	WX_FUNC(select),
	WX_FUNC(setselection),
	WX_FUNC(setstring),
	WX_FUNC(setstringselection)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "destruct", "setstringselection")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ControlWithItems, "controlwithitems", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ControlWithItems, val, controlwithitems) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ControlWithItems,controlwithitems)

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(controlwithitems_destruct, 1, Nil)
	DLIB_WXDELETE(controlwithitems, ControlWithItems, ctrl)
}

DLIB_FUNC_START(controlwithitems_append, 2, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(item)
		WX_SETNUMBER(ctrl->Append(item))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Table) {
		WX_GETTABLE(strings_array)
		int size = strings_array->Total();
		wxArrayString strings;
		for (int i = 0; i < size; ++i) {
			DeltaValue value;
			strings_array->Get(DeltaValue(DeltaNumberValueType(i)), &value);
			if (value.Type() == DeltaValue_String) {
				strings.Add(wxString(value.ToString().c_str(), wxConvUTF8));
			}
		}
		ctrl->Append(strings);
	}
}

DLIB_FUNC_START(controlwithitems_clear, 1, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	ctrl->Clear();
}

DLIB_FUNC_START(controlwithitems_delete, 2, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETNUMBER(num)
	ctrl->Delete(num);
}

WX_FUNC_ARGRANGE_START(controlwithitems_findstring, 2, 3, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETSTRING(string)
	bool bCase = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(bCase) }
	WX_SETNUMBER(ctrl->FindString(string, bCase))
}

DLIB_FUNC_START(controlwithitems_getcount, 1, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_SETNUMBER(ctrl->GetCount())
}

DLIB_FUNC_START(controlwithitems_getselection, 1, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_SETNUMBER(ctrl->GetSelection())
}

DLIB_FUNC_START(controlwithitems_getstring, 2, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETNUMBER(num)
	WX_SETSTRING(ctrl->GetString(num))
}

DLIB_FUNC_START(controlwithitems_getstrings, 1, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	wxArrayString strings = ctrl->GetStrings();
	DeltaObject *retval = DNEW(DeltaObject);
	for (int i = 0, num = (int)strings.GetCount(); i < num; ++i) {
		DeltaValue value;
		value.FromString(std::string(strings.Item(i).mb_str(wxConvUTF8)));
		retval->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	DLIB_RETVAL_REF.FromTable(retval);
}

DLIB_FUNC_START(controlwithitems_getstringselection, 1, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_SETSTRING(ctrl->GetStringSelection())
}

DLIB_FUNC_START(controlwithitems_insert, 3, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETSTRING(item)
	WX_GETNUMBER(pos)
	WX_SETNUMBER(ctrl->Insert(item, pos))
}

DLIB_FUNC_START(controlwithitems_isempty, 1, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_SETBOOL(ctrl->IsEmpty())
}

DLIB_FUNC_START(controlwithitems_select, 2, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETNUMBER(num)
	ctrl->Select(num);
}

DLIB_FUNC_START(controlwithitems_setselection, 2, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETNUMBER(num)
	ctrl->SetSelection(num);
}

DLIB_FUNC_START(controlwithitems_setstring, 3, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETNUMBER(num)
	WX_GETSTRING(string)
	ctrl->SetString(num, string);
}

DLIB_FUNC_START(controlwithitems_setstringselection, 2, Nil)
	DLIB_WXGET_BASE(controlwithitems, ControlWithItems, ctrl)
	WX_GETSTRING(string)
	WX_SETBOOL(ctrl->SetStringSelection(string))
}
