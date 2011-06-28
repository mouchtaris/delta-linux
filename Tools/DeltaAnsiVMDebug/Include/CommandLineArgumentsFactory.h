#ifndef __OPTIONS_FACTORY__HPP__
#define __OPTIONS_FACTORY__HPP__

#include "CommandLineArgumentsFactory_Forward.h"
#include "CommandLineArguments_Forward.h"
//
#include <iosfwd>

class CommandLineArgumentsFactory {
public:
	static void InitialiseOptionsFromArguments (
					int const				argc,
					char const* const		argv[],
					CommandLineArguments&	options);

	static std::ostream& PrintHelpTo (std::ostream&);
};

#endif //__OPTIONS_FACTORY__HPP__
