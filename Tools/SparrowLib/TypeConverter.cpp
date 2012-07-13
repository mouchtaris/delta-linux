/**
 *	TypeConverter.cpp
 *
 *	-- Sparrow library function for Delta --
 *
 *	Convert of type from C++ to Delta
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "TypeConverter.h"

#include "DeltaByteCodeTypes.h"
#include "DeltaObject.h"

#include "BinaryBuffer.h"
#include "DeltaVMFuncReturnTypes.h"
#include "ProjectManagerFuncReturnTypes.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class TypeConverter ------------------------------//

	TypeConverter::EncodingMap TypeConverter::numberEncoding;
	TypeConverter::EncodingMap TypeConverter::stringEncoding;
	TypeConverter::EncodingMap TypeConverter::tableEncoding;
	TypeConverter::EncodingMap TypeConverter::externIdEncoding;
	TypeConverter::DecodingMap TypeConverter::decoding;

	void TypeConverter::Initialize(void)
	{
#define INITIALIZE_ENCODER_TYPE(map, type)	map[#type] = Converter<type>::encode
#define INITIALIZE_DECODER_TYPE(type)	decoding[#type] = Converter<type>::decode

		numberEncoding["size_t"] = Converter<uint>::encode;
		INITIALIZE_ENCODER_TYPE(numberEncoding, uint);
		INITIALIZE_ENCODER_TYPE(numberEncoding, ulong);
		INITIALIZE_ENCODER_TYPE(numberEncoding, float);
		INITIALIZE_ENCODER_TYPE(numberEncoding, double);

		INITIALIZE_ENCODER_TYPE(stringEncoding, std::string);
		INITIALIZE_ENCODER_TYPE(stringEncoding, String);

		INITIALIZE_ENCODER_TYPE(tableEncoding, Handle);
		INITIALIZE_ENCODER_TYPE(tableEncoding, BinaryBuffer);
		INITIALIZE_ENCODER_TYPE(tableEncoding, InputBinaryBuffer);
		INITIALIZE_ENCODER_TYPE(tableEncoding, SerializedEditorAST);
		INITIALIZE_ENCODER_TYPE(tableEncoding, UserCommandDesc);
		INITIALIZE_ENCODER_TYPE(tableEncoding, StringBoolPair);
		INITIALIZE_ENCODER_TYPE(tableEncoding, ObjectGraphData);
		INITIALIZE_ENCODER_TYPE(tableEncoding, ScriptDependencies);
		INITIALIZE_ENCODER_TYPE(tableEncoding, wxFont);
		INITIALIZE_ENCODER_TYPE(tableEncoding, wxColour);

		INITIALIZE_ENCODER_TYPE(externIdEncoding, StdStringList);
		INITIALIZE_ENCODER_TYPE(externIdEncoding, StringList);
		INITIALIZE_ENCODER_TYPE(externIdEncoding, StringListList);
		INITIALIZE_ENCODER_TYPE(externIdEncoding, IntList);
		INITIALIZE_ENCODER_TYPE(externIdEncoding, UIntList);
		INITIALIZE_ENCODER_TYPE(externIdEncoding, HandleList);

		decoding["size_t"] = Converter<uint>::decode;
		
		INITIALIZE_DECODER_TYPE(uint);
		INITIALIZE_DECODER_TYPE(ulong);
		INITIALIZE_DECODER_TYPE(float);
		INITIALIZE_DECODER_TYPE(double);
		INITIALIZE_DECODER_TYPE(int);
		INITIALIZE_DECODER_TYPE(bool);
		INITIALIZE_DECODER_TYPE(std::string);
		INITIALIZE_DECODER_TYPE(String);
		INITIALIZE_DECODER_TYPE(Handle);
		INITIALIZE_DECODER_TYPE(StdStringList);
		INITIALIZE_DECODER_TYPE(StringList);
		INITIALIZE_DECODER_TYPE(StringListList);
		INITIALIZE_DECODER_TYPE(IntList);
		INITIALIZE_DECODER_TYPE(UIntList);
		INITIALIZE_DECODER_TYPE(HandleList);
		INITIALIZE_DECODER_TYPE(BinaryBuffer);
		INITIALIZE_DECODER_TYPE(InputBinaryBuffer);
		INITIALIZE_DECODER_TYPE(SerializedEditorAST);
		INITIALIZE_DECODER_TYPE(UserCommandDesc);
		INITIALIZE_DECODER_TYPE(StringBoolPair);
		INITIALIZE_DECODER_TYPE(ObjectGraphData);
		INITIALIZE_DECODER_TYPE(ScriptDependencies);
		INITIALIZE_DECODER_TYPE(wxFont);
		INITIALIZE_DECODER_TYPE(wxColour);

#undef INITIALIZE_ENCODER_TYPE
#undef INITIALIZE_CONVERTER_TYPE
	}

	//-----------------------------------------------------------------------

	void TypeConverter::CleanUp(void)
	{
		numberEncoding.clear();
		stringEncoding.clear();
		tableEncoding.clear();
		decoding.clear();
	}

	//-----------------------------------------------------------------------

	bool TypeConverter::EncodeDeltaValue(comm::encoder& encoder, DeltaValue* obj, const std::string& type)
	{
		EncodingMap::const_iterator i;
		switch (obj->Type())
		{
		case DeltaValue_Number:
			i = numberEncoding.find(type);
			if(i != numberEncoding.end())
				(*i->second)(encoder, obj);
			else
				Converter<int>().encode(encoder, obj);
			break;

		case DeltaValue_String:
			i = stringEncoding.find(type);
			if(i != stringEncoding.end())
				(*i->second)(encoder, obj);
			else
				return false;
			break;

		case DeltaValue_Bool:
			Converter<bool>().encode(encoder, obj);
			break;

		case DeltaValue_Table:
			i = tableEncoding.find(type);
			if(i != tableEncoding.end())
				(*i->second)(encoder, obj);
			else
				return false;
			break;

		case DeltaValue_ExternId:
			i = externIdEncoding.find(type);
			if(i != externIdEncoding.end())
				(*i->second)(encoder, obj);
			else
				return false;
			break;

		default:
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool TypeConverter::DecodeDeltaValue(comm::decoder& decoder, DeltaValue* obj, const std::string& type)
	{
		DecodingMap::const_iterator i = decoding.find(type);
		if(i != decoding.end())
			(*i->second)(decoder, obj);

		return obj;
	}

	//-----------------------------------------------------------------------
}
