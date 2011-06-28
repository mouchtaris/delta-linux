/**
 *	PropertiesFactory.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "PropertiesFactory.h"
#include "Properties.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

void PropertiesFactory::Initialize (void)
{
	INSTALL_PRIMITIVE_PROPERTY(IntProperty, "int");
	InstallFactoryAlias("uint", "int");
	InstallFactoryAlias("size_t", "int");
	InstallFactoryAlias("unsigned", "int");
	INSTALL_PRIMITIVE_PROPERTY(BoolProperty, "bool");

	INSTALL_PRIMITIVE_PROPERTY(RealProperty, "real");
	InstallFactoryAlias("double", "real");
	InstallFactoryAlias("float", "real");

	INSTALL_PRIMITIVE_PROPERTY(StringProperty, "String");
	InstallFactoryAlias("std::string", "String");
}

//**********************************************************************

void PropertiesFactory::CleanUp (void)
{

}

//**********************************************************************

void PropertiesFactory::InstallFactory (const std::string& id, CreationFunc factory)
{
	s_factoryMap[id] = factory;
}

//**********************************************************************

void PropertiesFactory::InstallFactoryAlias (const std::string& alias, const std::string& id)
{
	FactoryMap::const_iterator iter = s_factoryMap.find(id);
	if (iter != s_factoryMap.end())
		InstallFactory(alias, iter->second);
}

//**********************************************************************

Property* PropertiesFactory::Create (const std::string& id, const String& label, const String& desc)
{
	FactoryMap::const_iterator iter = s_factoryMap.find(id);
	if (iter != s_factoryMap.end())
		return iter->second(label, desc);
	return 0;
}

//**********************************************************************

PropertiesFactory::FactoryMap PropertiesFactory::s_factoryMap;

////////////////////////////////////////////////////////////////////////

} // namespace conf
