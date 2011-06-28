/**
 *	PropertiesFactory.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_PROPERTIESFACTORY_H
#define CONF_PROPERTIESFACTORY_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <map>

namespace conf {

////////////////////////////////////////////////////////////////////////

#define INSTALL_PRIMITIVE_PROPERTY(className, id) \
	InstallFactory(id, &className::CreateProperty)

////////////////////////////////////////////////////////////////////////

class Property;

////////////////////////////////////////////////////////////////////////

class _BASE_API PropertiesFactory {
public:
	typedef Property* (*CreationFunc) (const String&, const String&);

	static void Initialize (void);
	static void CleanUp (void);

	static void	InstallFactory (const std::string& id, CreationFunc factory);
	static void	InstallFactoryAlias (const std::string& alias, const std::string& id);

	static Property* Create (
			const std::string&	id,
			const String&		label = String(),
			const String&		desc = String()
		);

private:
	DISABLE_CONSTRUCTION(PropertiesFactory);

	typedef std::map<const std::string, CreationFunc> FactoryMap;

	static FactoryMap s_factoryMap;
};

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_PropertiesFactory_H
