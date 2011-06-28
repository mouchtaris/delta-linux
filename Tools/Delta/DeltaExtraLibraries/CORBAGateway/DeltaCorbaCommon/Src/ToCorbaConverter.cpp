#include "ToCorbaConverter.h"
#include "ConversionException.h"
#include "TableStructure.h"

#include "Delta.h"

#include <stdlib.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace corba;

ToCorbaConverter::ConversionMap ToCorbaConverter::conversions;

ToCorbaConverter::TypecodeMapping ToCorbaConverter::numberToAnyConversions;
ToCorbaConverter::TypecodeMapping ToCorbaConverter::stringToAnyConversions;
ToCorbaConverter::TypecodeMapping ToCorbaConverter::charToAnyConversions;

ToCorbaConverter::UnionDefaultValueGenerationMap ToCorbaConverter::unionDefaultValues;

ToCorbaConverter::ObjectConversionFuncList ToCorbaConverter::objectConversions;

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> T default_starting_value(void) { return 0; }
template<> CORBA::Short default_starting_value<CORBA::Short>(void) { return ACE_INT16_MIN; }
template<> CORBA::Long default_starting_value<CORBA::Long>(void) { return ACE_INT32_MIN + 1; }

template<class T> void next_value(T& val) { val++; }
template<> void next_value(bool& val) { val ^= true; }

template<class T> static void generate_default_union_value(CORBA::Any* output, CORBA::TypeCode_ptr tc) {
	T defaultValue = default_starting_value<T>();
	while (true) {
		bool found = false;
		for (CORBA::ULong i = 0; i < tc->member_count(); ++i) {
			if (i != tc->default_index()) {
				CORBA::Any* any = tc->member_label(i);
				T val;
				bool ok = (*any) >>= val;
				DASSERT(ok);
				if (val == defaultValue) {
					found = true;
					break;
				}
			}
		}
		if (found)
			next_value<T>(defaultValue);
		else
			break;
	}
	(*output) <<= (T) defaultValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(_name, _type, _typeStr)						\
	void ToCorbaConverter::_name(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {	\
		if (input->Type() != DeltaValue_Number)												\
			throw ConversionException(_typeStr);												\
		*output <<= (_type) input->ToNumber();													\
	}

TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_short, CORBA::Short, "short")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_long, CORBA::Long, "long")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_ushort, CORBA::UShort, "ushort")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_ulong, CORBA::ULong, "ulong")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_float, CORBA::Float, "float")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_double, CORBA::Double, "double")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_longlong, CORBA::LongLong, "longlong")
TO_CORBA_IMPLEMENT_ARITHMETIC_CONVERSION(to_ulonglong, CORBA::ULongLong, "ulonglong")

void ToCorbaConverter::to_longdouble(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_Number)
		throw ConversionException("longdouble");
	CORBA::LongDouble num;
	num.assign(input->ToNumber());
	*output <<= num;
}

void ToCorbaConverter::to_boolean(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_Bool)
		throw ConversionException("boolean");
	*output <<= input->ToBool();
}

void ToCorbaConverter::to_string(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_String)
		throw ConversionException("string");
	*output <<= input->ToString().c_str();
}

void ToCorbaConverter::to_wstring(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_String)
		throw ConversionException("wstring");
	const std::string& str = input->ToString();
	size_t len = str.size();
	wchar_t *wstr = new wchar_t[len + 1];
	mbstowcs(wstr, str.c_str(), len);
	wstr[len] = L'\0';
	*output <<= wstr;
	delete[] wstr;
}

void ToCorbaConverter::to_char(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_String || input->ToString().size() > 1)
		throw ConversionException("char");
	*output <<= CORBA::Any::from_char(input->ToString()[0]);
}

void ToCorbaConverter::to_wchar(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_String || input->ToString().size() > 1)
		throw ConversionException("wchar");
	wchar_t wc;
	mbstowcs(&wc, input->ToString().c_str(), 1);
	*output <<= CORBA::Any::from_wchar(wc);
}

void ToCorbaConverter::to_octet(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	if (input->Type() != DeltaValue_Number)
		throw ConversionException("octet");
	*output <<= CORBA::Any::from_octet((unsigned char) input->ToNumber());
}

//**********************************************************************

#define TO_CORBA_IMPLEMENT_NUMERIC_INDEX_CONVERSION(_name, _type, _typeStr, _code)					\
	void ToCorbaConverter::_name(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {	\
		if (input->Type() != DeltaValue_Table)														\
			throw ConversionException(_typeStr);													\
		DeltaTable* table = input->ToTable();														\
		typedef std::vector<DeltaValue> DeltaValueVector;											\
		DeltaValueVector seq;																		\
		for (int i = 0; true; ++i) {																\
			DeltaValue data;																		\
			table->Get((DeltaNumberValueType) i, &data);											\
			if (data.Type() == DeltaValue_Nil)														\
				break;	/*no more available indices in table*/										\
			else																					\
				seq.push_back(data);																\
		}																							\
		DynamicAny::_type##_var dynany =															\
			DynamicAny::_type::_narrow(GetDynAnyFactory()->create_dyn_any_from_type_code(tc));		\
		_code;																						\
		CORBA::TypeCode_var type = tc->content_type();												\
		for(DeltaValueVector::iterator i = seq.begin(); i != seq.end(); ++i, dynany->next()) {		\
			CORBA::Any any;																			\
			try { convert(&any, &*i, type); }														\
			catch(const ConversionException&) { throw ConversionException(_typeStr); }				\
			dynany->current_component()->from_any(any);												\
		}																							\
		*output = *dynany->to_any();																\
		dynany->destroy();																			\
	}

#define SET_SEQUENCE_LENGTH_CODE	dynany->set_length((CORBA::ULong) seq.size())
#define EMPTY_CODE

TO_CORBA_IMPLEMENT_NUMERIC_INDEX_CONVERSION(to_sequence, DynSequence, "sequence", SET_SEQUENCE_LENGTH_CODE)
TO_CORBA_IMPLEMENT_NUMERIC_INDEX_CONVERSION(to_array, DynArray, "array", EMPTY_CODE)

void ToCorbaConverter::to_struct(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {
	if (input->Type() != DeltaValue_Table)
		throw ConversionException("struct");

	DeltaTable* table = input->ToTable();
	DynamicAny::DynStruct_var dyn_struct =
		DynamicAny::DynStruct::_narrow(GetDynAnyFactory()->create_dyn_any_from_type_code(tc));

	for (CORBA::ULong i = 0; i < tc->member_count(); ++i, dyn_struct->next()) {
		DeltaValue tindex, tdata;
		tindex.FromString(tc->member_name(i));
		table->Get(tindex, &tdata);
		if (tdata.Type() == DeltaValue_Nil)
			throw ConversionException("struct");
		else {
			CORBA::Any any;
			try { convert(&any, &tdata, tc->member_type(i)); }
			catch(const ConversionException&) { throw ConversionException("struct"); }
			dyn_struct->current_component()->from_any(any);
		}
	}
	*output = *dyn_struct->to_any();
	dyn_struct->destroy();
}

void ToCorbaConverter::to_union_helper(CORBA::Any* output, DeltaValue* value, CORBA::TypeCode_ptr tc, unsigned index) {
	DynamicAny::DynAnyFactory_ptr dynany_factory = GetDynAnyFactory();
	CORBA::Any any;
	try { convert(&any, value, tc->member_type(index)); }
	catch(const ConversionException&) { throw ConversionException("union"); }

	DynamicAny::DynUnion_var dyn_union =
		DynamicAny::DynUnion::_narrow(dynany_factory->create_dyn_any_from_type_code(tc));

	CORBA::Any* dvalue = tc->member_label(index);
	CORBA::Any disc;
	if (dvalue->type()->kind() == CORBA::tk_octet) {	//default case
		UnionDefaultValueGenerationMap::const_iterator i =
			unionDefaultValues.find(tc->discriminator_type()->kind());
		DASSERT(i != unionDefaultValues.end());
		(*i->second)(&disc, tc);
	}
	else
		disc = *dvalue;

	DynamicAny::DynAny_var dyn_disc = dynany_factory->create_dyn_any(disc);
	dyn_union->set_discriminator(dyn_disc);
	dyn_union->member()->from_any(any);
	*output = *dyn_union->to_any();

	dyn_disc->destroy();
	dyn_union->destroy();
}

void ToCorbaConverter::to_union(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {
	try {
		DynamicAny::DynAnyFactory_ptr dynany_factory = GetDynAnyFactory();
		if (input->Type() == DeltaValue_Table) {
			DeltaTable* table = input->ToTable();
			DeltaValue type, value;
			table->Get("type", &type);
			table->Get("value", &value);
			if (type.IsNil()) {
				if (value.IsNil()) {	//no data means union with no active member
					DynamicAny::DynUnion_var dyn_union =
						DynamicAny::DynUnion::_narrow(dynany_factory->create_dyn_any_from_type_code(tc));
					//BUG: TAO doesn't handle set_to_no_active_member correctly
					//(same problem as set_to_default_member but there is no workaround for this)
					//The call will succeed, but the conversion to any will throw a marshal exception.
					//Anything that we can do for this?
					dyn_union->set_to_no_active_member();
					*output = *dyn_union->to_any();
					dyn_union->destroy();
					return;
				}
				else {
					to_union_helper(output, &value, tc, tc->default_index());
					return;
				}
			}
			else if (type.Type() == DeltaValue_String && !value.IsNil())
				for (CORBA::ULong i = 0; i < tc->member_count(); ++i)
					if (type.ToString() == tc->member_name(i)) {
						to_union_helper(output, &value, tc, i);
						return;
					}
		}
	}
	catch(const CORBA::Exception&) {}	//upon any corba exception we have a conversion error
	throw ConversionException("union");
}

void ToCorbaConverter::to_enum(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {
	if (input->Type() != DeltaValue_String || input->GetTypeTag() != "enum")
		throw ConversionException("enum");

	std::string name = input->ToString();
	std::string::size_type pos = name.rfind("::");
	if (pos != std::string::npos) {
		const std::string scopedName = name.substr(0, pos + 2) + tc->name();
		CORBA::Contained_ptr contained;
		CORBA::EnumDef_ptr enumDef;
		if (CORBA::is_nil(contained = corba::GetIFR()->lookup(scopedName.c_str())) ||
			CORBA::is_nil(enumDef = CORBA::EnumDef::_narrow(contained)) ||
			!enumDef->type()->equivalent(tc)
		)
			throw ConversionException("enum");
		name = name.substr(pos + 2);
	}
	for (CORBA::ULong i = 0; i < tc->member_count(); ++i) {
		if (name == tc->member_name(i)) {
			DynamicAny::DynEnum_var dyn_enum =
				DynamicAny::DynEnum::_narrow(GetDynAnyFactory()->create_dyn_any_from_type_code(tc));
			dyn_enum->set_as_string(tc->member_name(i));
			*output = *dyn_enum->to_any();
			dyn_enum->destroy();
			return;
		}
	}
	throw ConversionException("enum");
}

//**********************************************************************

void ToCorbaConverter::to_alias(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {
	try { convert(output, input, tc->content_type()); }
	catch(const ConversionException&) { throw ConversionException("alias"); }
}

void ToCorbaConverter::to_except(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {
	try { to_struct(output, input, tc); }
	catch(const ConversionException&) { throw ConversionException("except"); }
}

void ToCorbaConverter::to_any(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	CORBA::Any any;
	try {
		switch(input->Type()) {
			case DeltaValue_Number:
			{
				TypecodeMapping::const_iterator i = 
					numberToAnyConversions.find(input->GetTypeTag());
				CORBA::TypeCode_ptr tc = i == numberToAnyConversions.end() ?
					CORBA::_tc_double : i->second;
				convert(&any, input, tc);
				break;
			}
			case DeltaValue_String:
			{
				CORBA::TypeCode_ptr tc;
				const std::string& value = input->ToString();
				const std::string& tag = input->GetTypeTag();
				TypecodeMapping::const_iterator charConversionIter;
				if (value.size() == 1 &&
					((charConversionIter = charToAnyConversions.find(tag)) != charToAnyConversions.end() || tag.empty())
				)
					tc = charConversionIter == charToAnyConversions.end() ? CORBA::_tc_char : charConversionIter->second;
				else {
					TypecodeMapping::const_iterator i = stringToAnyConversions.find(tag);
					tc = i == stringToAnyConversions.end() ? CORBA::_tc_string : i->second;
				}
				convert(&any, input, tc);
				break;
			}
			case DeltaValue_Bool:		convert(&any, input, CORBA::_tc_boolean);	break;
			case DeltaValue_Table:	
			{
				CORBA::TypeCode_ptr tc = TableStructure::ExtractTypeCode(input);
				if (CORBA::is_nil(tc))
					throw ConversionException("any");
				else {
					convert(&any, input, tc);
					CORBA::release(tc);
				}
				break;
			}
			case DeltaValue_ExternId:	convert(&any, input, CORBA::_tc_Object);	break;
			case DeltaValue_Nil:		any <<= CORBA::Object::_nil();				break;
			default:					throw ConversionException("any");
		}
	}
	catch(const ConversionException&) { throw ConversionException("any"); }
	*output <<= any;
}

void ToCorbaConverter::to_objref(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr) {
	for(ObjectConversionFuncList::const_iterator i = objectConversions.begin(); i != objectConversions.end(); ++i)
		if ((**i)(output, input))
			return;
	throw ConversionException("objref");
}

void ToCorbaConverter::to_null(CORBA::Any* output, DeltaValue*, CORBA::TypeCode_ptr)
	{ *output <<= CORBA::Object::_nil(); }

void ToCorbaConverter::to_TypeCode(CORBA::Any* output, DeltaValue*, CORBA::TypeCode_ptr) {
	DASSERT(false);
	//TODO implement this
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ToCorbaConverter::convert(CORBA::Any* output, DeltaValue* input, CORBA::TypeCode_ptr tc) {
	DASSERT(tc->kind() != CORBA::tk_void);
	ConversionMap::const_iterator i = conversions.find(tc->kind());
	DASSERT(i != conversions.end());
	(*i->second)(output, input, tc);
}

void ToCorbaConverter::AddObjectConversion (ObjectConversionFunc f) { objectConversions.push_back(f); }
void ToCorbaConverter::RemoveObjectConversion (ObjectConversionFunc f) { objectConversions.remove(f); }

void ToCorbaConverter::Initialize(void) {
#define INITIALIZE_TYPECODE_CONVERSIONS(type)	conversions[CORBA::tk_##type] = to_##type

	INITIALIZE_TYPECODE_CONVERSIONS(short);
	INITIALIZE_TYPECODE_CONVERSIONS(long);
	INITIALIZE_TYPECODE_CONVERSIONS(ushort);
	INITIALIZE_TYPECODE_CONVERSIONS(ulong);
	INITIALIZE_TYPECODE_CONVERSIONS(float);
	INITIALIZE_TYPECODE_CONVERSIONS(double);
	INITIALIZE_TYPECODE_CONVERSIONS(longlong);
	INITIALIZE_TYPECODE_CONVERSIONS(ulonglong);
	INITIALIZE_TYPECODE_CONVERSIONS(longdouble);
	INITIALIZE_TYPECODE_CONVERSIONS(boolean);
	INITIALIZE_TYPECODE_CONVERSIONS(string);
	INITIALIZE_TYPECODE_CONVERSIONS(wstring);
	INITIALIZE_TYPECODE_CONVERSIONS(char);
	INITIALIZE_TYPECODE_CONVERSIONS(wchar);
	INITIALIZE_TYPECODE_CONVERSIONS(octet);
	INITIALIZE_TYPECODE_CONVERSIONS(sequence);
	INITIALIZE_TYPECODE_CONVERSIONS(array);
	INITIALIZE_TYPECODE_CONVERSIONS(struct);
	INITIALIZE_TYPECODE_CONVERSIONS(union);
	INITIALIZE_TYPECODE_CONVERSIONS(enum);
	INITIALIZE_TYPECODE_CONVERSIONS(alias);
	INITIALIZE_TYPECODE_CONVERSIONS(except);
	INITIALIZE_TYPECODE_CONVERSIONS(any);
	INITIALIZE_TYPECODE_CONVERSIONS(objref);
	INITIALIZE_TYPECODE_CONVERSIONS(null);
	INITIALIZE_TYPECODE_CONVERSIONS(TypeCode);

#undef INITIALIZE_TYPECODE_CONVERSIONS

	unionDefaultValues[CORBA::tk_short]		= generate_default_union_value<CORBA::Short>;
	unionDefaultValues[CORBA::tk_ushort]	= generate_default_union_value<CORBA::UShort>;
	unionDefaultValues[CORBA::tk_long]		= generate_default_union_value<CORBA::Long>;
	unionDefaultValues[CORBA::tk_ulong]		= generate_default_union_value<CORBA::ULong>;
	unionDefaultValues[CORBA::tk_longlong]	= generate_default_union_value<CORBA::LongLong>;
	unionDefaultValues[CORBA::tk_ulonglong]	= generate_default_union_value<CORBA::ULongLong>;
	unionDefaultValues[CORBA::tk_char]		= generate_default_union_value<CORBA::Char>;
	unionDefaultValues[CORBA::tk_wchar]		= generate_default_union_value<CORBA::WChar>;
	unionDefaultValues[CORBA::tk_boolean]	= generate_default_union_value<CORBA::Boolean>;
	unionDefaultValues[CORBA::tk_enum]		= generate_default_union_value<CORBA::Long>;

	numberToAnyConversions["short"]			= CORBA::_tc_short;
	numberToAnyConversions["long"]			= CORBA::_tc_long;
	numberToAnyConversions["ushort"]		= CORBA::_tc_ushort;
	numberToAnyConversions["ulong"]			= CORBA::_tc_ulong;
	numberToAnyConversions["float"]			= CORBA::_tc_float;
	numberToAnyConversions["double"]		= CORBA::_tc_double;
	numberToAnyConversions["longlong"]		= CORBA::_tc_longlong;
	numberToAnyConversions["ulonglong"]		= CORBA::_tc_ulonglong;
	numberToAnyConversions["longdouble"]	= CORBA::_tc_longdouble;

	stringToAnyConversions["string"]		= CORBA::_tc_string;
	stringToAnyConversions["wstring"]		= CORBA::_tc_wstring;

	charToAnyConversions["char"]			= CORBA::_tc_char;
	charToAnyConversions["wchar"]			= CORBA::_tc_wchar;
}

void ToCorbaConverter::Cleanup(void) {
	conversions.clear();
	unionDefaultValues.clear();
	numberToAnyConversions.clear();
	stringToAnyConversions.clear();
	charToAnyConversions.clear();
}

/*
    tk_TypeCode,
    tk_Principal,
     
    tk_fixed,
    tk_value,
    tk_value_box,
    tk_native,
    tk_abstract_interface,
    tk_local_interface,
    tk_component,
    tk_home,
    tk_event
*/