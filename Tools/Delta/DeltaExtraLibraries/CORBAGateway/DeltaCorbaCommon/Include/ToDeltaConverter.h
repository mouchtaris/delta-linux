#ifndef TO_DELTA_CONVERTER_H
#define TO_DELTA_CONVERTER_H

#include "Corba.h"
#include <map>

class DeltaValue;
class DeltaTable;

class CORBACOMMON_CLASS ToDeltaConverter {
public:
	typedef bool (*ObjectConversionFunc)(DeltaValue* output, CORBA::Object_ptr obj);	//returns true if successful conversion
private:
	typedef void (*ConverterFunc)(DeltaValue* output, CORBA::Any* input);

	typedef std::map<CORBA::TCKind, ConverterFunc> TypecodeConversionMap;
	static TypecodeConversionMap typecodeConversions;

	typedef std::map<CORBA::DefinitionKind, ConverterFunc> DefinitionConversionMap;
	static DefinitionConversionMap definitionConversions;

	static ObjectConversionFunc clientConversion;
	static ObjectConversionFunc serverConversion;

	//**********************************************************************

	static void AddContainerField(DeltaTable* table, const std::string& name, const CORBA::Container& container, CORBA::DefinitionKind dk);
	
	template<class T> static void from_dk(DeltaValue* output, CORBA::Any* input) {
		T* definition;
		(*input) >>= definition;
		convert(output, *definition);
	}

	template<class T> static void AddBasicContainedFields(DeltaTable* table, const T& contained) {
		table->Set("name", std::string(contained.name));
		table->Set("id", std::string(contained.id));
		table->Set("defined_in", std::string(contained.defined_in));
		table->Set("version", std::string(contained.version));
	}

	template<class T, typename Ptr>
	static void AddNamedField(DeltaTable* table, const std::string& name, const T& object, Ptr memPtr) {
		DeltaValue value;
		convert(&value, object.*memPtr);
		table->Set(name, value);
	}
	template<class T, typename Ptr>
	static void AddNamedFunctionField(DeltaTable* table, const std::string& name, T& object, Ptr memPtr) {
		DeltaValue value;
		convert(&value, (object.*memPtr)());
		table->Set(name, value);
	}

	template<class T>
	static void numericIndexConversion(DeltaValue* output, CORBA::Any* input, const std::string& typeStr);

	//**********************************************************************

	static void from_tc_short		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_long		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_ushort		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_ulong		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_float		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_double		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_longlong	(DeltaValue* output, CORBA::Any* input);
	static void from_tc_ulonglong	(DeltaValue* output, CORBA::Any* input);
	static void from_tc_longdouble	(DeltaValue* output, CORBA::Any* input);

	static void from_tc_boolean		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_string		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_wstring		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_char		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_wchar		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_octet		(DeltaValue* output, CORBA::Any* input);

	static void from_tc_sequence	(DeltaValue* output, CORBA::Any* input);
	static void from_tc_array		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_struct		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_union		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_enum		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_alias		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_except		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_any			(DeltaValue* output, CORBA::Any* input);
	static void from_tc_objref		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_null		(DeltaValue* output, CORBA::Any* input);
	static void from_tc_TypeCode	(DeltaValue* output, CORBA::Any* input);
public:
	static void convert (DeltaValue* output, CORBA::Object_ptr obj);
	static void convert	(DeltaValue* output, CORBA::Any* input);
	static void convert	(DeltaValue* output, CORBA::Any* input, CORBA::DefinitionKind dk);

	static void convert	(DeltaValue* output, const CORBA::AttributeDescription& attribute);
	static void convert	(DeltaValue* output, const CORBA::ConstantDescription& constant);
	static void convert	(DeltaValue* output, const CORBA::ExceptionDescription& exception);
	static void convert	(DeltaValue* output, const CORBA::InterfaceDef& interface);
	static void convert	(DeltaValue* output, const CORBA::ModuleDef& module);
	static void convert	(DeltaValue* output, const CORBA::OperationDescription& operation);

	static void convert	(DeltaValue* output, const CORBA::TypeDescription& operation);

	static void convert	(DeltaValue* output, const CORBA::PrimitiveDef& operation);
	static void convert	(DeltaValue* output, const CORBA::StringDef& operation);
	static void convert	(DeltaValue* output, const CORBA::WstringDef& operation);
	
	static void convert	(DeltaValue* output, const CORBA::ParameterDescription& param);
	static void convert	(DeltaValue* output, const CORBA::OperationMode& mode);
	static void convert	(DeltaValue* output, const CORBA::AttributeMode& mode);
	static void convert	(DeltaValue* output, const CORBA::ParameterMode& mode);
	static void convert	(DeltaValue* output, const CORBA::TypeCode_ptr& type);

	static void SetClientObjectConversion (ObjectConversionFunc f);
	static void SetServerObjectConversion (ObjectConversionFunc f);

	static void Initialize	(void);
	static void Cleanup		(void);
};

#endif
