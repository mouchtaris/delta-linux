/**
 *	ComponentDataTypeConverter.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentDataTypeConverter.h"
#include "PropertiesFactory.h"
#include "ComponentRegistry.h"
#include "Encoding.h"

#include "Algorithms.h"
#include "Adaptors.h"
#include "Component.h"
#include "ComponentHandle.h"

#include <boost/lexical_cast.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentDataTypeConverter class
//
void ComponentDataTypeConverter::Initialize (void)
{
	InstallConverter("Handle", &convertHandle);
}

//**********************************************************************

void ComponentDataTypeConverter::CleanUp (void)
{

}

//**********************************************************************

void ComponentDataTypeConverter::InstallConverter (const std::string& type, ConversionFunc func)
{
	s_conversions[type] = func;
}

//**********************************************************************

conf::Property* ComponentDataTypeConverter::TypeToPropertyPlaceholder (const std::string& type)
{
	ConversionMap::const_iterator iter = s_conversions.find(type);
	if (iter != s_conversions.end())
		return iter->second(type, OP_PLACEHOLDER, 0, 0, 0);
	else
		return convertPrimitive(type, OP_PLACEHOLDER, 0, 0, 0);
}

//**********************************************************************

void ComponentDataTypeConverter::EncodeType (
		comm::encoder&		enc,
		const std::string&	type,
		conf::Property*		prop
	)
{
	ConversionMap::const_iterator iter = s_conversions.find(type);
	if (iter != s_conversions.end())
		iter->second(type, OP_ENCODE, prop, &enc, 0);
	else
		convertPrimitive(type, OP_ENCODE, prop, &enc, 0);
}

//**********************************************************************

conf::Property* ComponentDataTypeConverter::DecodeType (
		comm::decoder&		dec,
		const std::string&	type
	)
{
	ConversionMap::const_iterator iter = s_conversions.find(type);
	if (iter != s_conversions.end())
		return iter->second(type, OP_DECODE, 0, 0, &dec);
	else
		convertPrimitive(type, OP_DECODE, 0, 0, &dec);
	return 0;
}

//**********************************************************************

conf::Property* ComponentDataTypeConverter::convertPrimitive (
		const std::string&	typeId,
		OperationType		requestedOperation,
		conf::Property*		prop,
		comm::encoder*		enc,
		comm::decoder*		dec
	)
{
	switch (requestedOperation) {
		case OP_PLACEHOLDER:
			return conf::PropertiesFactory::Create(typeId);
		case OP_ENCODE:
			assert(prop);
			prop->Encode(*enc);
			return 0;
		case OP_DECODE:
			assert(prop);
			prop->Decode(*dec);
			return 0;
	}
	return 0;
}

//**********************************************************************

conf::Property* ComponentDataTypeConverter::convertHandle (
		const std::string&	typeId,
		OperationType		requestedOperation,
		conf::Property*		prop,
		comm::encoder*		enc,
		comm::decoder*		dec
	)
{
	conf::EnumStringProperty* enumProp = static_cast<conf::EnumStringProperty*>(prop);

	switch (requestedOperation) {
		case OP_PLACEHOLDER: {
			ComponentRegistry::InstanceList instList;
			StringVec handles;
			ComponentRegistry::Instance().GetAllInstances(instList);

			ComponentRegistry::InstanceList::const_iterator iter = instList.begin();
			for (; iter != instList.end(); ++iter) {
				const String instStr = util::std2str((*iter)->GetClassId()) + _T("#") +
					boost::lexical_cast<String>((*iter)->GetSerial());
				handles.push_back(instStr);
			}
			return new conf::EnumStringProperty(String(), handles, 0);
		}
		case OP_ENCODE: {
			const String& value = enumProp->GetValue();
			size_t hashPos = value.rfind('#');
			uint serial = boost::lexical_cast<uint>(value.substr(hashPos+1));
			const Handle handle(util::str2std(value.substr(0, hashPos)), serial);
			*enc << handle;
			return 0;
		}
		case OP_DECODE: {
			// TODO: not needed for now
			return 0;
		}
	}
	return 0;
}

//**********************************************************************

ComponentDataTypeConverter::ConversionMap ComponentDataTypeConverter::s_conversions;

////////////////////////////////////////////////////////////////////////

} // namespace ide
