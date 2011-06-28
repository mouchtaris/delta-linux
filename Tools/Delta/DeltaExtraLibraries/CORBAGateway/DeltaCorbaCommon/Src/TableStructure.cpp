#include "TableStructure.h"
#include "Delta.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

TableStructure::TypecodeMappings TableStructure::mappings;

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool TableStructure::FromTable(DeltaTable* table) {
	for(DeltaTable::const_iterator i = table->begin(); i != table->end(); ++i) {
		DeltaValue index, value;
		Index indexValue;
		i->GetIndex(&index);
		if (index.Type() == DeltaValue_Number)
			indexValue = Index(index.ToNumber());
		else if (index.Type() == DeltaValue_String)
			indexValue = Index(index.ToString());
		else
			return false;	//not a corba convertible structure

		Value valueValue;
		i->GetContent(&value);
		switch(value.Type()) {
			case DeltaValue_Number:	valueValue = Value(ValueTypeNumber);	break;
			case DeltaValue_String:	valueValue = Value(ValueTypeString);	break;
			case DeltaValue_Bool:		valueValue = Value(ValueTypeBool);		break;

			case DeltaValue_Table:		{
											TableStructure* table = new TableStructure;
											if (!table->FromTable(value.ToTable())) {
												delete table;
												structure.clear();
												return false;
											}
											valueValue = Value(table);
										}
										break;

			case DeltaValue_ExternId:	{
											structure.clear();
											return false;
											//TODO implement this
											//std::string type;
											//void *val;
											//CorbaObjectWrapper *corbaObject;
											//if (value.Type() == DeltaValue_ExternId && (val = value.ToExternId(type)) &&
											//	type == CORBA_OBJECT_TYPE_STR && (corbaObject = CORBA_OBJECT_VAL(val)))
											//	valueValue = Value(corbaObject->GetCorbaObject()->_interface_repository_id());
											//else {
											//	structure.clear();
											//	return false;	//not a corba convertible extern id
											//}
										}
										break;
			default:					structure.clear();
										return false;
		}
		structure[indexValue] = valueValue;
	}
	return CheckStruct() || CheckArray();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//"tk_null", "tk_void", "tk_short", "tk_long", "tk_ushort", "tk_ulong", "tk_float", "tk_double",
//"tk_boolean", "tk_char", "tk_octet", "tk_any", "tk_TypeCode", "tk_Principal", "tk_objref",
//"tk_struct", "tk_union", "tk_enum", "tk_string", "tk_sequence", "tk_array", "tk_alias",
//"tk_except", "tk_longlong", "tk_ulonglong", "tk_longdouble", "tk_wchar", "tk_wstring",
//"tk_fixed", "tk_value", "tk_value_box", "tk_native", "tk_abstract_interface",
//"tk_local_interface", "tk_component", "tk_home", "tk_event"

bool TableStructure::FromTypecode(CORBA::TypeCode_ptr tc) {
	//CORBA::tk_struct
	const CORBA::ULong count = tc->member_count();
	for(CORBA::ULong i = 0; i < count; ++i) {
		Value value;
		CORBA::TCKind kind = TAO_DynAnyFactory::unalias(tc->member_type(i));
		switch(kind) {
			case CORBA::tk_short:		//Fallback
			case CORBA::tk_long:		//Fallback
			case CORBA::tk_ushort:		//Fallback
			case CORBA::tk_ulong:		//Fallback
			case CORBA::tk_float:		//Fallback
			case CORBA::tk_double:		//Fallback
			case CORBA::tk_longlong:	//Fallback
			case CORBA::tk_ulonglong:	//Fallback
			case CORBA::tk_longdouble:	//Fallback
			case CORBA::tk_octet:		value = Value(ValueTypeNumber);	break;

			case CORBA::tk_char:		//Fallback
			case CORBA::tk_wchar:		//Fallback
			case CORBA::tk_string:		//Fallback
			case CORBA::tk_wstring:		value = Value(ValueTypeString);	break;

			case CORBA::tk_boolean:		value = Value(ValueTypeBool);	break;

			case CORBA::tk_struct:		//Fallback
			case CORBA::tk_except:		//Fallback
			case CORBA::tk_union:		//Fallback
			case CORBA::tk_enum:		
			

			case CORBA::tk_sequence:
			case CORBA::tk_array:

			case CORBA::tk_any:			//value is by default any
										break;

			case CORBA::tk_TypeCode:
			case CORBA::tk_objref:

			default:					structure.clear();	//maybe throw unimplemented exception?
										break;
		}
		structure[Index(tc->member_name(i))] = value;
	}
	//
	//ifr->describe_contents(
	//CORBA::Container::DescriptionSeq *descriptions = const_cast<CORBA::Container&>(container).describe_contents(dk, true, -1);

	//for (CORBA::ULong i = 0; i < descriptions->length (); ++i) {
	//	DeltaValue item;
	//	convert(&item, &(*descriptions)[i].value, dk);
	//	value->Set((DeltaNumberValueType) i, item);
	//}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool TableStructure::CheckArray(void) {
	bool anyType = false;
	Value arrayType;
	unsigned count = 0;
	for(Structure::const_iterator i = structure.begin(); i != structure.end(); ++i) {
		if (i->first.isString || i->first.value.numberVal != (DeltaNumberValueType) count++)
			return false;
		if (i == structure.begin())
			arrayType = i->second;
		else if (arrayType != i->second)
			anyType = true;
	}
	isArray = true;
	this->arrayType = anyType ? Value() : arrayType;
	arraySize = count;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool TableStructure::CheckStruct(void) {
	for(Structure::const_iterator i = structure.begin(); i != structure.end(); ++i)
		if (!i->first.isString)
			return false;
	isArray = false;	//struct
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool TableStructure::operator ==(const TableStructure& table) const {
	Structure::const_iterator i, j;
	for (i = structure.begin(), j = table.structure.begin(); i != structure.end() && j != table.structure.end(); ++i, ++j)
		if (i->first != j->first || i->second != j->second)
			return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CORBA::TypeCode_ptr TableStructure::ExtractTypeCode (const Value& value) {
	switch(value.type) {
		case ValueTypeNumber:	return CORBA::_tc_double;
		case ValueTypeString:	return CORBA::_tc_string;
		case ValueTypeBool:		return CORBA::_tc_boolean;
		case ValueTypeTable:	return ExtractTypeCode(*value.value.tableVal, "");
		case ValueTypeExtern:	return CORBA::_tc_Object;	//TODO typecode as well, based on value.value.externVal
		case ValueTypeAny:		return CORBA::_tc_any;
		default:				DASSERT(false); return CORBA::TypeCode::_nil();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CORBA::TypeCode_ptr TableStructure::ExtractTypeCode (const TableStructure& tableStructure, const std::string& tag) {
	if (tableStructure.IsArray()) {
		CORBA::TypeCode_var tc = ExtractTypeCode(tableStructure.arrayType);
		return CORBA::is_nil(tc) ? CORBA::TypeCode::_nil() : corba::GetORB()->create_array_tc(tableStructure.arraySize, tc);
	}
	else {
		std::list<CORBA::TypeCode_ptr> matches;
		for(TypecodeMappings::const_iterator i = mappings.begin(); i != mappings.end(); ++i)
			if (*i->second == tableStructure)
				matches.push_back(i->first);
		if (matches.empty())
			return CORBA::TypeCode::_nil();
		else if (matches.size() == 1)
			return matches.front();
		else if (tag.empty())	//ambiguous conversion
			return CORBA::TypeCode::_nil();
		else {
			//CORBA::Contained_ptr contained = corba::ifr->lookup(tag.c_str());
			//if (CORBA::is_nil(contained) {
			//	CORBA::ContainedSeq_ptr seq = corba::ifr->lookup_name(tag.c_str(), -1, , true);

			//}
			//CORBA::StructDef_var structDef = CORBA::StructDef::_narrow(contained)
			//ConstantDef, ExceptionDef, AttributeDef, OperationDef, StructDef, UnionDef, EnumDef, AliasDef, TypedefDef
			//TODO implement this
			return CORBA::TypeCode::_nil();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CORBA::TypeCode_ptr TableStructure::ExtractTypeCode (DeltaValue* object) {
	DASSERT(object->Type() == DeltaValue_Table);
	TableStructure ts;
	if (ts.FromTable(object->ToTable()))
		return ExtractTypeCode(ts, object->GetTypeTag());
	else
		return CORBA::TypeCode::_nil();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool TableStructure::Initialize(void) {
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void TableStructure::Cleanup(void) {
}
