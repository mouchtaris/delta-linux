// uvalidatable.h
// A generic instance validator, handling the case where
// a call to some functions may destroy and recycle a dynamic instance.
// The latter may happen for heap / stack objects.
// Inside the ScriptFighter Project.
// A. Savidis, December 2005.
// Extended January 2008, added support for constituents to detect
// destruction of their owner container.
//

#ifndef	UVALIDATABLE_H
#define	UVALIDATABLE_H

#include "uvalidator.h"
#include "utypes.h"
#include "ucrosscast.h"

#include <stack>
#include <map>

#define	VALIDATABLE_STARTING_SERIALNO	1
#define	VALIDATABLE_CANTHAVE_SERIALNO	0

#define	VALIDATABLE_RECORD_SERIALNO1(var)				\
			util_ui32 serialNo##var = var->GetSerialNo()

#define	VALIDATABLE_RECORD_SERIALNO2(var1,var2)			\
			VALIDATABLE_RECORD_SERIALNO1(var1);			\
			VALIDATABLE_RECORD_SERIALNO1(var2)

#define	VALIDATABLE_RECORD_SERIALNO3(var1,var2,var3)	\
			VALIDATABLE_RECORD_SERIALNO1(var1);			\
			VALIDATABLE_RECORD_SERIALNO2(var2,var3)

#define	VALIDATABLE_CHECKED_SERIALNO1(var1)				\
	(ValidatableHandler::Get(serialNo##var1))

#define	VALIDATABLE_CHECKED_SERIALNO2(var1,var2)		\
		(VALIDATABLE_CHECKED_SERIALNO1(var1) && VALIDATABLE_CHECKED_SERIALNO1(var2))

#define	VALIDATABLE_CHECKED_SERIALNO3(var1,var2,var3)	\
		(VALIDATABLE_CHECKED_SERIALNO1(var1) && VALIDATABLE_CHECKED_SERIALNO2(var2,var3))

#define	VALIDATABLE_VAR_SERIALNO(var) serialNo##var

//---------------------------------------------------------------

class UTILLIB_CLASS Validatable;

class UTILLIB_CLASS ValidatableHandler {

	friend class Validatable;

	private:
	static bool										singletonCreated;
	static uvalidator*								validator;
	typedef std::map<util_ui32, Validatable*>		InverseMap;
	static InverseMap*								inverseValidators[UVALIDATOR_HASH_SIZE];

	static util_ui32	Hash (util_ui32 serialNo);
	static util_ui32	Hash (Validatable* obj);
	static void			Register (Validatable* obj);
	static void 		Cancel (Validatable* obj);
	static bool			IsSingletonCreated(void) 
							{ return singletonCreated; }

	//////////////////////////////////////////////////////////////////

	public:
	static void 		SingletonCreate (void);
	static void			SingletonDestroy (void);

	static Validatable*	Get (util_ui32 serialNo);
	static bool 		Validate (const Validatable* obj, util_ui32 serialNo);
	static bool 		Validate (const Validatable& obj, util_ui32 serialNo)
							{ return Validate(&obj, serialNo); }
};

//////////////////////////////////////////////////////////////////
// The 'owner' allows to validate members of aggregates accrording to the existence
// of the aggregate, assuming the aggregate is also wrapped-up as a Validatable.
//
class UTILLIB_CLASS Validatable : public DynamicCastable {
	
	//////////////////////////////////////////////////////////////////

	private:
	typedef std::list<util_ui32*>	SerialNoPtrStack;

	static util_ui32	currSerialNo;
	util_ui32			serialNo;
	Validatable*		owner;
	util_ui32			ownerSerialNo;
	SerialNoPtrStack	serialNoStack;

	bool				Invariant (void) const;

	//////////////////////////////////////////////////////////////////

	public:
	virtual const char*	GetExtClassString (void) const = 0;

	const Validatable*	GetOwner (void) const
							{ return owner; }
	util_ui32			GetOwnerSerialNo (void) const
							{ return ownerSerialNo; }

	util_ui32			GetSerialNo (void) const 
							{ return serialNo; }

	void				Validate (void) const
							{ DASSERT(IsValid()); }

	void				PushSerialNoPointer (util_ui32* sn)
							{ serialNoStack.push_front(sn); }

	void				PopSerialNoPointer (util_ui32* sn)
							{ DASSERT(sn == serialNoStack.front()); serialNoStack.pop_front(); }

	static bool			IsValidInstance (const Validatable* obj);
	bool				IsValid (void) const;

	template <class T>
	static T*	Get (void* serialNo, const std::string& classId) {
					Validatable* v = ValidatableHandler::Get((util_ui32) serialNo);
					if (v && (!v->owner || ValidatableHandler::Get(v->ownerSerialNo)))
						return (T*) v->CastTo(classId);
					else
						return (T*) 0;
	}

	Validatable (Validatable* p = (Validatable*) 0);
	virtual ~Validatable();
};

//---------------------------------------------------------------
// No copy constructor, no assignment operator, no parameterized constructor,
// and no default constructor, as a value is uniquely associated to a serial number.
//
template <class T, class C> class ValidatableValue : public Validatable {
	private:
	T* val;

	public:
	static ValidatableValue*	Get (void* val) 
									{ return Validatable::Get<ValidatableValue>(val, C::ID); }

	virtual const char*			GetExtClassString (void) const
									{ return C::ID; }

	T*							GetValue (void) { return val; }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(C::ID)

	ValidatableValue (T* _val, Validatable* p = (Validatable*) 0):  Validatable(p), val(_val){}
	~ValidatableValue(){}
};

//---------------------------------------------------------------

class ValidatableUntypedValue : public Validatable {
	private:
	void*			val;
	std::string		myClassId;

	public:
	virtual const char*			GetExtClassString (void) const
									{ return myClassId.c_str(); }

	void*						GetValue (void) 
									{ return val; }

	template <class T>
	static T*					GetTypedValue (void* val, const std::string& classId) {
									ValidatableUntypedValue* v = Validatable::Get<ValidatableUntypedValue>(val, classId);
									return !v ? (T*) 0 : (T*) v->GetValue();
								}

	static void*				GetUntypedValue (void* val) {
									ValidatableUntypedValue* v = (ValidatableUntypedValue*) ValidatableHandler::Get((util_ui32) val);
									return !v ? (void*) 0 : v->GetValue();
								}

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(myClassId)

	ValidatableUntypedValue (void* _val, const std::string& _myClassId, Validatable* p = (Validatable*) 0):  
			Validatable(p), 
			val(_val), 
			myClassId(_myClassId){}

	virtual ~ValidatableUntypedValue(){}
};

//////////////////////////////////////////////////////////////////

#define	VALIDATABLE_VGET_INST(serialNo)										\
	ValidatableHandler::Get((util_ui32) serialNo)

#define	VGET_INST(_class, _value, _classid)									\
	Validatable::Get<_class>((void*) _value, _classid)

#define	VGET_VALUE(_class, _classidtrait, _value)							\
	ValidatableValue<_class,_classidtrait>::Get((void*) _value)

#define	VGET_TYPEDVALUE(_class, _classid, _value)							\
	ValidatableUntypedValue::GetTypedValue<_class>(_value, _classid)

#define	VGET_UNTYPEDVALUE(_value)											\
	ValidatableUntypedValue::GetUntypedValue(_value)

//*********************************

#define	VCLASSID_HEADER(classname)											\
	struct classname { static const char* ID; };

#define	VCLASSID_IMPL(classname, cidstr)									\
	const char* classname::ID = cidstr;

#define	VCLASSID_DEFINE(classname, cidstr)									\
	VCLASSID_HEADER(classname)												\
	VCLASSID_IMPL(classname, cidstr)

#define	VCLASSID_TRAITS_DEFINE(traitsname, classname, cidstr)				\
	template <> struct traitsname<classname> { static const char* ID; };	 \
	const char* traitsname<classname>::ID = cidstr;

//*********************************
// Say FILE* is the native value type and we use the class name "file".
// The we use the macros as follows (may choose other namwe than FILE_CID
// and ValidatableFILE, but it is suggested to keep it this way for readability.
//
// VCLASSID_DEFINE(FILE_CID, "file");
// typedef ValidatableValue<FILE, FILE_CID> ValidatableFILE;
//
// To gain from a serial number (possibly casted to void*) back the FILE*
// after validation use the following: VGET_VALUE(FILE, FILE_CID, val)

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

