// RcParseActions.cpp
// Parse actions for Resource Loader parser.
// ScriptFigher Project.
// A. Savidis, January 2000.
// Refactored January 2007 (deleting all tables upon error,
// other corrections).
// Extension March 2009, late string destruction (ids, consts).
// Extension July 2009, added support for explicit numeric indices.
//
#include "DDebug.h"
#include "DeltaValue.h"
#include "DeltaObject.h"
#include "DeltaTableFactory.h"
#include "RcParseActions.h"
#include "ustlhelpers.h"
#include <list>
#include <functional>

using namespace RcLoader;

//------------------------------------------------------------

static DeltaObject*				resources	= (DeltaObject*) 0;
static std::list<DeltaTable*>*	allTables	= (std::list<DeltaTable*>*) 0;

struct DecRefCounterFunctor : public std::unary_function<DeltaTable*, void> {
	void operator()(DeltaTable* t) { DPTR(t)->DecRefCounter((DeltaValue*) 0); }
};

//////////////////////////////////////////////////////////////

void AllocateResources (void)  {

	DASSERT(!resources && !allTables); 
	resources = DeltaObject::NativeCodeHelpers::NewObject();
	DeltaObject::NativeCodeHelpers::Refer(resources);
	DPTR(resources)->IncRefCounter((DeltaValue*) 0);

	allTables = DNEW(std::list<DeltaTable*>);
}

//*****************************

DeltaTable*	GetResources (void) {
	DASSERT(resources); 
	DeltaObject* ret = resources; 
	unullify(resources);
	return ret;
}

//*****************************

void UnreferenceTables (void) {
	std::for_each(
		DPTR(allTables)->begin(),
		DPTR(allTables)->end(),
		DecRefCounterFunctor()
	);
	udelete(allTables);
}

//*****************************

void ReleaseResources (void) {
	DASSERT(resources);
	resources->DecRefCounter((DeltaValue*) 0);
	unullify(resources);
}

//------------------------------------------------------------

char* RcLoader::Manage_StringConst (char* s1, char* s2){
	if (!s2)
		{ DASSERT(s1); return s1; }
	else
	if (!s1)
		{ DASSERT(s2); return s2; }
	else
		return StringLateDestroy(ucopystr(std::string(s1) + s2));
}

////////////////////////////////////////////////////////////////

static void MakeResourceContent (DeltaValue* obj, ResourceValue& val) {
	switch (val.type) {
		case Resource_String:	DASSERT(val.strVal); obj->FromString(val.strVal);	return;
		case Resource_Number:	obj->FromNumber(val.numVal);							return;
		case Resource_Bool:		obj->FromBool(val.boolVal);							return;
		case Resource_Table:	DASSERT(val.table); obj->FromTable(val.table);		return;
		default: DASSERT(false);
	}
}

////////////////////////////////////////////////////////////////

static void MakeResourceIndex (DeltaValue* obj, ResourceValue& val) {
	if (val.strIndex)
		obj->FromString(val.strIndex);
	else
		obj->FromNumber(val.explicitNumIndex ? val.numIndex : val.autoNumIndex);
}

////////////////////////////////////////////////////////////////

static void AddResourceInTable (DeltaTable* table, ResourceValue& val) {

	DeltaValue index;
	MakeResourceIndex(&index, val);

	DeltaValue content;
	MakeResourceContent(&content, val);

	DPTR(table)->Set(index, content);
}

////////////////////////////////////////////////////////////////
// Since we have late destruction no deletion takes place,
// just nullification.
// 
static void ClearResource (ResourceValue& val) {
	unullify(val.strIndex);
	if (val.type == Resource_String)
		unullify(val.strVal);
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_Literal (ResourceValue& pp, char* strVal) { 
	pp.Initialize();
	pp.type		= Resource_String;
	pp.strVal	= strVal;	// It is already a dynamic string const
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_Literal (ResourceValue& pp, double numVal) { 
	pp.Initialize();
	pp.type		= Resource_Number;
	pp.numVal	= numVal;
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_Literal (ResourceValue& pp, bool boolVal) { 
	pp.Initialize();
	pp.type	= Resource_Bool;
	pp.boolVal	= boolVal;
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_EmptyResources (ResourceValue& pp) {
	pp.Initialize();
	pp.type		= Resource_Table;
	pp.table	= DeltaObject::NativeCodeHelpers::NewObject();
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_ResourceItems (ResourceValue& item) {
	DASSERT(item.IsValid());
	AddResourceInTable(resources, item);
	ClearResource(item);
}

////////////////////////////////////////////////////////////////
// The automatic numeric index is kept in pp, and is increased by 1 one each time a resource
// value not explicitly indexed is added.
//
void RcLoader::Manage_ResourceValues (ResourceValue& pp, ResourceValue& values, ResourceValue& value) { 

	DASSERT(values.IsValid() && value.IsValid());

	if (!value.strIndex) {
		if (value.type == Resource_SetIndex)
			pp.autoNumIndex	 = value.autoNumIndex;	// Set explicitly the next index.
		else {
			if (!value.explicitNumIndex)
				value.autoNumIndex = values.autoNumIndex;
			pp.autoNumIndex	 = values.autoNumIndex + (value.explicitNumIndex ? 0 : 1);
		}
	}
	else
		pp.autoNumIndex = values.autoNumIndex;

	unullify(pp.strIndex);
	pp.type			= Resource_Table;
	pp.table		= values.table;

	if (value.type != Resource_SetIndex)
		AddResourceInTable(pp.table, value);
	ClearResource(value);
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_ResourceValue (ResourceValue& pp, ResourceValue& value) { 
	
	DASSERT(value.IsValid());
	
	pp.Initialize();

	if (value.strIndex || value.explicitNumIndex)
		pp.autoNumIndex	= 0;
	else
	if (value.type == Resource_SetIndex)
		pp.autoNumIndex	= value.autoNumIndex;
	else
		pp.autoNumIndex	= 1;
	pp.type			= Resource_Table;
	pp.table		= DeltaObject::NativeCodeHelpers::NewObject();

	// We keep all tables cenrtally as they may only
	// be destroyed via reference counting decrease.
	//
	pp.table->IncRefCounter((DeltaValue*) 0);
	DPTR(allTables)->push_back(pp.table);

	if (value.type != Resource_SetIndex)
		AddResourceInTable(pp.table, value);
	ClearResource(value);
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_ResourceItem (ResourceValue& pp, char* index, ResourceValue& val) {
	pp.Assign(val);
	pp.strIndex = index;
	pp.explicitNumIndex = false;
	pp.autoNumIndex = 0;
}

void RcLoader::Manage_NumericIndexResource (ResourceValue& pp, double index, ResourceValue& val) {
	pp.Assign(val);
	unullify(pp.strIndex);
	pp.explicitNumIndex = true;
	pp.autoNumIndex = 0;
	pp.numIndex	= (util_ui32) index;
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_ArrayOfResourceValues (ResourceValue& pp, ResourceValue& values) {
	pp.Assign(values);
	pp.autoNumIndex = 0; 
	pp.explicitNumIndex = false;
	unullify(pp.strIndex); 
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_SingletonResource (ResourceValue& pp, ResourceValue& value) {
	Manage_ResourceValue(pp, value);
	pp.autoNumIndex = 0; 
	unullify(pp.strIndex); 		
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_DirectiveResource (ResourceValue& pp, const char* id) {
	ResourceValue val;
	Manage_Literal(val, StringLateDestroy(ucopystr(id)));
	Manage_ResourceValue(pp, val);
	pp.autoNumIndex = 0; 
	unullify(pp.strIndex); 				
}

////////////////////////////////////////////////////////////////

void RcLoader::Manage_SetAutoNumIndexStartValue (ResourceValue& pp, util_ui32 val) {
	pp.Initialize();
	pp.type = Resource_SetIndex;
	pp.autoNumIndex = val;
}

//------------------------------------------------------------
