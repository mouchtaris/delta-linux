#ifndef TO_CORBA_CONVERTER_H
#define TO_CORBA_CONVERTER_H

#include "Corba.h"
#include <map>
#include <list>

class DeltaValue;

class CORBACOMMON_CLASS ToCorbaConverter {
public:
	typedef bool (*ObjectConversionFunc)(CORBA::Any* output, DeltaValue* input);	//returns true if successful conversion
private:
	typedef void (*ConverterFunc)(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	typedef std::map<CORBA::TCKind, ConverterFunc> ConversionMap;
	static ConversionMap conversions;

	//**********************************************************************

	typedef std::map<std::string, CORBA::TypeCode_ptr> TypecodeMapping;
	static TypecodeMapping numberToAnyConversions;
	static TypecodeMapping stringToAnyConversions;
	static TypecodeMapping charToAnyConversions;

	//**********************************************************************

	typedef void (*UnionDefaultValueGenerator)(CORBA::Any* output, CORBA::TypeCode_ptr tc);
	typedef std::map<CORBA::TCKind, UnionDefaultValueGenerator> UnionDefaultValueGenerationMap;
	static UnionDefaultValueGenerationMap unionDefaultValues;

	//**********************************************************************

	typedef std::list<ObjectConversionFunc> ObjectConversionFuncList;
	static ObjectConversionFuncList objectConversions;

	//**********************************************************************

	static void to_union_helper(CORBA::Any* output, DeltaValue* value, CORBA::TypeCode_ptr tc, unsigned index);

	//**********************************************************************

	static void to_short		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_long			(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_ushort		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_ulong		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_float		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_double		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_longlong		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_ulonglong	(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_longdouble	(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);

	static void to_boolean		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_string		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_wstring		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_char			(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_wchar		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);
	static void to_octet		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc = NULL);

	static void to_sequence		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_array		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_struct		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_union		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_enum			(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_alias		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_except		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_any			(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_objref		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_null			(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);
	static void to_TypeCode		(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);

public:
	static void convert			(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc);

	static void AddObjectConversion		(ObjectConversionFunc f);
	static void RemoveObjectConversion	(ObjectConversionFunc f);

	static void Initialize	(void);
	static void Cleanup		(void);
};

#endif
