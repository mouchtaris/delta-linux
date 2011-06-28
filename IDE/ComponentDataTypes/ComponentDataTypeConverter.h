/**
 *	ComponentDataTypeConverter.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTDATATYPECONVERTER_H
#define IDE_COMPONENTDATATYPECONVERTER_H

#include "Common.h"
#include "IDECommon.h"
#include "Properties.h"

#include <map>

namespace comm {
	class encoder;
	class decoder;
} // comm

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentDataTypeConverter {
public:
	enum OperationType { OP_PLACEHOLDER, OP_ENCODE, OP_DECODE };

	typedef conf::Property* (*ConversionFunc) (
			const std::string&	typeId,
			OperationType		requestedOperation,
			conf::Property*		prop,
			comm::encoder*		enc,
			comm::decoder*		dec
		);

	static void Initialize (void);
	static void CleanUp (void);

	//******************************************************************

	static void InstallConverter (const std::string& type, ConversionFunc func);

	//******************************************************************

	static conf::Property* TypeToPropertyPlaceholder (const std::string& type);

	//******************************************************************

	static void	EncodeType (
			comm::encoder&		enc,
			const std::string&	type,
			conf::Property*		prop
		);

	static conf::Property* DecodeType (
			comm::decoder&		dec,
			const std::string&	type
		);

private:
	DISABLE_CONSTRUCTION(ComponentDataTypeConverter);

	//******************************************************************
	// Default conversionFuncs
	//
	static conf::Property* convertPrimitive (
			const std::string&	typeId,
			OperationType		requestedOperation,
			conf::Property*		prop,
			comm::encoder*		enc,
			comm::decoder*		dec
		);
	static conf::Property* convertHandle (
			const std::string&	typeId,
			OperationType		requestedOperation,
			conf::Property*		prop,
			comm::encoder*		enc,
			comm::decoder*		dec
		);

	//******************************************************************

	typedef std::map<const std::string, ConversionFunc> ConversionMap;
	static ConversionMap s_conversions;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTDATATYPECONVERTER_H
