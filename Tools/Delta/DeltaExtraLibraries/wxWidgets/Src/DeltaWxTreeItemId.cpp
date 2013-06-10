#include "DeltaWxTreeItemId.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(treeitemid, name)
#define WX_FUNC(name) WX_FUNC1(treeitemid, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(isok),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "isok", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(TreeItemId, "treeitemid")

void DeltaWxTreeItemIdInitFunc()
{
	TreeItemIdUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&treeitemid_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&treeitemid_notequal_LibFunc, binder));
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaWxTreeItemId, "treeitemid", "wx::treeitemid_",
								  methods = DeltaObject::NativeCodeHelpers::NewObject();
								  DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);
								  DeltaWxTreeItemIdInitFunc(),
								  DELTALIBFUNC_DESTROY_METHODS_TABLE())

////////////////////////////////////////////////////////////////

WX_FUNC_START(treeitemid_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId())
}

WX_FUNC_START(treeitemid_isok, 1, Nil)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, id)
	WX_SETBOOL(id->IsOk())
}

WX_FUNC_START(treeitemid_equal, 2, Nil)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, id)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, id2)
	WX_SETBOOL((*id)==(*id2))
}

WX_FUNC_START(treeitemid_notequal, 2, Nil)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, id)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, id2)
	WX_SETBOOL((*id)!=(*id2))
}
