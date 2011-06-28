// DebugAttributeInformation.h
// Holds information about object attributes (the pattern), and
// is stored as extern id in values of the epression evaluator.
// ScriptFighter Project.
// A. Savidis, December 2009.
//

#ifndef	DEBUGATTRIBUTEINFORMATION_H
#define	DEBUGATTRIBUTEINFORMATION_H

#include "uvalidatable.h"
#include "utypes.h"
#include "usingleton.h"
#include "DeltaTable.h"
#include "DeltaExternIdFieldGetter.h"
#include <string>

#define	ATTR_INFO_TYPE_STR "<attribute>"

///////////////////////////////////////////////////////////////

class DebugAttributeInformation : public Validatable {
	
	private:
	static ubag<Validatable*>*			insts;
	static DeltaExternIdFieldGetter*	fieldGetter;
	const DeltaTable::Attribute*		attr;

	USINGLETON_APISTYLE_DECLARE_PRIVATESTDMETHODS

	static void						ToString (DeltaString* at, void* val);	// For extern ids.
	const char*						GetExtClassString (void) const		
										{ return ATTR_INFO_TYPE_STR; }
	const DeltaTable::Attribute*	GetAttribute (void) const
										{ return attr; }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(ATTR_INFO_TYPE_STR)

	static bool 					GetKeys		(void* val, DeltaValue* at);
	static bool 					GetValue	(void* val, DeltaValue* at); 
	static bool 					GetSetter	(void* val, DeltaValue* at); 
	static bool 					GetGetter	(void* val, DeltaValue* at); 

	static DeltaExternIdFieldGetter::GetByStringFuncEntry fieldGetters[4];

	DebugAttributeInformation (const DeltaTable::Attribute* _attr);
	virtual ~DebugAttributeInformation();

	///////////////////////////////////////////////////////////////

	public:
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	static void						Initialise (void);	
	static void						CleanUp (void);		// Deletes all created instances.

	static void						ConvertToDelta (
										const DeltaTable::Attribute*	attr, 
										DeltaValue*						at
									);
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

