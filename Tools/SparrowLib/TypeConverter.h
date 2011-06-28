/**
 *	TypeConverter.h
 *
 *	-- Sparrow library function for Delta --
 *
 *	Convert of type from C++ to Delta
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#ifndef SPARROW_LIB_TYPE_CONVERTER_H_
#define SPARROW_LIB_TYPE_CONVERTER_H_

#include "Common.h"
#include "Encoding.h"

#include "ConversionTemplates.h"
#include "DeltaByteCodeTypes.h"

#include <boost/tuple/tuple.hpp>

class DeltaValue;
class DeltaTable;
class DeltaVirtualMachine;

namespace ide
{
	//----------------------------
	//-- class TypeConverter

	class TypeConverter
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		TypeConverter(void) {}
		~TypeConverter(void) {}

		///--- public API
		///--- public API
		static void Initialize(void);
		static void CleanUp(void);

		static bool EncodeDeltaValue(comm::encoder& encoder, DeltaValue* obj, const std::string& type);
		static bool DecodeDeltaValue(comm::decoder& decoder, DeltaValue* obj, const std::string& type);

		template <class TupleType>
		static void DeltaToCpp(TupleType& tuple, const DeltaArgumentVec& args);
		template <class TupleType>
		static void CppToDelta(DeltaArgumentVec& args, const TupleType& tuple);

	private:
		///--- private API

		template<typename T> struct Converter {
			static void encode(comm::encoder& encoder, DeltaValue* obj);
			static void decode(comm::decoder& decoder, DeltaValue* obj);
		};

		///--- private members
		typedef void (*EncoderFunc)(comm::encoder&, DeltaValue*);
		typedef std::map<std::string, EncoderFunc> EncodingMap;
		static EncodingMap numberEncoding, stringEncoding, tableEncoding, externIdEncoding;
		
		typedef void (*DecoderFunc)(comm::decoder&, DeltaValue*);
		typedef std::map<std::string, DecoderFunc> DecodingMap;
		static DecodingMap decoding;
	};


	//-------------------------------------------------------//
	//---- class TypeConverter template functions -----------//

	template<typename T>
	void TypeConverter::Converter<T>::encode(comm::encoder& encoder, DeltaValue* obj)
	{
		T output;
		to_native<T>().convert(output, obj);
		encoder << output;
	}

	template<typename T>
	void TypeConverter::Converter<T>::decode(comm::decoder& decoder, DeltaValue* obj)
	{
		T input;
		decoder >> input;
		to_delta<T>().convert(obj, input);
	}

	//-----------------------------------------------------------------------

	template <class TupleType>
	void TypeConverter::DeltaToCpp(TupleType& tuple, const DeltaArgumentVec& args)
	{
		delta_tuple_converter_t<TupleType, boost::tuples::length<TupleType>::value>()(tuple, args, -1);
	}

	//-----------------------------------------------------------------------

	template <class TupleType>
	void TypeConverter::CppToDelta(DeltaArgumentVec& args, const TupleType& tuple)
	{
		tuple_delta_converter_t<TupleType, boost::tuples::length<TupleType>::value>()(args, tuple);
	}

	//-----------------------------------------------------------------------
}

#endif	//SPARROW_LIB_TYPE_CONVERTER
