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

std::map<std::string, wxSize> defaultSizeMap;

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(size, name)
#define WX_FUNC(name) WX_FUNC1(size, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(decby)
WX_FUNC_DEF(decto)
WX_FUNC_DEF(isfullyspecified)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(incby)
WX_FUNC_DEF(incto)
WX_FUNC_DEF(scale)
WX_FUNC_DEF(set)
WX_FUNC_DEF(setdefaults)
WX_FUNC_DEF(setwidth)
WX_FUNC_DEF(setheight)
WX_FUNC_DEF(assign)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)
WX_FUNC_DEF(plus)
WX_FUNC_DEF(minus)
WX_FUNC_DEF(divide)
WX_FUNC_DEF(multiply)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(decby),
	WX_FUNC(decto),
	WX_FUNC(isfullyspecified),
	WX_FUNC(getwidth),
	WX_FUNC(getheight),
	WX_FUNC(incby),
	WX_FUNC(incto),
	WX_FUNC(scale),
	WX_FUNC(set),
	WX_FUNC(setdefaults),
	WX_FUNC(setwidth),
	WX_FUNC(setheight),
	WX_FUNC(assign),
	WX_FUNC(equal),
	WX_FUNC(notequal),
	WX_FUNC(plus),
	WX_FUNC(minus),
	WX_FUNC(divide),
	WX_FUNC(multiply)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "multiply")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(Size, "size")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Size, val, size) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxSize *size = DLIB_WXTYPECAST_BASE(Size, val, size);
	WX_SETNUMBER_EX(*at, size->x)
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxSize *size = DLIB_WXTYPECAST_BASE(Size, val, size);
	WX_SETNUMBER_EX(*at, size->y)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxSizeInitFunc()
{
	defaultSizeMap.insert(std::pair<std::string, wxSize> ("DefaultSize", wxDefaultSize));
	SizeUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("="), DeltaValue(&size_assign_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&size_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&size_notequal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("+"), DeltaValue(&size_plus_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("-"), DeltaValue(&size_minus_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("/"), DeltaValue(&size_divide_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("*"), DeltaValue(&size_multiply_LibFunc, binder));
}

void DeltaWxSizeCleanUpFunc()
{
	defaultSizeMap.clear();
}

bool DeltaWxSizeSearch(std::string str, wxSize *sz)
{
	if (str.find("wx") == 0 && str.length() > 0)
		str = str.substr(2);
	std::map<std::string, wxSize>::iterator it;
	it = defaultSizeMap.find(str);
	if (it == defaultSizeMap.end())
		return false;
	*sz = it->second;
	return true;
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Size, size,
								   DeltaWxSizeInitFunc();,
								   DeltaWxSizeCleanUpFunc(););

////////////////////////////////////////////////////////////////

#define WXSIZE_AVOID_UNNECESSARY_OBJECTS(size, func)							\
	const wxSize& size##Ref = size->func;										\
	if (&size##Ref == size) {													\
		DLIB_RETVAL_REF = DPTR(vm)->GetActualArg(0);							\
	} else {																	\
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(size##Ref)));	\
		WX_SETOBJECT(Size, retval)												\
	}

WX_FUNC_ARGRANGE_START(size_construct, 0, 2, Nil)
	wxSize *wxsize = (wxSize*) 0;
	DeltaWxSize *size = (DeltaWxSize*) 0;
	if (n == 0)
		wxsize = new wxSize();
	else if (n == 2) {
		WX_GETDEFINE(width)
		WX_GETDEFINE(height)
		wxsize = new wxSize(width, height);
	} else {
		std::string str;
		wxSize sz;
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String)
			str = DPTR(vm)->GetActualArg(_argNo++)->ToString();
		if (DeltaWxSizeSearch(str, &sz))
			wxsize = new wxSize(sz);
	}
	if (wxsize) size = DNEWCLASS(DeltaWxSize, (wxsize));
	WX_SETOBJECT(Size, size)
}

DLIB_FUNC_START(size_destruct, 1, Nil)
	DLIB_WXDELETE(size, Size, size)
}

WX_FUNC_ARGRANGE_START(size_decby, 2, 3, Nil)
	DLIB_WXGET_BASE(size, Size, curr_size)
	if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGETSIZE_BASE(wxVar)
			curr_size->DecBy(*wxVar);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(d)
			curr_size->DecBy((int)d);
		}
	} else if (n == 3) {
		WX_GETNUMBER(dx)
		WX_GETNUMBER(dy)
		curr_size->DecBy((int)dx, (int)dy);
	}
}

DLIB_FUNC_START(size_decto, 2, Nil)
	DLIB_WXGET_BASE(size, Size, curr_size)
	DLIB_WXGETSIZE_BASE(wxVar)
	curr_size->DecTo(*wxVar);
}

DLIB_FUNC_START(size_isfullyspecified, 1, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	WX_SETBOOL( wxsize->IsFullySpecified() );
}

DLIB_FUNC_START(size_getwidth, 1, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	WX_SETNUMBER( wxsize->GetWidth() );
}

DLIB_FUNC_START(size_getheight, 1, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	WX_SETNUMBER( wxsize->GetHeight() );
}

WX_FUNC_ARGRANGE_START(size_incby, 2, 3, Nil)
	DLIB_WXGET_BASE(size, Size, curr_size)
	if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGETSIZE_BASE(wxVar)
			curr_size->IncBy(*wxVar);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(d)
			curr_size->IncBy((int)d);
		}
	} else if (n == 3) {
		WX_GETNUMBER(dx)
		WX_GETNUMBER(dy)
		curr_size->IncBy((int)dx, (int)dy);
	}
}

DLIB_FUNC_START(size_incto, 2, Nil)
	DLIB_WXGET_BASE(size, Size, curr_size)
	DLIB_WXGETSIZE_BASE(wxVar2)
	curr_size->IncTo(*wxVar2);
}

DLIB_FUNC_START(size_scale, 3, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	WX_GETNUMBER(xscale)
	WX_GETNUMBER(yscale)
	WXSIZE_AVOID_UNNECESSARY_OBJECTS(size, Scale(xscale, yscale))
}

DLIB_FUNC_START(size_set, 3, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	wxsize->Set((int)width, (int)height);
}

DLIB_FUNC_START(size_setdefaults, 2, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	DLIB_WXGETSIZE_BASE(wxVar)
	wxsize->SetDefaults(*wxVar);
}

DLIB_FUNC_START(size_setwidth, 2, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	WX_GETNUMBER(width)
	wxsize->SetWidth((int)width);
}

DLIB_FUNC_START(size_setheight, 2, Nil)
	DLIB_WXGET_BASE(size, Size, wxsize)
	WX_GETNUMBER(height)
	wxsize->SetHeight((int)height);
}

DLIB_FUNC_START(size_assign, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	DLIB_WXGETSIZE_BASE(size2)
	size->operator=(*size2);
}

DLIB_FUNC_START(size_equal, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	DLIB_WXGETSIZE_BASE(size2)
	WX_SETBOOL(size->operator==(*size2))
}

DLIB_FUNC_START(size_notequal, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	DLIB_WXGETSIZE_BASE(size2)
	WX_SETBOOL(size->operator!=(*size2))
}

DLIB_FUNC_START(size_plus, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	DLIB_WXGETSIZE_BASE(size2)
	WXSIZE_AVOID_UNNECESSARY_OBJECTS(size, operator+(*size2))
}

DLIB_FUNC_START(size_minus, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	DLIB_WXGETSIZE_BASE(size2)
	WXSIZE_AVOID_UNNECESSARY_OBJECTS(size, operator-(*size2))
}

DLIB_FUNC_START(size_divide, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	WX_GETNUMBER(factor)
	WXSIZE_AVOID_UNNECESSARY_OBJECTS(size, operator/(factor))
}

DLIB_FUNC_START(size_multiply, 2, Nil)
	DLIB_WXGET_BASE(size, Size, size)
	WX_GETNUMBER(factor)
	WXSIZE_AVOID_UNNECESSARY_OBJECTS(size, operator*(factor))
}
