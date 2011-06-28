#include "ToDeltaConverter.h"
#include "ConversionException.h"

#include "Delta.h"

#include <stdlib.h>
#include <wchar.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace corba;

ToDeltaConverter::TypecodeConversionMap ToDeltaConverter::typecodeConversions;
ToDeltaConverter::DefinitionConversionMap ToDeltaConverter::definitionConversions;

ToDeltaConverter::ObjectConversionFunc ToDeltaConverter::clientConversion;
ToDeltaConverter::ObjectConversionFunc ToDeltaConverter::serverConversion;

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ToDeltaConverter::AddContainerField(DeltaTable* table, const std::string& name, const CORBA::Container& container, CORBA::DefinitionKind dk) {
	DeltaTable* value = DeltaObject::NewObject();
	table->Set(name, value);
	CORBA::Container::DescriptionSeq *descriptions = const_cast<CORBA::Container&>(container).describe_contents(dk, false, -1);
	unsigned unnamed = 0;
	for (CORBA::ULong i = 0; i < descriptions->length (); ++i) {
		DeltaValue item;
		try { convert(&item, &(*descriptions)[i].value, dk); }
		catch(const ConversionException&) { continue; } //only for the reflection api, so suppress conversion errors
		DeltaValue name;
		if(item.Type() == DeltaValue_Table && item.ToTable()->Get("name", &name) && !name.IsNil())
			value->Set(name, item);
		else
			value->Set((DeltaNumberValueType) unnamed++, item);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(_name, _type, _typeStr)	\
	void ToDeltaConverter::_name(DeltaValue* output, CORBA::Any* input) {	\
		_type val;															\
		if ((*input) >>= val) {												\
			output->FromNumber((DeltaNumberValueType) val);					\
			output->SetTypeTag(_typeStr);									\
		}																	\
		else																\
			throw ConversionException(_typeStr);							\
	}

TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_short, CORBA::Short, "short")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_long, CORBA::Long, "long")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_ushort, CORBA::UShort, "ushort")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_ulong, CORBA::ULong, "ulong")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_float, CORBA::Float, "float")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_double, CORBA::Double, "double")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_longlong, CORBA::LongLong, "longlong")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_ulonglong, CORBA::ULongLong, "ulonglong")
TO_DELTA_IMPLEMENT_ARITHMETIC_CONVERSION(from_tc_longdouble, CORBA::LongDouble, "longdouble")

//**********************************************************************

void ToDeltaConverter::from_tc_boolean(DeltaValue* output, CORBA::Any* input) {
	bool val;
	if ((*input) >>= val)
		output->FromBool(val);
	else
		throw ConversionException("boolean");
}

void ToDeltaConverter::from_tc_string(DeltaValue* output, CORBA::Any* input) {
	const char *conversionFunc = "string";
	const char* str = NULL;
	if ((*input) >>= str) {
		output->FromString(str);
		output->SetTypeTag(conversionFunc);
	}
	else
		throw ConversionException(conversionFunc);
}

void ToDeltaConverter::from_tc_wstring(DeltaValue* output, CORBA::Any* input) {
	const char *conversionFunc = "wstring";
	const CORBA::WChar *wstr = (const CORBA::WChar*) 0;
	if ((*input) >>= wstr) {
		size_t len = wcslen(wstr);
		char *str = new char[len + 1];
		wcstombs(str, wstr, len);
		str[len] = '\0';
		output->FromString(str);
		output->SetTypeTag(conversionFunc);
		delete[] str;
	}
	else
		throw ConversionException(conversionFunc);
}

void ToDeltaConverter::from_tc_char(DeltaValue* output, CORBA::Any* input) {
	const char *conversionFunc = "char";
	CORBA::Char c;
	if ((*input) >>= c) {
		output->FromString(std::string(1, c));
		output->SetTypeTag(conversionFunc);
	}
	else
		throw ConversionException(conversionFunc);
}

void ToDeltaConverter::from_tc_wchar(DeltaValue* output, CORBA::Any* input) {
	const char *conversionFunc = "wchar";
	CORBA::WChar wc;
	if ((*input) >>= wc) {
		char c;
		wcstombs(&c, &wc, 1);
		output->FromString(std::string(1, c));
		output->SetTypeTag(conversionFunc);
	}
	else
		throw ConversionException(conversionFunc);
}

void ToDeltaConverter::from_tc_octet(DeltaValue* output, CORBA::Any* input) {
	CORBA::Octet oct;
	if ((*input) >>= CORBA::Any::to_octet(oct))
		output->FromNumber((DeltaNumberValueType) oct);
	else
		throw ConversionException("octet");
}

//**********************************************************************

template<class T>
void ToDeltaConverter::numericIndexConversion(DeltaValue* output, CORBA::Any* input, const std::string& typeStr) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	CORBA::TypeCode_ptr tc = input->type();
	typename T::_var_type dynany = T::_narrow(GetDynAnyFactory()->create_dyn_any_from_type_code(tc));
	dynany->from_any(*input);
	for (CORBA::ULong i = 0; i < dynany->component_count(); ++i, dynany->next()) {
		DeltaValue data;
		try { convert(&data, dynany->current_component()->to_any()); }
		catch(const ConversionException&) {
			dynany->destroy();
			output->Undefine();
			throw ConversionException(typeStr);
		}
		table->Set((DeltaNumberValueType) i, data);
	}																						
	dynany->destroy();
}

void ToDeltaConverter::from_tc_sequence(DeltaValue* output, CORBA::Any* input)
	{ numericIndexConversion<DynamicAny::DynSequence>(output, input, "sequence"); }

void ToDeltaConverter::from_tc_array(DeltaValue* output, CORBA::Any* input)
	{ numericIndexConversion<DynamicAny::DynArray>(output, input, "array"); }

//**********************************************************************

void ToDeltaConverter::from_tc_struct(DeltaValue* output, CORBA::Any* input) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	CORBA::TypeCode_ptr tc = input->type();
	DeltaValue data;
	try { convert(&data, tc); }
	catch(const ConversionException&) {
		output->Undefine();
		throw ConversionException("struct");
	}
	table->Set("type", data);
	DynamicAny::DynStruct_var dyn_struct =
		DynamicAny::DynStruct::_narrow(GetDynAnyFactory()->create_dyn_any_from_type_code(tc));
	dyn_struct->from_any(*input);
	for (CORBA::ULong i = 0; i < tc->member_count(); ++i, dyn_struct->next()) {
		try { convert(&data, dyn_struct->current_component()->to_any()); }
		catch(const ConversionException&) {
			dyn_struct->destroy();
			output->Undefine();
			throw ConversionException("struct");
		}
		table->Set(tc->member_name(i), data);
	}
	dyn_struct->destroy();
}

void ToDeltaConverter::from_tc_union(DeltaValue* output, CORBA::Any* input) {
	DynamicAny::DynAnyFactory_ptr dynany_factory = GetDynAnyFactory();
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	CORBA::TypeCode_ptr tc = input->type();
	DynamicAny::DynUnion_var dyn_union =
		DynamicAny::DynUnion::_narrow(dynany_factory->create_dyn_any_from_type_code(tc));
	dyn_union->from_any(*input);
	if (dyn_union->has_no_active_member()) {	//if no active member set just return empty table
		dyn_union->destroy();
		return;
	}
	DeltaValue type, value;
	DynamicAny::DynAny_var dyn_disc = dynany_factory->create_dyn_any(*dyn_union->get_discriminator()->to_any());
	CORBA::ULong i;
	for (i = 0; i < tc->member_count(); ++i) {
		DynamicAny::DynAny_var dynany = dynany_factory->create_dyn_any(*tc->member_label(i));
		if (dyn_disc->equal(dynany)) {
			table->Set("type", tc->member_name(i));
			break;
		}
		dynany->destroy();
	}
	if (i == tc->member_count())	//if no match yet it has to be the default
		table->Set("type", tc->member_name(tc->default_index()));
	dyn_disc->destroy();
		
	CORBA::Any* any = dyn_union->member()->to_any();
	try { convert(&value, any); }
	catch(const ConversionException&) {
		dyn_union->destroy();
		output->Undefine();
		throw ConversionException("union");
	}
	table->Set("value", value);
	dyn_union->destroy();
}

void ToDeltaConverter::from_tc_enum(DeltaValue* output, CORBA::Any* input) {
	CORBA::TypeCode_ptr tc = input->type();
	DynamicAny::DynEnum_var dyn_enum =
		DynamicAny::DynEnum::_narrow(GetDynAnyFactory()->create_dyn_any_from_type_code(tc));
	dyn_enum->from_any(*input);
	const std::string name = dyn_enum->get_as_string();
	//TODO return the most limited non-ambiguous value
	output->FromString(name);
	dyn_enum->destroy();
}

//**********************************************************************

void ToDeltaConverter::from_tc_alias(DeltaValue* output, CORBA::Any* input) {
	CORBA::Any any(*input);
	any.type(input->type()->content_type());
	try { convert(output, &any); }
	catch(const ConversionException&) { throw ConversionException("alias"); }
}

void ToDeltaConverter::from_tc_except(DeltaValue* output, CORBA::Any* input) {
	try { from_tc_struct(output, input); }	//exceptions and structs have the same structure
	catch(const ConversionException&) { throw ConversionException("except"); }
	DASSERT(output->Type() == DeltaValue_Table);
}

void ToDeltaConverter::from_tc_any(DeltaValue* output, CORBA::Any* input) {
	CORBA::Any* any;
	bool conversionError = false;
	if ((*input) >>= ((const CORBA::Any *&)any))
		try { convert(output, any); }
		catch(const ConversionException&) { conversionError = true; }
	else
		conversionError = true;
	if (conversionError)
		throw ConversionException("any");
}

void ToDeltaConverter::from_tc_objref(DeltaValue* output, CORBA::Any* input) {
     CORBA::Object_ptr obj;
	 bool conversionError = false;
	 if ((*input) >>= CORBA::Any::to_object(obj))
		 try { convert(output, obj); }
		catch(const ConversionException&) { conversionError = true; }
	else
		conversionError = true;
	if (conversionError)
		throw ConversionException("objref");
}

void ToDeltaConverter::from_tc_null(DeltaValue* output, CORBA::Any*) { output->FromNil(); }

void ToDeltaConverter::from_tc_TypeCode(DeltaValue* output, CORBA::Any* input) {
	CORBA::TypeCode_ptr tc;
	 bool conversionError = false;
	if ((*input) >>= tc)
		try { convert(output, tc); }
		catch(const ConversionException&) { conversionError = true; }
	else
		conversionError = true;
	if (conversionError)
		throw ConversionException("TypeCode");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::AttributeDescription& attribute) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddBasicContainedFields(table, attribute);
	AddNamedField(table, "type", attribute, &CORBA::AttributeDescription::type);
	AddNamedField(table, "mode", attribute, &CORBA::AttributeDescription::mode);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::ConstantDescription& constant) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddBasicContainedFields(table, constant);
	AddNamedField(table, "type", constant, &CORBA::ConstantDescription::type);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::ExceptionDescription& exception) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddBasicContainedFields(table, exception);
	AddNamedField(table, "type", exception, &CORBA::ExceptionDescription::type);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::InterfaceDef& interface) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddContainerField(table, "constants", interface, CORBA::dk_Constant);
	AddContainerField(table, "structs", interface, CORBA::dk_Struct);
	AddContainerField(table, "unions", interface, CORBA::dk_Union);
	AddContainerField(table, "enums", interface, CORBA::dk_Enum);
	AddContainerField(table, "aliases", interface, CORBA::dk_Alias);
	AddContainerField(table, "exceptions", interface, CORBA::dk_Exception);
	AddContainerField(table, "attributes", interface, CORBA::dk_Attribute);
	AddContainerField(table, "operations", interface, CORBA::dk_Operation);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::ModuleDef& module) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddContainerField(table, "constants", module, CORBA::dk_Constant);
	AddContainerField(table, "structs", module, CORBA::dk_Struct);
	AddContainerField(table, "unions", module, CORBA::dk_Union);
	AddContainerField(table, "enums", module, CORBA::dk_Enum);
	AddContainerField(table, "aliases", module, CORBA::dk_Alias);
	AddContainerField(table, "exceptions", module, CORBA::dk_Exception);
	AddContainerField(table, "operations", module, CORBA::dk_Operation);
	AddContainerField(table, "interfaces", module, CORBA::dk_Interface);
	AddContainerField(table, "modules", module, CORBA::dk_Module);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::OperationDescription& operation) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddBasicContainedFields(table, operation);
	AddNamedField(table, "result", operation, &CORBA::OperationDescription::result);
	AddNamedField(table, "mode", operation, &CORBA::OperationDescription::mode);

	DeltaTable* contexts = DeltaObject::NewObject();
	table->Set("contexts", contexts);
	for (CORBA::ULong i = 0; i < operation.contexts.length(); ++i)
		contexts->Set((DeltaNumberValueType) i, std::string(operation.contexts[i]));

	DeltaTable* parameters = DeltaObject::NewObject();
	table->Set("parameters", parameters);
	for (CORBA::ULong i = 0; i < operation.parameters.length(); ++i) {
		DeltaValue params;
		try { convert(&params, operation.parameters[i]); }
		catch(const ConversionException&) { DASSERT(false); }	//should never have a conversion exception here
		parameters->Set((DeltaNumberValueType) i, params);
	}

	DeltaTable* exceptions = DeltaObject::NewObject();
	table->Set("exceptions", exceptions);
	for (CORBA::ULong i = 0; i < operation.exceptions.length(); ++i) {
		DeltaValue ex;
		try { convert(&ex, operation.exceptions[i]); }
		catch(const ConversionException&) { DASSERT(false); }	//should never have a conversion exception here
		exceptions->Set((DeltaNumberValueType) i, ex);
	}
}

//**********************************************************************

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::TypeDescription& typeDef) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddBasicContainedFields(table, typeDef);
	AddNamedField(table, "type", typeDef, &CORBA::TypeDescription::type);
}

//**********************************************************************

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::PrimitiveDef& primitiveDef) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	AddNamedFunctionField(table, "type", const_cast<CORBA::PrimitiveDef&>(primitiveDef), &CORBA::PrimitiveDef::type);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::StringDef& stringDef) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	CORBA::StringDef& def = const_cast<CORBA::StringDef&>(stringDef);
	AddNamedFunctionField(table, "type", def, &CORBA::StringDef::type);
	table->Set("bound", (DeltaNumberValueType) def.bound());
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::WstringDef& wstringDef) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	CORBA::WstringDef& def = const_cast<CORBA::WstringDef&>(wstringDef);
	AddNamedFunctionField(table, "type", def, &CORBA::StringDef::type);
	table->Set("bound", (DeltaNumberValueType) def.bound());
}

//**********************************************************************

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::ParameterDescription& param) {
	DeltaTable* table = DeltaObject::NewObject();
	output->FromTable(table);
	table->Set("name", std::string(param.name));
	AddNamedField(table, "type", param, &CORBA::ParameterDescription::type);
	AddNamedField(table, "mode", param, &CORBA::ParameterDescription::mode);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::OperationMode& mode) {
	if (mode == CORBA::OP_NORMAL)
		output->FromString("normal");
	else if (mode == CORBA::OP_ONEWAY)
		output->FromString("oneway");
	else
		DASSERT(false);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::AttributeMode& mode) {
	if (mode == CORBA::ATTR_NORMAL)
		output->FromString("normal");
	else if (mode == CORBA::ATTR_READONLY)
		output->FromString("readonly");
	else
		DASSERT(false);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::ParameterMode& mode) {
	if (mode == CORBA::PARAM_IN)
		output->FromString("in");
	else if (mode == CORBA::PARAM_OUT)
		output->FromString("out");
	else if (mode == CORBA::PARAM_INOUT)
		output->FromString("inout");
	else
		DASSERT(false);
}

void ToDeltaConverter::convert(DeltaValue* output, const CORBA::TypeCode_ptr& type) {
	static const char* typeCodeStrs[] = {
		"tk_null", "tk_void", "tk_short", "tk_long", "tk_ushort", "tk_ulong", "tk_float", "tk_double",
		"tk_boolean", "tk_char", "tk_octet", "tk_any", "tk_TypeCode", "tk_Principal", "tk_objref",
		"tk_struct", "tk_union", "tk_enum", "tk_string", "tk_sequence", "tk_array", "tk_alias",
		"tk_except", "tk_longlong", "tk_ulonglong", "tk_longdouble", "tk_wchar", "tk_wstring",
		"tk_fixed", "tk_value", "tk_value_box", "tk_native", "tk_abstract_interface",
		"tk_local_interface", "tk_component", "tk_home", "tk_event"
	};
	DASSERT((util_ui32)type->kind() < uarraysize(typeCodeStrs));
	CORBA::TCKind tc = type->kind();
	if (tc == CORBA::tk_null		|| tc == CORBA::tk_void			|| tc == CORBA::tk_short	||
		tc == CORBA::tk_long		|| tc == CORBA::tk_ushort		|| tc == CORBA::tk_ulong	||
		tc == CORBA::tk_float		|| tc == CORBA::tk_double		|| tc == CORBA::tk_boolean	||
		tc == CORBA::tk_char		|| tc == CORBA::tk_octet		|| tc == CORBA::tk_any		||
		tc == CORBA::tk_TypeCode	|| tc == CORBA::tk_Principal	|| tc == CORBA::tk_longlong	||
		tc == CORBA::tk_ulonglong	|| tc == CORBA::tk_longdouble	|| tc == CORBA::tk_wchar
	)
		output->FromString(typeCodeStrs[tc]);
	else {
		DeltaTable* table = DeltaObject::NewObject();
		output->FromTable(table);
		table->Set("type", typeCodeStrs[tc]);

		if (tc == CORBA::tk_objref			|| tc == CORBA::tk_struct	|| tc == CORBA::tk_union				||
			tc == CORBA::tk_enum			|| tc == CORBA::tk_alias	|| tc == CORBA::tk_value				||
			tc == CORBA::tk_value_box		|| tc == CORBA::tk_native	|| tc == CORBA::tk_abstract_interface	||
			tc == CORBA::tk_local_interface	|| tc == CORBA::tk_except	|| tc == CORBA::tk_component			||
			tc == CORBA::tk_home			|| tc == CORBA::tk_event
		) {
			table->Set("id", type->id());
			table->Set("name", type->name());
		}
		if (tc == CORBA::tk_struct	|| tc == CORBA::tk_union	|| tc == CORBA::tk_enum ||
			tc == CORBA::tk_value	|| tc == CORBA::tk_except	|| tc == CORBA::tk_event
		) {
			DeltaTable* members = DeltaObject::NewObject();
			table->Set("members", members);
			const CORBA::ULong count = type->member_count();
			for(CORBA::ULong i = 0; i < count; ++i) {
				DeltaTable* member = DeltaObject::NewObject();
				members->Set(type->member_name(i), member);
				member->Set("name", type->member_name(i));
				if (tc != CORBA::tk_enum) {
					DeltaValue memberType;
					try {
						convert(&memberType, type->member_type(i));
						member->Set("type", memberType);
					}
					catch(const ConversionException&) {} // ignore possible conversion errors
				}
				if (tc == CORBA::tk_union) {
					try {
						DeltaValue memberLabel;
						CORBA::Any *label = type->member_label(i);
						if (label->type()->kind() == CORBA::tk_octet)	//default case
							memberLabel.FromString("default");
						else
							convert(&memberLabel, label);
						member->Set("label", memberLabel);
					}
					catch(const ConversionException&) {} // ignore possible conversion errors
				}
				else if (tc == CORBA::tk_value || tc == CORBA::tk_event) {
					member->Set("visibility", (DeltaNumberValueType) type->member_visibility(i));
				}
			}
			if (tc == CORBA::tk_union)
				table->Set("default_index", (DeltaNumberValueType) type->default_index());
			else if (tc == CORBA::tk_value || tc == CORBA::tk_event) {
				table->Set("type_modifier", (DeltaNumberValueType) type->type_modifier());
				DeltaValue concreteBaseType;
				try {
					convert(&concreteBaseType, type->concrete_base_type());
					table->Set("concrete_base_type", concreteBaseType);
				}
				catch(const ConversionException&) {} // ignore possible conversion errors
			}
		}
		if (tc == CORBA::tk_string		|| tc == CORBA::tk_wstring	||
			tc == CORBA::tk_sequence	|| tc == CORBA::tk_array
		)
			table->Set("length", (DeltaNumberValueType) type->length());
		if (tc == CORBA::tk_sequence	|| tc == CORBA::tk_array ||
			tc == CORBA::tk_value_box	|| tc == CORBA::tk_alias
		)
			try {
				DeltaValue contentType;
				convert(&contentType, type->content_type());
				table->Set("content_type", contentType);
			}
			catch(const ConversionException&) {} // ignore possible conversion errors
		if (tc == CORBA::tk_fixed) {
			table->Set("digits", (DeltaNumberValueType) type->fixed_digits());
			table->Set("scale", (DeltaNumberValueType) type->fixed_scale());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ToDeltaConverter::convert(DeltaValue* output, CORBA::Object_ptr obj) {
	ObjectConversionFunc conversionFuncs[] = { serverConversion, clientConversion };
	for(unsigned i = 0; i < uarraysize(conversionFuncs); ++i)
		if (conversionFuncs[i] && (*conversionFuncs[i])(output, obj))
			return;
	//We only get here when having only a serverConversion that failed.
	//Can't just return undef or nil because we won't be able to get the original object back.
	//TODO check this again, and probably use CorbaClient to return an externid (even without binding the obj info)
	throw ConversionException("objref");
}

void ToDeltaConverter::convert(DeltaValue* output, CORBA::Any* input) {
	CORBA::TCKind kind = input->type()->kind();
	TypecodeConversionMap::const_iterator i = typecodeConversions.find(kind);
	DASSERT(i != typecodeConversions.end());
	(*i->second)(output, input);
}

void ToDeltaConverter::convert(DeltaValue* output, CORBA::Any* input, CORBA::DefinitionKind dk) {
	DefinitionConversionMap::const_iterator i = definitionConversions.find(dk);
	DASSERT(i != definitionConversions.end());
	(*i->second)(output, input);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ToDeltaConverter::SetClientObjectConversion (ObjectConversionFunc f) { clientConversion = f; }
void ToDeltaConverter::SetServerObjectConversion (ObjectConversionFunc f) { serverConversion = f; }

void ToDeltaConverter::Initialize(void) {
#define INITIALIZE_TYPECODE_CONVERSIONS(type)	typecodeConversions[CORBA::tk_##type] = from_tc_##type

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

	definitionConversions[CORBA::dk_Attribute]	= from_dk<CORBA::AttributeDescription>;
	definitionConversions[CORBA::dk_Constant]	= from_dk<CORBA::ConstantDescription>;
	definitionConversions[CORBA::dk_Exception]	= from_dk<CORBA::ExceptionDescription>;
	definitionConversions[CORBA::dk_Interface]	= from_dk<CORBA::InterfaceDef>;
	definitionConversions[CORBA::dk_Module]		= from_dk<CORBA::ModuleDef>;
	definitionConversions[CORBA::dk_Operation]	= from_dk<CORBA::OperationDescription>;

	definitionConversions[CORBA::dk_Typedef]	= from_dk<CORBA::TypeDescription>;
	definitionConversions[CORBA::dk_Struct]		= from_dk<CORBA::TypeDescription>;
	definitionConversions[CORBA::dk_Union]		= from_dk<CORBA::TypeDescription>;
	definitionConversions[CORBA::dk_Enum]		= from_dk<CORBA::TypeDescription>;
	definitionConversions[CORBA::dk_Alias]		= from_dk<CORBA::TypeDescription>;

	definitionConversions[CORBA::dk_Primitive]	= from_dk<CORBA::PrimitiveDef>;
	definitionConversions[CORBA::dk_String]		= from_dk<CORBA::StringDef>;
	definitionConversions[CORBA::dk_Wstring]	= from_dk<CORBA::WstringDef>;

	//definitionConversions[CORBA::dk_Sequence] = from_dk_sequence;
	//definitionConversions[CORBA::dk_Array] = from_dk_array;
	//definitionConversions[CORBA::dk_Fixed] = from_dk_fixed;
	//definitionConversions[CORBA::dk_Value] = from_dk_value;
	//definitionConversions[CORBA::dk_ValueBox] = from_dk_value_box;
	//definitionConversions[CORBA::dk_Native] = from_dk_native;
	//definitionConversions[CORBA::dk_AbstractInterface] = from_dk_abstract_interface;
	//definitionConversions[CORBA::dk_LocalInterface] = from_dk_local_interface;

	clientConversion = serverConversion = (ObjectConversionFunc) 0;
}

void ToDeltaConverter::Cleanup(void) {
	typecodeConversions.clear();
	definitionConversions.clear();
}