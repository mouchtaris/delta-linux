// DeltaExternId.h
// A class for holding different types of extern ids to
// make the corresponding DeltaValue field thinner.
// It is actually a decorator class.
// ScriptFigher Project.
// A. Savidis, June 2003.
// Extended July 2008, added field getter support for external
// (native) objects
//

#ifndef	DELTA_EXTERNID_H
#define	DELTA_EXTERNID_H

#include <string.h>
#include "DDebug.h"
#include "DeltaString.h"
#include "DeltaCollectableContainer.h"
#include "DeltaCollectableSimple.h"
#include "DeltaExternIdFieldGetter.h"
#include "DeltaValueTypes.h"
#include "DeltaCreationInfo.h"
#include "urecyclefactory.h"
#include "uvalidatable.h"
#include "usingleton.h"
#include <string>

///////////////////////////////////
// Extern ids may fall in any of the following
// three types.
//
enum DeltaExternIdType {

	DeltaExternId_NonCollectable		= 0,
	DeltaExternId_CollectableContainer	= 1,
	DeltaExternId_CollectableSimple		= 2
};

////////////////////////////////////////////////////////////

#define	DELTA_EXTERNID_TYPESTR_UNDEF	"undef"
#define	DELTA_EXTERNID_TOSTRINGNULL		(void(*)(DeltaString*, void*)) 0
#define	DELTA_EXTERNID_SELF_CLASSID		"DeltaExternId"
#define	DELTA_EXTERNID_VGET_INST(val)	VGET_INST(DeltaExternId, (val), DELTA_EXTERNID_SELF_CLASSID)
#define	DELTA_EXTERNID_TYPE_INDEX		"_type_"
#define	DELTA_EXTERNID_MADEIN_INDEX		"_madein_"
#define	DELTA_EXTERNID_USERDATA_INDEX	"_userdata_"
#define	DELTA_EXTERNID_USERDATA_DESC	"userdata"

class DVM_CLASS DeltaExternId : public Validatable {

	DFRIENDDESTRUCTOR()
	friend class DeltaExternIdFactory;
	friend class DeltaExternIdPlacementConstructor;

	////////////////////////////////////////////////////////////

	public:
	typedef void (*ToStringFunc) (DeltaString* at, void* val);

	////////////////////////////////////////////////////////////

	private:
	USINGLETON_APISTYLE_DECLARE_PRIVATESTDMETHODS

	typedef std::map<void*, DeltaExternId*>		ExternIdMap;
	typedef std::map<std::string, ExternIdMap>	ExternIdMaps;

	static ExternIdMaps*			externIdMaps;

	DeltaExternIdType				type;
	void*							val;
	std::string						typeStr;
	util_ui32						refCounter;
	ubag<DeltaValue*>				referrers;
	void							(*toString)(DeltaString* at, void* val);
	const DeltaExternIdFieldGetter*	fieldGetter;

	DeltaObject*					userData;
	DeltaCreationInfo				creationInfo;

	bool					Invariant (void) const;

	void					Register (void);
	void					Cancel (void);
	void					CreateUserData (void);

	void					ReferToCollectable (DeltaValue* owner);
	void					UnreferToCollectable (DeltaValue* owner);

	void					ConvertToStringHandler (
								DeltaString* at, 
								void (CollectableContainer::*converter)(DeltaString*, void*)
							) const;

	//**********************
	// This way we record if the string conversion process was inquired
	// to be done with overloading or not. These two conversion process
	// can never be intreleaved.
	//
	static util_i32			toStringCounter;

	static void				EnterConvertToString (void);
	static void				EnterConvertToStringBySkippingUserCode (void);
	static void				ExitConvertToString (void);
	static void				ExitConvertToStringBySkippingUserCode (void);

	////////////////////////////////////////////////////////////

	public:
	static bool				UseConvertToStringBySkippingUserCode (void);
	static bool				UseConvertToString (void);
	static bool				IsStringConversionInProgress (void);

	ToStringFunc			GetToStringFunc (void) const 
									{ DASSERT(Invariant()); return toString; }
	DeltaExternIdType		GetType (void) const
									{ DASSERT(Invariant()); return type; }
	const std::string& 		GetTypeStr (void) const
								{ DASSERT(Invariant()); return typeStr; }
	void*					GetValue (void)
								{ DASSERT(Invariant()); return val; }
	const DeltaExternIdFieldGetter*	
							GetFieldGetter (void) const
								{ DASSERT(Invariant()); return fieldGetter; }
	DeltaObject*			GetUserData (void) 
								{ return userData; }
	const DeltaObject*		GetUserData (void) const
								{ return userData; }
	static const DeltaExternId*	
							GetExternId (void* val);

	//**************************

	bool					Equal (const std::string& _typeStr, void* val) const;
	bool					operator==(const DeltaExternId& id) const;
	bool					operator!=(const DeltaExternId& id) const 
								{ return !(operator==(id)); }
	bool					ToBool (void) const 
								{ return true; }
	bool					operator!(void) const // Always true. No null values allowed.
								{ return false; }

	void					ConvertToString (DeltaString* at, void* closure = (void*) 0) const;
	void					ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0) const;

	const DeltaCreationInfo&	
							GetCreationInfo (void) const 
								{ return creationInfo; }
	DeltaCreationInfo&		GetCreationInfo (void) 
								{ return creationInfo; }

	//**************************
	// Sharing and auto-collection.

	void					IncRefCounter (DeltaValue* owner);
	void					DecRefCounter (DeltaValue* owner);
	const ubag<DeltaValue*>&
							GetReferrers (void) const 
								{ return referrers; }

	//**************************
	// Virtual functions required for Validatable base class.
	
	const char*				GetExtClassString (void) const 
								{ return typeStr.c_str(); }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(DELTA_EXTERNID_SELF_CLASSID)

	//**************************

	static bool				IsReservedFieldKey (const std::string& key);
	static void				GetReservedField (
								const std::string&	key, 
								DeltaValue&			externId, 
								DeltaValue*			at
							);
	static void				AddReservedFieldKeys (std::list<std::string>* keys);

	static DeltaExternId*	Get (const std::string& typeStr, void* val);

	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS

	////////////////////////////////////////////////////////////

	private:

	//**************************
	// In case of extern ids that are collectables, the void* passed to the
	// string converter callback is always 'this'

	DeltaExternId (
		void*							_val,
		DeltaExternIdType				_type			= DeltaExternId_NonCollectable,
		ToStringFunc					_toString		= (ToStringFunc) 0,
		const std::string&				_typeStr		= DELTA_EXTERNID_TYPESTR_UNDEF,
		const DeltaExternIdFieldGetter*	_fieldGetter	= DELTA_EXTERNID_NO_FIELD_GETTER
	);
	virtual ~DeltaExternId();
};

////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point...
