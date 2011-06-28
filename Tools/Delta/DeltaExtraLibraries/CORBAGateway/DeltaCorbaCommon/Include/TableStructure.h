#ifndef TABLE_STRUCTURE_H
#define TABLE_STRUCTURE_H

#include "Corba.h"
#include "DeltaByteCodeTypes.h"
#include <map>

class DeltaValue;
class DeltaTable;

class TableStructure {
private:
	struct Index {
		union { std::string* stringVal; DeltaNumberValueType numberVal; } value;
		bool isString;

		bool operator < (const Index& index) const {
			return	index.isString && !isString || //numerics before strings
					index.isString && isString && *value.stringVal < *index.value.stringVal ||
					!index.isString && !isString && value.numberVal < index.value.numberVal ;
		}
		bool operator == (const Index& index) const {
			return	isString && index.isString && *value.stringVal == *index.value.stringVal	||
					!isString && !index.isString && value.numberVal == index.value.numberVal	;
		}
		bool operator != (const Index& index) const { return !operator==(index); }

		Index(DeltaNumberValueType number = 0) : isString(false) { value.numberVal = number; }
		Index(const std::string& str) : isString(true) { value.stringVal = new std::string(str); }
		Index(const Index& index) : isString(index.isString), value(index.value) {}
		~Index() {
			if (isString && value.stringVal) {
				delete value.stringVal;
				value.stringVal = 0;
			}
		}
	};

	enum ValueType { ValueTypeNumber, ValueTypeString, ValueTypeBool, ValueTypeTable, ValueTypeExtern, ValueTypeAny };
	struct Value {
		union { std::string* externVal; TableStructure* tableVal; } value;
		ValueType type;

		bool operator == (const Value& value) const {
			return	value.type == ValueTypeAny || type == ValueTypeAny ||
					value.type == type && (type == ValueTypeNumber || type == ValueTypeString || type == ValueTypeBool ||
					type == ValueTypeTable && *value.value.tableVal == *this->value.tableVal ||
					type == ValueTypeExtern && *value.value.externVal == *this->value.externVal);
		}
		bool operator != (const Value& value) const { return !operator==(value); }

		Value(ValueType type = ValueTypeAny) : type(type) { value.externVal = 0, value.tableVal = 0; }
		Value(const std::string& str) : type(ValueTypeExtern) { value.externVal = new std::string(str); }
		Value(TableStructure* table) : type(ValueTypeTable) { value.tableVal = table; }
		~Value() {
			if (type == ValueTypeExtern && value.externVal) {
				delete value.externVal;
				value.externVal = 0;
			}
			else if (type == ValueTypeTable && value.tableVal) {
				delete value.tableVal;
				value.tableVal = 0;
			}
		}
	};

	typedef std::map<Index, Value> Structure;
	Structure structure;
	bool isArray;
	Value arrayType;
	unsigned arraySize;

	typedef std::map<CORBA::TypeCode_ptr, TableStructure*> TypecodeMappings;
	static TypecodeMappings mappings;

	bool CheckArray(void);
	bool CheckStruct(void);

	static CORBA::TypeCode_ptr ExtractTypeCode (const Value& value);
	static CORBA::TypeCode_ptr ExtractTypeCode (const TableStructure& tableStructure, const std::string& tag);

public:
	bool FromTable(DeltaTable* table);
	bool FromTypecode(CORBA::TypeCode_ptr tc);

	bool IsArray	(void) const { return isArray;	}
	bool IsStruct	(void) const { return !isArray;	}

	bool operator ==(const TableStructure& table) const;


	static bool Initialize	(void);
	static void Cleanup		(void);

	static CORBA::TypeCode_ptr ExtractTypeCode (DeltaValue* object);

	TableStructure(void) : isArray(false), arraySize(0) {}
	~TableStructure() { structure.clear(); }
};

#endif