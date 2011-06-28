// RcParseActions.h
// Parse actions for Resource Loader parser.
// ScriptFigher Project.
// A. Savidis, January 2000.
// Slight refactoring, January 2007.
//

#ifndef	RCPARSEACTIONS_H
#define	RCPARSEACTIONS_H

#include "uptr.h"
#include "utypes.h"

//------------------------------------------------------------

class DeltaTable;
extern void			AllocateResources (void);
extern DeltaTable*	GetResources (void);
extern void			ReleaseResources (void);
extern void			UnreferenceTables (void);

//------------------------------------------------------------

enum ResourceValueType {
	Resource_String		= 0,
	Resource_Number		= 1,
	Resource_Bool		= 2,
	Resource_Table		= 3,
	Resource_SetIndex	= 4,	// Appears only to change the auto indexing.
	Resource_Undefined	= 5
};

struct ResourceValue {

	ResourceValueType	type;
	util_ui32			autoNumIndex;
	util_ui32			numIndex;
	char*				strIndex;
	char*				strVal;
	double				numVal;
	bool				boolVal;
	bool				explicitNumIndex;
	DeltaTable*			table;
	
	bool HasExplicitNumericIndex (void) const 
		{ return explicitNumIndex; }

	void Assign (const ResourceValue& p) {
		type				= p.type;
		autoNumIndex		= p.autoNumIndex;
		numIndex			= p.numIndex;
		strIndex			= p.strIndex;
		strVal				= p.strVal;
		numVal				= p.numVal;
		boolVal				= p.boolVal;
		explicitNumIndex	= p.explicitNumIndex;
		table				= p.table;

	}

	bool IsValid (void) const { 
		return	((util_ui32) type) >= Resource_String &&
				((util_ui32) type) <= Resource_SetIndex;
	}

	void Initialize (void) {
		type				= Resource_Undefined;
		
		autoNumIndex		= 0;
		numIndex			= 0;
		numVal				= 0;

		boolVal				=
		explicitNumIndex	= false;
		
		unullify(strIndex);
		unullify(strVal);
		unullify(table);
	}
};

//------------------------------------------------------------
namespace RcLoader {

extern char*	Manage_StringConst (char* s1, char* s2);
extern void		Manage_Literal (ResourceValue& pp, char* strVal);	// String is assigned locally
extern void		Manage_Literal (ResourceValue& pp, double numVal);
extern void		Manage_Literal (ResourceValue& pp, bool boolVal);

extern void		Manage_SingletonResource (ResourceValue& pp, ResourceValue& value);
extern void		Manage_DirectiveResource (ResourceValue& pp, const char* id);

extern void 	Manage_ResourceItems (ResourceValue& item);
extern void 	Manage_EmptyResources (ResourceValue& pp);
extern void		Manage_ResourceValues (ResourceValue& pp, ResourceValue& values, ResourceValue& value);
extern void		Manage_ResourceValue (ResourceValue& pp, ResourceValue& value);
extern void		Manage_ResourceItem (ResourceValue& pp, char* index, ResourceValue& val);

extern void		Manage_NumericIndexResource (ResourceValue& pp, double index, ResourceValue& val);
extern void		Manage_ArrayOfResourceValues (ResourceValue& pp, ResourceValue& values);
extern void		Manage_SetAutoNumIndexStartValue (ResourceValue& pp, util_ui32 val);

extern char*	StringLateDestroy (char*);	// Implemented in API.
}

//------------------------------------------------------------


#endif	// Do not add stuff beyond this point.