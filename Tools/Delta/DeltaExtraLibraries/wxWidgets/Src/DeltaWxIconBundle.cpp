#include "DeltaWxIconBundle.h"
#include "DeltaWxIcon.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(iconbundle, name)
#define WX_FUNC(name) WX_FUNC1(iconbundle, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addicon)
WX_FUNC_DEF(geticon)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(addicon),
	WX_FUNC(geticon)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "geticon")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(IconBundle, "iconbundle")

void wxIconBundleInstallLibraryMethods()
{
	IconBundleUtils::InstallAll(methods);
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaWxIconBundle, "iconbundle", "wx::iconbundle_",
								  methods = DeltaObject::NativeCodeHelpers::NewObject();
								  DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);
								  wxIconBundleInstallLibraryMethods(),
								  DELTALIBFUNC_DESTROY_METHODS_TABLE())

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(iconbundle_construct, 0, 2, Nil)
	wxIconBundle *wxbundle = (wxIconBundle*) 0;
	DeltaWxIconBundle *bundle = (DeltaWxIconBundle*) 0;
	if (n == 0) {
		wxbundle = new wxIconBundle();
	} else if (n == 1) {
		DLIB_WXGET_BASE(icon, Icon, icon)
		wxbundle = new wxIconBundle(*icon);
	} else if (n == 2) {
		WX_GETSTRING(file)
		WX_GETDEFINE(type)
		wxbundle = new wxIconBundle(file, type);
	}
	if (wxbundle) bundle = DNEWCLASS(DeltaWxIconBundle, (wxbundle));
	WX_SETOBJECT(IconBundle, bundle)
}

DLIB_FUNC_START(iconbundle_destruct, 1, Nil)
	DLIB_WXDELETE(iconbundle, IconBundle, bundle)
}

WX_FUNC_ARGRANGE_START(iconbundle_addicon, 2, 3, Nil)
	DLIB_WXGET_BASE(iconbundle, IconBundle, bundle)
	if (n == 2) {
		DLIB_WXGET_BASE(icon, Icon, icon)
		bundle->AddIcon(*icon);
	} else if (n == 3) {
		WX_GETSTRING(file)
		WX_GETDEFINE(type)
		bundle->AddIcon(file, type);
	}
}

WX_FUNC_ARGRANGE_START(iconbundle_geticon, 1, 2, Nil)
	DLIB_WXGET_BASE(iconbundle, IconBundle, bundle)
	DeltaWxIcon *retval = (DeltaWxIcon*) 0;
	if (n == 1) {
		retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(bundle->GetIcon())));
	} else if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(size, Size, size)
			retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(bundle->GetIcon(*size))));
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String ||
				   DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETDEFINE(size)
			retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(bundle->GetIcon(size))));
		}
	}
	WX_SETOBJECT(Icon, retval)
}
