#include "DeltaWxTreeItemData.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(treeitemdata, name)
#define WX_FUNC(name) WX_FUNC1(treeitemdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getid)
WX_FUNC_DEF(setid)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getid),
	WX_FUNC(setid)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getid", "setid")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(TreeItemData, "treeitemdata")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TreeItemData, val, treeitemdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetItemId (void* val, DeltaValue* at) 
{
	wxTreeItemData *data = DLIB_WXTYPECAST_BASE(TreeItemData, val, treeitemdata);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, TreeItemId, new wxTreeItemId(data->GetId()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "itemId",				&GetItemId,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TreeItemData,treeitemdata)

////////////////////////////////////////////////////////////////

WX_FUNC_START(treeitemdata_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemData, new wxTreeItemData())
}

WX_FUNC_START(treeitemdata_getid, 1, Nil)
	DLIB_WXGET_BASE(treeitemdata, TreeItemData, data)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(data->GetId()))
}

WX_FUNC_START(treeitemdata_setid, 2, Nil)
	DLIB_WXGET_BASE(treeitemdata, TreeItemData, data)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, id)
	data->SetId(*id);
}
