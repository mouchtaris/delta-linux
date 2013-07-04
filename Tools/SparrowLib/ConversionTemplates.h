/**
*	ConversionTemplates.h
*
*	-- project_name --
*
*	description
*
*	Themistoklis Bourdenas <themis@ics.forth.gr>
*	July 2007
*/
#ifndef DELTA_IDE_LIB_CONVERSION_TEMPLATES_H_
#define DELTA_IDE_LIB_CONVERSION_TEMPLATES_H_

#include "Common.h"
#include "SparrowLibCommon.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"

#include "DeltaByteCodeTypes.h"
#include "DeltaTable.h"
#include "DeltaObject.h"
#include "DeltaTableSerialiser.h"
#include "BufferLib.h"
#include "ListLib.h"

#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentUserCommandDesc.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "ExtSerialization.h"

#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <vector>
#include <exception>
#include <new>

#define DLIB_WX_WINDOW_NATIVE_INSTANCE_TYPE_STR		"wxNativeInstance"

namespace ide
{
	template <class Type>
	void DeltaValueToNative(Type& t, DeltaValue* obj, uint index);
	template <class Type>
	void NativeToDeltaValue(DeltaValue* obj, const Type& t);

	typedef std::vector<DeltaValue*> DeltaArgumentVec;

	static void ClearDeltaArgumentVec(DeltaArgumentVec& vec) {
		BOOST_FOREACH(DeltaValue *o, vec)
			delete o;
		vec.clear();
	}

	//----------------------------
	//-- struct delta_tuple_converter_t

	template <class TupleType, int N>
	struct delta_tuple_converter_t {					
		void operator ()(TupleType& t, const DeltaArgumentVec& args, int explicitIndex = -1) const {
			enum { index = boost::tuples::length<TupleType>::value - N };
			DeltaValueToNative(boost::get<index>(t), args[index], explicitIndex == -1 ? index : explicitIndex);
			delta_tuple_converter_t<TupleType, N-1>()(t, args);
		}
	};
	template <class TupleType>
	struct delta_tuple_converter_t<TupleType, 0> {
		void operator ()(TupleType& t, const DeltaArgumentVec& args) const {}
	};

	//-----------------------------------------------------------------------


	//----------------------------
	//-- struct tuple_delta_converter_t

	template <class TupleType, int N>
	struct tuple_delta_converter_t {					
		void operator ()(DeltaArgumentVec& args, const TupleType& t) const {
			enum { index = boost::tuples::length<TupleType>::value - N };
			NativeToDeltaValue(args[index], boost::get<index>(t));
			tuple_delta_converter_t<TupleType, N-1>()(args, t);
		}
	};
	template <class TupleType>
	struct tuple_delta_converter_t<TupleType, 0> {
		void operator ()(DeltaArgumentVec& args, const TupleType& t) const {}
	};

	//-----------------------------------------------------------------------


	//----------------------------
	//-- struct string_cast

	template <class OutStringType, class InStringType>
	struct string_cast {
		InStringType convert(const InStringType& input) const { return input; }
	};
	template <>
	struct string_cast<String, std::string> {
		String convert(const std::string& input) const { return util::std2str(input); }
	};
	template <>
	struct string_cast<std::string, String> {
		std::string convert(const String& input) const { return util::str2std(input); }
	};

	//-----------------------------------------------------------------------


	//----------------------------
	//-- struct to_native

	template <typename Type>
	struct to_native {
		void convert(PARAM(Type) output, DeltaValue* input, uint index=0) const {
			throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted (not supported)").c_str());
		}
	};

	template <>
	struct to_native<DeltaValue*> {
		void convert(DeltaValue*& output, DeltaValue* input, uint index=0) const {
			output = input;
		}
	};

#define TO_NATIVE_ARITHMETIC_CONVERTER(TYPE)													\
	template <>																					\
	struct to_native<TYPE> {																	\
		void convert(TYPE& output, DeltaValue* input, uint index=0) const {						\
			if (input->Type() != DeltaValue_Number)												\
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)	\
					+ " can not be converted to numerical type").c_str());						\
			output = (TYPE)input->ToNumber();													\
		}																						\
	}
	TO_NATIVE_ARITHMETIC_CONVERTER(uchar);
	TO_NATIVE_ARITHMETIC_CONVERTER(ushort);
	TO_NATIVE_ARITHMETIC_CONVERTER(uint);
	TO_NATIVE_ARITHMETIC_CONVERTER(ulong);
	TO_NATIVE_ARITHMETIC_CONVERTER(time_t);
	TO_NATIVE_ARITHMETIC_CONVERTER(int);
	TO_NATIVE_ARITHMETIC_CONVERTER(float);
	TO_NATIVE_ARITHMETIC_CONVERTER(double);
#undef TO_NATIVE_ARITHMETIC_CONVERTER

	template <>
	struct to_native<bool> {
		void convert(bool& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Bool)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to bool").c_str());
			output = input->ToBool();
		}
	};
	template <>
	struct to_native<std::string> {
		void convert(std::string& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to std::string").c_str());
			output = input->ToString();
		}
	};
	template <>
	struct to_native<String> {
		void convert(String& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to String").c_str());
			output = util::std2str(input->ToString());
		}
	};
	template <>
	struct to_native<Handle> {
		void convert(Handle& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to Handle").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "Handle")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Handle object (.class != 'Handle')").c_str());

			table->BasicGet("serial", &tdata);
			if (tdata.Type() != DeltaValue_Number)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Handle object (.serial is not a Number)").c_str());
			uint serial = (uint)tdata.ToNumber();

			table->BasicGet("class_id", &tdata);
			if (tdata.Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Handle object (.class_id is not a String)").c_str());
			const std::string& class_id = tdata.ToString();

			output = Handle(class_id, serial);
		}
	};
	template <>
	struct to_native<SerializedEditorAST> {
		void convert(SerializedEditorAST& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to a BinaryBuffer").c_str());
			unsigned size;
			void *data = DeltaTableSerialiser().Serialise(input->ToTable(), &size);
			new (&output) SerializedEditorAST(data, size);
			DeltaTableSerialiser::DestroyBuffer(data);
		}
	};
	template <>
	struct to_native<InputBinaryBuffer> {
		void convert(InputBinaryBuffer& output, DeltaValue* input, uint index=0) const {
			if (ubinaryio::InputBuffer* ib = DeltaInputBuffer_Get(*input))
				new (&output) InputBinaryBuffer(ib->GetBuffer(), ib->GetSize());
			else
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
				+ " can not be converted to an " + std::string(INPUTBUFFER_TYPE_STR)).c_str());
		}
	};
	template <>
	struct to_native<Component*> {
		void convert(Component*& output, DeltaValue* input, uint index=0) const {
			Handle handle;
			to_native<Handle>().convert(handle, input, index);
			if (Component* comp = handle.Resolve())
				output = comp;
			else
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
				+ " is not a valid Component (cannot be resolved)").c_str());
		}
	};
	template <typename Type>
	struct to_native<std::list<Type>> {
		void convert(std::list<Type>& output, DeltaValue* input, uint index=0) const {
			if (std::list<DeltaValue>* nativeList = DeltaList_Get(*input))
				for(std::list<DeltaValue>::iterator i = nativeList->begin(); i != nativeList->end(); ++i) {
					Type object = Type();
					to_native<Type>().convert(object, &*i, index);
					output.push_back(object);
				}
			else
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
				+ " can not be converted to " + std::string(LIST_TYPE_STR)).c_str());
		}
	};
	template <typename Type>
	struct to_native<std::map<const std::string, Type>> {
		void convert(std::map<const std::string, Type>& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to Map").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tindex, tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "Map")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Map object (.class != 'Map')").c_str());

			table->BasicGet("value", &tdata);
			if (tdata.Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Map object (.value is not a Table)").c_str());
			table = tdata.ToTable();
			for(DeltaTable::const_iterator i = table->begin(); i != table->end(); ++i) {
				i->GetIndex(&tindex);
				if (tindex.Type() != DeltaValue_String)
					throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
						+ " is not a valid Map object (contains non-string indices)").c_str());

				i->GetContent(&tdata);
				Type object;
				to_native<Type>().convert(object, &tdata, index);
				output[tindex.ToString()] = object;
			}
		}
	};
	template <typename Type1, typename Type2>
	struct to_native<std::pair<Type1, Type2>> {
		void convert(std::pair<Type1, Type2>& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to Pair").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "Pair")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Pair object (.class != 'Pair')").c_str());

			table->BasicGet("first", &tdata);
			to_native<Type1>().convert(output.first, &tdata, index);
			table->BasicGet("second", &tdata);
			to_native<Type2>().convert(output.second, &tdata, index);
		}
	};
	template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	struct to_native<boost::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>> {
		typedef boost::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> Type;
		void convert(Type& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to Tuple").c_str());

			DeltaTable* table = input->ToTable();
			DeltaArgumentVec values;
			DeltaValue tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "Tuple")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Tuple object (.class != 'Tuple')").c_str());

			for (int i=0; true; ++i) {
				table->BasicGet((DeltaNumberValueType) i, &tdata);
				if (tdata.Type() == DeltaValue_Nil)
					break;	///< no more available items in table
				values.push_back(new DeltaValue(tdata));
			}
			try { delta_tuple_converter_t<Type, boost::tuples::length<Type>::value>()(output, values, index); }
			catch(std::exception&) {
				ClearDeltaArgumentVec(values);
				throw;
			}
			ClearDeltaArgumentVec(values);
		}
	};
	template <>
	struct to_native<UserCommandDesc> {
		void convert(UserCommandDesc& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to UserCommandDesc").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "UserCommandDesc")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.class != 'UserCommandDesc')").c_str());

			table->BasicGet("class_id", &tdata);
			if (tdata.Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.class_id is not a String)").c_str());
			const std::string class_id = tdata.ToString();

			table->BasicGet("function_name", &tdata);
			if (tdata.Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.function_name is not a String)").c_str());
			const std::string function_name = tdata.ToString();

			table->BasicGet("isCheck", &tdata);
			if (tdata.Type() != DeltaValue_Nil && tdata.Type() != DeltaValue_Bool)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.isCheck is not a Bool)").c_str());
			bool isCheck = tdata.Type() != DeltaValue_Nil ? tdata.ToBool() : false;

			table->BasicGet("flags", &tdata);
			if (tdata.Type() != DeltaValue_Number)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.flags is not a Number)").c_str());
			int flags = tdata.ToNumber();

			table->BasicGet("isStatic", &tdata);
			if (tdata.Type() != DeltaValue_Nil && tdata.Type() != DeltaValue_Bool)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.isStatic is not a Bool)").c_str());
			bool isStatic = tdata.Type() != DeltaValue_Nil ? tdata.ToBool() : true;

			table->BasicGet("enabled", &tdata);
			if (tdata.Type() != DeltaValue_Nil && tdata.Type() != DeltaValue_Bool)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.enabled is not a Bool)").c_str());
			bool enabled = tdata.Type() != DeltaValue_Nil ? tdata.ToBool() : true;

			table->BasicGet("image", &tdata);
			if (tdata.Type() != DeltaValue_Nil && tdata.Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid UserCommandDesc object (.image is not a String)").c_str());
			const String image = tdata.Type() != DeltaValue_Nil ? util::std2str(tdata.ToString()) : String();

			output = UserCommandDesc(UserCommandDesc::Callback(class_id, function_name), isCheck, flags, isStatic, enabled, image);
		}
	};
	template <>
	struct to_native<wxFont> {
		void convert(wxFont& output, DeltaValue* input, uint index=0) const {
			typedef std::map<std::string, int> TypeMap;
			static TypeMap families, styles, weights;
			static bool initialized = false;
			if (!initialized) {
				families["DEFAULT"]		= wxFONTFAMILY_DEFAULT;
				families["DECORATIVE"]	= wxFONTFAMILY_DECORATIVE;
				families["ROMAN"]		= wxFONTFAMILY_ROMAN;
				families["SCRIPT"]		= wxFONTFAMILY_SCRIPT;
				families["SWISS"]		= wxFONTFAMILY_SWISS;
				families["MODERN"]		= wxFONTFAMILY_MODERN;
				families["TELETYPE"]	= wxFONTFAMILY_TELETYPE;

				styles["NORMAL"]	= wxFONTSTYLE_NORMAL;
				styles["SLANT"]		= wxFONTSTYLE_SLANT;
				styles["ITALIC"]	= wxFONTSTYLE_ITALIC;

				weights["NORMAL"]	= wxFONTWEIGHT_NORMAL;
				weights["LIGHT"]	= wxFONTWEIGHT_LIGHT;
				weights["BOLD"]		= wxFONTWEIGHT_BOLD;
				initialized = true;
			}

			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to wxFont").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "wxFont")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.class != 'wxFont')").c_str());

			table->BasicGet("size", &tdata);
			if (tdata.Type() != DeltaValue_Number)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.size is not a Number)").c_str());
			else
				output.SetPointSize((int) tdata.ToNumber());

			TypeMap::const_iterator familyIter;
			table->BasicGet("family", &tdata);
			if (tdata.IsNil())
				output.SetFamily(wxFONTFAMILY_DEFAULT);
			else if (tdata.Type() != DeltaValue_String || (familyIter = families.find(tdata.ToString())) == families.end())
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.family is not a String or is not valid)").c_str());
			else
				output.SetFamily(familyIter->second);

			TypeMap::const_iterator styleIter;
			table->BasicGet("style", &tdata);
			if (tdata.IsNil())
				output.SetStyle(wxFONTSTYLE_NORMAL);
			else if (tdata.Type() != DeltaValue_String || (styleIter = styles.find(tdata.ToString())) == styles.end())
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.style is not a String or is not valid)").c_str());
			else
				output.SetStyle(styleIter->second);

			TypeMap::const_iterator weightIter;
			table->BasicGet("weight", &tdata);
			if (tdata.IsNil())
				output.SetWeight(wxFONTWEIGHT_NORMAL);
			else if (tdata.Type() != DeltaValue_String || (weightIter = weights.find(tdata.ToString())) == weights.end())
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.weight is not a String or is not valid)").c_str());
			else
				output.SetWeight(weightIter->second);

			table->BasicGet("underline", &tdata);
			if (tdata.IsNil())
				output.SetUnderlined(false);
			else if (tdata.Type() != DeltaValue_Bool)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.underline is not Bool)").c_str());
			else
				output.SetUnderlined(tdata.ToBool());

			table->BasicGet("facename", &tdata);
			if (tdata.Type() != DeltaValue_Nil && tdata.Type() != DeltaValue_String)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxFont object (.facename is not a String)").c_str());
			else if (!tdata.IsNil())
				output.SetFaceName(util::std2str(tdata.ToString()));
		}
	};
	template <>
	struct to_native<wxColour> {
		unsigned char get(DeltaTable *table, const std::string& index, uint arg) const {
			DeltaValue tdata;
			table->BasicGet(index, &tdata);
			if (tdata.Type() != DeltaValue_Number)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(arg + 1)
				+ std::string(" is not a valid wxColour object (.") + index + " is not a Number)").c_str());
			return tdata.ToNumber();
		}
		void convert(wxColour& output, DeltaValue* input, uint index=0) const {
			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to wxColour").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tdata;
			table->BasicGet("class", &tdata);
			if (tdata.Type() != DeltaValue_String || tdata.ToString() != "wxColour")
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid wxColour object (.class != 'wxColour')").c_str());

			unsigned char r = get(table, "r", index);
			unsigned char g = get(table, "g", index);
			unsigned char b = get(table, "b", index);
			unsigned char a;
			try { a = get(table, "a", index); }
			catch(...) { a = wxALPHA_OPAQUE;}
			output.Set(r, g, b, a);
		}
	};
	template <>
	struct to_native<conf::Property*> {
		typedef conf::Property* (*ConverterFunc)(DeltaTable*, uint);
	
		template<class T> static T GetTableItem(DeltaTable* table, uint index, const std::string& name,
			bool useDefaultValue = true, T defaultValue = T())
		{
			T value;
			DeltaValue tdata;
			table->BasicGet(name, &tdata);
			try { to_native<T>().convert(value, &tdata, index); }
			catch(std::exception& e) {
				if (useDefaultValue)
					value = defaultValue;
				else
					throw e;
			}
			return value;
		}

		template<class T> static conf::Property* ConvertPrimitiveProperty(DeltaTable* table, uint index)
			{ return new T(String(), GetTableItem<T::ValueType>(table, index, "value", false));	}

		static conf::Property* ConvertIntRangeProperty(DeltaTable* table, uint index) {
			int min = GetTableItem<int>(table, index, "min", false);
			int max = GetTableItem<int>(table, index, "max", false);
			int value = GetTableItem<int>(table, index, "value", false);
			return new conf::IntRangeProperty(String(), min, max, value);
		}

		static conf::Property* ConvertEnumStringProperty(DeltaTable* table, uint index) {
			StringList listOptions = GetTableItem<StringList>(table, index, "options", false);
			uint optionIndex = GetTableItem<uint>(table, index, "index", false);
			StringVec options(listOptions.begin(), listOptions.end());
			return new conf::EnumStringProperty(String(), options, optionIndex);
		}

		template<class T>
		static conf::Property* ConvertStringListPropertyTemplate(DeltaTable* table, uint index) {
			const std::string id("value");
			T* p = new T(String());
			DeltaValue tdata;
			table->BasicGet(id, &tdata);
			//Support both an array of strings or a single string that separates entries with ';' or ','
			if (tdata.Type() == DeltaValue_String)	
				p->SetValue(util::std2str(tdata.ToString()));
			else {
				StringList values;
				try { to_native<StringList>().convert(values, &tdata, index); }
				catch(...) { delete p; throw; }
				BOOST_FOREACH(const String& str, values)
					p->AddValue(str);
			}
			return p;
		}

		static conf::Property* ConvertStringListProperty(DeltaTable* table, uint index)
			{ return ConvertStringListPropertyTemplate<conf::StringListProperty>(table, index); }

		static conf::Property* ConvertAggregateProperty(DeltaTable* table, uint index) {
			DeltaValue data;
			table->BasicGet("properties", &data);
			conf::AggregateProperty* p = new conf::AggregateProperty(String());
			try { to_native< std::map<const std::string, conf::Property*> >().convert(p->GetPropertyMap(), &data, index); }
			catch(...) { delete p; throw; }
			return p;
		}

		static conf::Property* ConvertFontProperty(DeltaTable* table, uint index)
			{ return new conf::FontProperty(String(), GetTableItem<wxFont>(table, index, "value", false)); }

		static conf::Property* ConvertColorProperty(DeltaTable* table, uint index)
			{ return new conf::ColorProperty(String(), GetTableItem<wxColour>(table, index, "value", false)); }

		static conf::Property* ConvertDateProperty(DeltaTable* table, uint index) {
			//TODO: Maybe add support for a string encoding of the date as well.
			ushort day = GetTableItem<ushort>(table, index, "day", false);
			if (day > 31)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid DateProperty object (.day is not valid)").c_str());
			byte month = GetTableItem<ushort>(table, index, "month", false) - 1;
			if (month > 12)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid DateProperty object (.month is not valid)").c_str());
			int year = GetTableItem<int>(table, index, "year", false);
			return new conf::DateProperty(String(), wxDateTime(day, (wxDateTime::Month) month, year));
		}

		static conf::Property* ConvertFileListProperty(DeltaTable* table, uint index)
			{ return ConvertStringListPropertyTemplate<conf::FileListProperty>(table, index); }

		static conf::Property* ConvertDirectoryListProperty(DeltaTable* table, uint index)
			{ return ConvertStringListPropertyTemplate<conf::DirectoryListProperty>(table, index); }

		static conf::Property* ConvertAggregateListProperty(DeltaTable* table, uint index) {
			using namespace conf;
			DeltaValue tindex, tdata;
			table->BasicGet("type", &tdata);
			Property* type;
			to_native<Property*>().convert(type, &tdata, index);
			if (type->GetType() != AggregateProperty::Type) {
				delete type;
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid AggregateProperty object (.type is not valid)").c_str());
			}
			table->BasicGet("value", &tdata);
			AggregateListProperty* p = new AggregateListProperty(String(), safe_prop_cast<AggregateProperty>(type));
			AggregateListProperty::AggregatePropertyList& l = p->GetPropertyList();
			try { to_native<AggregateListProperty::AggregatePropertyList>().convert(l, &tdata, index); }
			catch(...) { delete p; throw; }
			return p;
		}

		static conf::Property* ConvertMultiChoiceProperty(DeltaTable* table, uint index) {
			DeltaValue data;
			table->BasicGet("value", &data);
			conf::MultiChoiceProperty* p = new conf::MultiChoiceProperty(String());
			conf::MultiChoiceProperty::ChoiceMap& choices = p->GetChoiceMap();
			try { to_native<conf::MultiChoiceProperty::ChoiceMap>().convert(choices, &data, index); }
			catch(...) { delete p; throw; }
			return p;
		}

		void convert(conf::Property*& output, DeltaValue* input, uint index=0) const {
			using namespace conf;
			typedef std::map<std::string, ConverterFunc> ConvertionMap;
			static ConvertionMap conversions;
			if (conversions.empty()) {
#define ADD_PROPERTY_CONVERTION(type) conversions[STRINGIFY(type)] = Convert##type
#define ADD_PRIMITIVE_PROPERTY(type) conversions[STRINGIFY(type)] = ConvertPrimitiveProperty<type>
				ADD_PRIMITIVE_PROPERTY(IntProperty);
				ADD_PRIMITIVE_PROPERTY(BoolProperty);
				ADD_PRIMITIVE_PROPERTY(RealProperty);
				ADD_PRIMITIVE_PROPERTY(StringProperty);
				ADD_PRIMITIVE_PROPERTY(StdStringProperty);
				ADD_PROPERTY_CONVERTION(IntRangeProperty);
				ADD_PROPERTY_CONVERTION(EnumStringProperty);
				ADD_PROPERTY_CONVERTION(StringListProperty);
				ADD_PROPERTY_CONVERTION(AggregateProperty);
				ADD_PROPERTY_CONVERTION(FontProperty);
				ADD_PROPERTY_CONVERTION(ColorProperty);
				ADD_PRIMITIVE_PROPERTY(FileProperty);
				ADD_PRIMITIVE_PROPERTY(DirectoryProperty);
				ADD_PROPERTY_CONVERTION(DateProperty);
				ADD_PROPERTY_CONVERTION(FileListProperty);
				ADD_PROPERTY_CONVERTION(DirectoryListProperty);
				ADD_PROPERTY_CONVERTION(AggregateListProperty);
				ADD_PROPERTY_CONVERTION(MultiChoiceProperty);
#undef ADD_PROPERTY_CONVERTION
#undef ADD_PRIMITIVE_PROPERTY
			}

			if (input->Type() != DeltaValue_Table)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " can not be converted to Property").c_str());

			DeltaTable* table = input->ToTable();
			DeltaValue tdata;
			table->BasicGet("class", &tdata);

			ConvertionMap::const_iterator i;
			if (tdata.Type() != DeltaValue_String ||
				(i = conversions.find(tdata.ToString())) == conversions.end()
			)
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
					+ " is not a valid Property object (.class is not valid)").c_str());

			output = (*i->second)(table, index);
			output->SetLabel(GetTableItem<String>(table, index, "label", false));
			output->SetVisible(GetTableItem<bool>(table, index, "visible", true, true));
			output->SetEnabled(GetTableItem<bool>(table, index, "enabled", true, true));
			output->SetDescription(GetTableItem<String>(table, index, "description"));
			output->SetCategory(GetTableItem<String>(table, index, "category"));
		}
	};
	template <>
	struct to_native<wxWindow*> {
		void convert(wxWindow*& output, DeltaValue* input, uint index=0) const {
			DeltaValueType type = input->Type();
			if (type == DeltaValue_Nil)
				output = (wxWindow*) 0;
			else if (type == DeltaValue_ExternId) {
				std::string typeStr;
				void* val = input->ToExternId(typeStr);
				if (typeStr != DLIB_WX_WINDOW_NATIVE_INSTANCE_TYPE_STR)
					throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
						+ " can not be converted to wxWindow (externid type is not valid)").c_str());
				output = (wxWindow*) val;
			}
			else
				throw std::exception(("argument " + boost::lexical_cast<std::string>(index + 1)
						+ " can not be converted to wxWindow").c_str());
		}
	};

	//----------------------------
	//-- struct to_delta

	template <typename Type>
	struct to_delta {
		void convert(DeltaValue* output, PARAM(Type) input) const {}
	};

#define TO_DELTA_ARITHMETIC_CONVERTER(TYPE)						\
	template <>													\
	struct to_delta<TYPE> {										\
		void convert(DeltaValue* output, TYPE input) const {	\
			output->FromNumber((DeltaNumberValueType)input);	\
		}														\
	}
	TO_DELTA_ARITHMETIC_CONVERTER(uchar);
	TO_DELTA_ARITHMETIC_CONVERTER(ushort);
	TO_DELTA_ARITHMETIC_CONVERTER(uint);
	TO_DELTA_ARITHMETIC_CONVERTER(ulong);
	TO_DELTA_ARITHMETIC_CONVERTER(time_t);
	TO_DELTA_ARITHMETIC_CONVERTER(int);
	TO_DELTA_ARITHMETIC_CONVERTER(float);
	TO_DELTA_ARITHMETIC_CONVERTER(double);
#undef TO_DELTA_ARITHMETIC_CONVERTER

	template <>
	struct to_delta<bool> {
		void convert(DeltaValue* output, bool input) const {
			output->FromBool(input);
		}
	};
	template <>
	struct to_delta<std::string> {
		void convert(DeltaValue* output, const std::string& input) const {
			output->FromString(input);
		}
	};
	template <>
	struct to_delta<String> {
		void convert(DeltaValue* output, const String& input) const {
			output->FromString(util::str2std(input));
		}
	};
	template <>
	struct to_delta<Handle> {
		void convert(DeltaValue* output, const Handle& input) const {
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "Handle");
			table->Set("class_id", input.GetClassId());
			table->Set("serial", (DeltaNumberValueType) input.GetSerial());
		}
	};
	template <>
	struct to_delta<SerializedEditorAST> {
		void convert(DeltaValue* output, const SerializedEditorAST& input) const {
			if (DeltaTable* table = DeltaTableSerialiser().Deserialise(input.GetData(), input.GetSize())) {
				output->FromTable(table);
				DeltaObject::NativeCodeHelpers::Unrefer(table);
			}
			else {
				DASSERT(UERROR_ISRAISED());
				Call<void (const String&), SafeCall>("SparrowLib", "Output", "Append")
					(util::std2str(uerror::GetSingleton().getreport() + "\n"));
				uerror::GetSingleton().clear();
				output->FromNil();
			}
		}
	};
	template <>
	struct to_delta<InputBinaryBuffer> {
		void convert(DeltaValue* output, const InputBinaryBuffer& input) const {
			if (input.GetSize()) {
				DASSERT(input.GetData());
				ubinaryio::InputBuffer ib(input.GetData(), input.GetSize(), false);
				DeltaInputBuffer_Make(output, &ib);
			}
			else
				{ DASSERT(!input.GetData()); output->FromNil(); }
		}
	};
	template <>
	struct to_delta<Component*> {	///< normally we dont pass pointers to delta, but it happens in internal component notifications
		void convert(DeltaValue* output, const Component* input) const {
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "Handle");
			table->Set("class_id", input->GetClassId());
			table->Set("serial", (DeltaNumberValueType) input->GetSerial());
		}
	};
	template <typename Type>
	struct to_delta<std::list<Type>> {
		void convert(DeltaValue* output, const std::list<Type>& input) const {
			DeltaList_Make(*output);
			std::list<DeltaValue>* nativeList = DeltaList_Get(*output);
			void* listCont = DeltaList_GetCollectableContainer(*output);
			for (std::list<Type>::const_iterator i = input.begin(); i != input.end(); ++i) {
				DeltaValue data;
				to_delta<Type>().convert(&data, *i);
				nativeList->push_back(data);
				nativeList->back().SetResidencyType(DeltaValue::Contained, listCont);
			}
		}
	};
	template <typename Type>
	struct to_delta<const std::map<const std::string, Type>> {
		typedef const std::map<const std::string, Type> Map;
		void convert(DeltaValue* output, Map& input) const {
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "Map");

			DeltaTable* value = DeltaObject::NewObject();
			table->Set("value", value);
			BOOST_FOREACH(const Map::value_type& pair, input) {
				DeltaValue index, data;
				index.FromString(pair.first);
				to_delta<Type>().convert(&data, pair.second);
				value->Set(index, data);
			}
		}
	};
	template <typename Type1, typename Type2>
	struct to_delta<std::pair<Type1, Type2>> {
		void convert(DeltaValue* output, const std::pair<Type1, Type2>& input) const {
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "Pair");

			DeltaValue data;
			to_delta<Type1>().convert(&data, input.first);
			table->Set("first", data);
			to_delta<Type2>().convert(&data, input.second);
			table->Set("second", data);
		}
	};
	template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	struct to_delta<boost::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>> {
		typedef boost::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> Type;
		void convert(DeltaValue* output, const Type& input) const {
			const unsigned N = boost::tuples::length<Type>::value;
			DeltaArgumentVec values;
			for(unsigned i = 0; i < N; ++i)
				values.push_back(new DeltaValue());
			tuple_delta_converter_t<Type, N>()(values, input);
			
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "Tuple");

			int index=0;
			BOOST_FOREACH(const DeltaValue* o, values)
				table->Set((DeltaNumberValueType) index++, *o);
			ClearDeltaArgumentVec(values);
			
		}
	};
	template <>
	struct to_delta<UserCommandDesc> {
		void convert(DeltaValue* output, const UserCommandDesc& input) const {
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "UserCommandDesc");
			table->Set("class_id", input.GetCallback().first);
			table->Set("function_name", input.GetCallback().second);
			table->Set("check", input.IsCheck());
			table->Set("flags", (DeltaNumberValueType) input.GetFlags());
			table->Set("enabled", input.IsEnabled());
			table->Set("image", util::str2std(input.GetImage()));
		}
	};
	template <>
	struct to_delta<wxFont> {
		void convert(DeltaValue* output, const wxFont& input) const {
			static std::map<int, std::string> families;
			static std::map<int, std::string> styles;
			static std::map<int, std::string> weights;
			static bool initialized = false;
			if (!initialized) {
				families[wxFONTFAMILY_DEFAULT]		= "DEFAULT";
				families[wxFONTFAMILY_DECORATIVE]	= "DECORATIVE";
				families[wxFONTFAMILY_ROMAN]		= "ROMAN";
				families[wxFONTFAMILY_SCRIPT]		= "SCRIPT";
				families[wxFONTFAMILY_SWISS]		= "SWISS";
				families[wxFONTFAMILY_MODERN]		= "MODERN";
				families[wxFONTFAMILY_TELETYPE]		= "TELETYPE";

				styles[wxFONTSTYLE_NORMAL]	= "NORMAL";
				styles[wxFONTSTYLE_SLANT]	= "SLANT";
				styles[wxFONTSTYLE_ITALIC]	= "ITALIC";

				weights[wxFONTWEIGHT_NORMAL]	= "NORMAL";
				weights[wxFONTWEIGHT_LIGHT]		= "LIGHT";
				weights[wxFONTWEIGHT_BOLD]		= "BOLD";
				initialized = true;
			}
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "wxFont");
			table->Set("size", (DeltaNumberValueType) input.GetPointSize());
			table->Set("family", families[input.GetFamily()]);
			table->Set("style", styles[input.GetStyle()]);
			table->Set("weight", weights[input.GetWeight()]);
			table->Set("underline", input.GetUnderlined());
			table->Set("facename", util::str2std(input.GetFaceName()));
		}
	};
	template <>
	struct to_delta<wxColour> {
		void convert(DeltaValue* output, const wxColour& input) const {
			DeltaTable* table = DeltaObject::NewObject();
			output->FromTable(table);
			table->Set("class", "wxColour");
			table->Set("r", (DeltaNumberValueType) input.Red());
			table->Set("g", (DeltaNumberValueType) input.Green());
			table->Set("b", (DeltaNumberValueType) input.Blue());
			table->Set("a", (DeltaNumberValueType) input.Alpha());
		}
	};
	template <>
	struct to_delta<const conf::Property*> {
		typedef void (*ConverterFunc)(DeltaTable*, const conf::Property*);

		template<class T> static void ConvertPrimitiveProperty(DeltaTable* table, const conf::Property* prop) {
			DeltaValue data;
			to_delta<typename T::ValueType>().convert(&data, conf::get_prop_value<T>(prop));
			table->Set("value", data);
		}

		static void ConvertIntRangeProperty(DeltaTable* table, const conf::Property* prop) {
			const conf::IntRangeProperty* p = conf::safe_prop_cast<const conf::IntRangeProperty>(prop);
			table->Set("min", (DeltaNumberValueType) p->GetMin());
			table->Set("max", (DeltaNumberValueType) p->GetMax());
			table->Set("value", (DeltaNumberValueType) p->GetValue());
		}

		static void ConvertEnumStringProperty(DeltaTable* table, const conf::Property* prop) {
			const conf::EnumStringProperty* p = conf::safe_prop_cast<const conf::EnumStringProperty>(prop);
			const StringVec& options = p->GetOptions();
			DeltaValue data;
			to_delta<StringList>().convert(&data, StringList(options.begin(), options.end()));
			table->Set("options", data);
			table->Set("index", (DeltaNumberValueType) p->GetOption());
			table->Set("value", util::str2std(p->GetValue()));
		}

		template<class T>
		static void ConvertStringListPropertyTemplate(DeltaTable* table, const conf::Property* prop) {
			const T* p = conf::safe_prop_cast<const T>(prop);
			const StringVec& values = p->GetValues();
			DeltaValue data;
			to_delta<StringList>().convert(&data, StringList(values.begin(), values.end()));
			table->Set("value", data);
		}

		static void ConvertStringListProperty(DeltaTable* table, const conf::Property* prop)
			{ return ConvertStringListPropertyTemplate<conf::StringListProperty>(table, prop); }

		static void ConvertAggregateProperty(DeltaTable* table, const conf::Property* prop) {
			const conf::AggregateProperty* p = conf::safe_prop_cast<const conf::AggregateProperty>(prop);
			DeltaValue data;
			to_delta< const std::map<const std::string, conf::Property*> >().convert(&data, p->GetPropertyMap());
			table->Set("properties", data);
		}

		static void ConvertFontProperty(DeltaTable* table, const conf::Property* prop) {
			DeltaValue data;
			to_delta<wxFont>().convert(&data, conf::get_prop_value<conf::FontProperty>(prop));
			table->Set("value", data);
		}

		static void ConvertColorProperty(DeltaTable* table, const conf::Property* prop) {
			DeltaValue data;
			to_delta<wxColour>().convert(&data, conf::get_prop_value<conf::ColorProperty>(prop));
			table->Set("value", data);
		}

		static void ConvertDateProperty(DeltaTable* table, const conf::Property* prop) {
			wxDateTime date = conf::get_prop_value<conf::DateProperty>(prop);
			table->Set("day", (DeltaNumberValueType) date.GetDay());
			table->Set("month", (DeltaNumberValueType) date.GetMonth() + 1);
			table->Set("year", (DeltaNumberValueType) date.GetYear());
		}

		static void ConvertFileListProperty(DeltaTable* table, const conf::Property* prop)
			{ return ConvertStringListPropertyTemplate<conf::FileListProperty>(table, prop); }

		static void ConvertDirectoryListProperty(DeltaTable* table, const conf::Property* prop)
			{ return ConvertStringListPropertyTemplate<conf::DirectoryListProperty>(table, prop); }

		static void ConvertAggregateListProperty(DeltaTable* table, const conf::Property* prop) {
			const conf::AggregateListProperty* p = conf::safe_prop_cast<const conf::AggregateListProperty>(prop);
			DeltaValue data;
			to_delta<const conf::Property*>().convert(&data, p->GetListType());
			table->Set("type", data);
			to_delta<conf::AggregateListProperty::AggregatePropertyList>().convert(&data, p->GetPropertyList());
			table->Set("value", data);
		}

		static void ConvertMultiChoiceProperty(DeltaTable* table, const conf::Property* prop) {
			const conf::MultiChoiceProperty* p = conf::safe_prop_cast<const conf::MultiChoiceProperty>(prop);
			DeltaValue data;
			to_delta<conf::MultiChoiceProperty::ChoiceMap>().convert(&data, p->GetChoiceMap());
			table->Set("value", data);
		}

		void convert(DeltaValue* output, const conf::Property* input) const {
			using namespace conf;
			typedef std::pair<std::string, ConverterFunc> NameFuncPair;
			typedef std::map<int, NameFuncPair> ConvertionMap;
			static ConvertionMap conversions;
			if (conversions.empty()) {
#define ADD_PROPERTY_CONVERTION(type) conversions[type::Type] = NameFuncPair(STRINGIFY(type), Convert##type)
#define ADD_PRIMITIVE_PROPERTY(type) conversions[type::Type] = NameFuncPair(STRINGIFY(type), ConvertPrimitiveProperty<type>)
				ADD_PRIMITIVE_PROPERTY(IntProperty);
				ADD_PRIMITIVE_PROPERTY(BoolProperty);
				ADD_PRIMITIVE_PROPERTY(RealProperty);
				ADD_PRIMITIVE_PROPERTY(StringProperty);
				ADD_PRIMITIVE_PROPERTY(StdStringProperty);
				ADD_PROPERTY_CONVERTION(IntRangeProperty);
				ADD_PROPERTY_CONVERTION(EnumStringProperty);
				ADD_PROPERTY_CONVERTION(StringListProperty);
				ADD_PROPERTY_CONVERTION(AggregateProperty);
				ADD_PROPERTY_CONVERTION(FontProperty);
				ADD_PROPERTY_CONVERTION(ColorProperty);
				ADD_PRIMITIVE_PROPERTY(FileProperty);
				ADD_PRIMITIVE_PROPERTY(DirectoryProperty);
				ADD_PROPERTY_CONVERTION(DateProperty);
				ADD_PROPERTY_CONVERTION(FileListProperty);
				ADD_PROPERTY_CONVERTION(DirectoryListProperty);
				ADD_PROPERTY_CONVERTION(AggregateListProperty);
				ADD_PROPERTY_CONVERTION(MultiChoiceProperty);
#undef ADD_PROPERTY_CONVERTION
#undef ADD_PRIMITIVE_PROPERTY
			}
			if (input) {
				DeltaTable* table = DeltaObject::NewObject();
				output->FromTable(table);
				ConvertionMap::const_iterator i = conversions.find(input->GetType());
				DASSERT(i != conversions.end());
				table->Set("class", i->second.first);
				table->Set("label", util::str2std(input->GetDescription()));
				table->Set("visible", input->IsVisible());
				table->Set("enabled", input->IsEnabled());
				if (!input->GetDescription().empty())
					table->Set("description", util::str2std(input->GetDescription()));
				if (!input->GetCategory().empty())
					table->Set("category", util::str2std(input->GetCategory()));
				(*i->second.second)(table, input);
			}
			else
				output->FromNil();
		}
	};
	template <>
	struct to_delta<conf::Property*> {
		void convert(DeltaValue* output, conf::Property* input) const {
			to_delta<const conf::Property*>().convert(output, const_cast<const conf::Property*>(input));
		}
	};
	template <>
	struct to_delta<wxWindow*> {
		void convert(DeltaValue* output, wxWindow* input) const {
			output->FromExternId(
				(void*)input,
				DeltaExternId_NonCollectable,
				(void(*)(DeltaString*, void*))0,
				DLIB_WX_WINDOW_NATIVE_INSTANCE_TYPE_STR
			);
		}
	};

	//-----------------------------------------------------------------------

	template <class Type>
	void DeltaValueToNative(Type& output, DeltaValue* input, uint index)
	{
		to_native<Type>().convert(output, input, index);
	}

	//-----------------------------------------------------------------------

	template <class Type>
	void NativeToDeltaValue(DeltaValue* output, const Type& input)
	{
		to_delta<Type>().convert(output, input);
	}

	//-----------------------------------------------------------------------
}

#endif	//DELTA_IDE_LIB_CONVERSION_TEMPLATES
