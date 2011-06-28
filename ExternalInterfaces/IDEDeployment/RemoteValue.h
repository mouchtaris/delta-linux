/**
 *	RemoteValue.h
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	October 2010
 */
#ifndef REMOTE_VALUE_H
#define REMOTE_VALUE_H

#include "IDEDeploymentCommon.h"
#include <list>
#include <string>
#include <assert.h>
#include <wx/string.h>

namespace ext {

////////////////////////////////////////////////////////////////////////

enum RemoteValueType {
	RemoteValueTypeVoid		= 0,
	RemoteValueTypeUChar	= 1,
	RemoteValueTypeChar		= 2,
	RemoteValueTypeUShort	= 3,
	RemoteValueTypeShort	= 4,
	RemoteValueTypeUInt		= 5,
	RemoteValueTypeInt		= 6,
	RemoteValueTypeULong	= 7,
	RemoteValueTypeLong		= 8,
	RemoteValueTypeFloat	= 9,
	RemoteValueTypeDouble	= 10,
	RemoteValueTypeBool		= 11,
	RemoteValueTypeString	= 12,
	RemoteValueTypeWxString	= 13,
	RemoteValueTypeHandle	= 14
};

////////////////////////////////////////////////////////////////////////

struct _DEPLOY_API RemoteHandle {
	std::string classId;
	unsigned int serial;

	RemoteHandle(void) : serial(0) {}
	RemoteHandle(const std::string& classId, unsigned int serial) : classId(classId), serial(serial) {}
};

////////////////////////////////////////////////////////////////////////

class _DEPLOY_API RemoteValue {
private:
	union {
		unsigned char	ucharVal;
		char			charVal;
		unsigned short	ushortVal;
		short			shortVal;
		unsigned int	uintVal;
		int				intVal;
		unsigned long	ulongVal;
		long			longVal;
		float			floatVal;
		double			doubleVal;
		bool			boolVal;
		std::string*	stringVal;
		wxString*		wxStringVal;
		RemoteHandle*	handleVal;
	} value;
	RemoteValueType type;
	bool undefined;

	//******************************************************************
	// Private functions

	void Clear (void) {
		if (!IsUndefined())
			if (IsString())			delete value.stringVal;
			else if (IsWxString())	delete value.wxStringVal;
			else if (IsHandle())	delete value.handleVal;
	}

	//******************************************************************
	// Method generation using templates and macros
	//
	template<typename T> void SetTypeTemplate (void);
	template<typename T> RemoteValueType GetTypeTemplate (void) const;

#define IMPLEMENT_TYPE_TEMPLATES(_ctype, _valuetype)										\
	template<> void SetTypeTemplate<_ctype> (void) { type = _valuetype; }					\
	template<> RemoteValueType GetTypeTemplate<_ctype> (void) const { return _valuetype; }

#define IMPLEMENT_SETTER(_name, _type, _field)	\
	void _name (_type val) {					\
		Clear();								\
		SetTypeTemplate<_type>();				\
		undefined = false;						\
		value._field = val;						\
	}
#define IMPLEMENT_GETTER(_name, _type, _field)						\
	_type _name (void) const {										\
		assert(!IsUndefined() && GetTypeTemplate<_type>() == type);	\
		return value._field;										\
	}
#define IMPLEMENT_CHECKER(_name, _type)	\
	bool _name (void) const { return GetTypeTemplate<_type>() == type; }

#define IMPLEMENT_CONSTRUCTOR(_type, _field)				\
	explicit RemoteValue (_type val) : undefined(false) {	\
		SetTypeTemplate<_type>();							\
		value._field = val;									\
	}

#define IMPLEMENT_FACTORY(_name, _type)	\
	static const RemoteValue _name (_type val) { return RemoteValue(val); }

#define IMPLEMENT_UNION_FIELD_FUNCTIONS(_basename, _valuetype, _ctype, _field)	\
	private:																	\
		IMPLEMENT_TYPE_TEMPLATES(_ctype, _valuetype)							\
		IMPLEMENT_CONSTRUCTOR(_ctype, _field)									\
	public:																		\
		IMPLEMENT_SETTER(Set##_basename, _ctype, _field)						\
		IMPLEMENT_GETTER(Get##_basename, _ctype, _field)						\
		IMPLEMENT_CHECKER(Is##_basename, _ctype)								\
		IMPLEMENT_FACTORY(_basename, _ctype)
		
	IMPLEMENT_UNION_FIELD_FUNCTIONS(UChar,	RemoteValueTypeUChar,	unsigned char,	ucharVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Char,	RemoteValueTypeChar,	char,			charVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(UShort,	RemoteValueTypeUShort,	unsigned short, ushortVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Short,	RemoteValueTypeShort,	short,			shortVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(UInt,	RemoteValueTypeUInt,	unsigned int,	uintVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Int,	RemoteValueTypeInt,		int,			intVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(ULong,	RemoteValueTypeULong,	unsigned long,	ulongVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Long,	RemoteValueTypeLong,	long,			longVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Float,	RemoteValueTypeFloat,	float,			floatVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Double,	RemoteValueTypeDouble,	double,			doubleVal)
	IMPLEMENT_UNION_FIELD_FUNCTIONS(Bool,	RemoteValueTypeBool,	bool,			boolVal)

#undef IMPLEMENT_UNION_FIELD_FUNCTIONS
#undef IMPLEMENT_FACTORY
#undef IMPLEMENT_CONSTRUCTOR
#undef IMPLEMENT_CHECKER
#undef IMPLEMENT_GETTER
#undef IMPLEMENT_SETTER

#define IMPLEMENT_OBJECT_SETTER(_name, _type, _field)	\
	void _name (const _type& val) {						\
		Clear();										\
		SetTypeTemplate<_type>();						\
		undefined = false;								\
		value._field = new _type(val);					\
	}
#define IMPLEMENT_OBJECT_GETTER(_name, _type, _field)								\
	const _type& _name (void) const {												\
		assert(!IsUndefined() && GetTypeTemplate<_type>() == type && value._field);	\
		return *value._field;														\
	}
#define IMPLEMENT_OBJECT_CHECKER(_name, _type, _field)	\
	bool _name (void) const { return GetTypeTemplate<_type>() == type; }

#define IMPLEMENT_OBJECT_CONSTRUCTOR(_ctype, _valuetype, _field)	\
	explicit RemoteValue (const _ctype& val) : type(_valuetype), undefined(false) { value._field = new _ctype(val); }

#define IMPLEMENT_OBJECT_FACTORY(_name, _type)	\
	static const RemoteValue _name (const _type& val) { return RemoteValue(val); }

#define IMPLEMENT_UNION_FIELD_OBJECT_FUNCTIONS(_basename, _valuetype, _ctype, _field)	\
	private:																			\
		IMPLEMENT_TYPE_TEMPLATES(_ctype, _valuetype)									\
		IMPLEMENT_OBJECT_CONSTRUCTOR(_ctype, _valuetype, _field)						\
	public:																				\
		IMPLEMENT_OBJECT_SETTER(Set##_basename, _ctype, _field)							\
		IMPLEMENT_OBJECT_GETTER(Get##_basename, _ctype, _field)							\
		IMPLEMENT_OBJECT_CHECKER(Is##_basename, _ctype, _field)							\
		IMPLEMENT_OBJECT_FACTORY(_basename, _ctype)
		

	IMPLEMENT_UNION_FIELD_OBJECT_FUNCTIONS(String,	RemoteValueTypeString,	std::string,	stringVal)
	IMPLEMENT_UNION_FIELD_OBJECT_FUNCTIONS(WxString, RemoteValueTypeWxString, wxString, wxStringVal)
	IMPLEMENT_UNION_FIELD_OBJECT_FUNCTIONS(Handle,	RemoteValueTypeHandle,	RemoteHandle,	handleVal)

#undef IMPLEMENT_UNION_FIELD_OBJECT_FUNCTIONS
#undef IMPLEMENT_OBJECT_FACTORY
#undef IMPLEMENT_OBJECT_CONSTRUCTOR
#undef IMPLEMENT_OBJECT_CHECKER
#undef IMPLEMENT_OBJECT_GETTER
#undef IMPLEMENT_OBJECT_SETTER

#undef IMPLEMENT_TYPE_TEMPLATES

public:
	//******************************************************************
	// Seperate handling for the void type
	//
	void SetVoid (void) { Clear(); type = RemoteValueTypeVoid; }
	bool IsVoid (void) const { return type == RemoteValueTypeVoid; }
	RemoteValue(void) : type(RemoteValueTypeVoid), undefined(false) {}

	//******************************************************************
	// Remaining members
	//
	RemoteValueType GetType (void) const { return type; }
	bool IsUndefined (void) const { return undefined; }

	explicit RemoteValue(RemoteValueType type) : type(type), undefined(true) {}

	RemoteValue(const RemoteValue& val) {
		memcpy(this, &val, sizeof(RemoteValue));
		if (!IsUndefined())
			if(IsString())
				value.stringVal = new std::string(val.GetString());
			else if(IsWxString())
				value.wxStringVal = new wxString(val.GetWxString());
			else if(IsHandle())
				value.handleVal = new RemoteHandle(val.GetHandle());
	}
	
	~RemoteValue() { Clear(); }
};

////////////////////////////////////////////////////////////////////////

typedef std::list<RemoteValue> RemoteValueList;

////////////////////////////////////////////////////////////////////////
// Helper functions to create argument lists
//
_DEPLOY_API RemoteValueList ArgumentList(void);
_DEPLOY_API RemoteValueList ArgumentList(const RemoteValue& t0);
_DEPLOY_API RemoteValueList ArgumentList(const RemoteValue& t0, const RemoteValue& t1);
_DEPLOY_API RemoteValueList ArgumentList(const RemoteValue& t0, const RemoteValue& t1, const RemoteValue& t2);
_DEPLOY_API RemoteValueList ArgumentList(const RemoteValue& t0, const RemoteValue& t1, const RemoteValue& t2, const RemoteValue& t3);
_DEPLOY_API RemoteValueList ArgumentList(const RemoteValue& t0, const RemoteValue& t1, const RemoteValue& t2, const RemoteValue& t3, const RemoteValue& t4);
_DEPLOY_API RemoteValueList ArgumentList(const RemoteValue& t0, const RemoteValue& t1, const RemoteValue& t2, const RemoteValue& t3, const RemoteValue& t4, const RemoteValue& t5);

////////////////////////////////////////////////////////////////////////

} // namespace ext

#endif // REMOTE_VALUE_H
