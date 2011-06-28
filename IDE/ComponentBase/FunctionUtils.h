/**
 *	FunctionUtils.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_FUNCTIONUTILS_H
#define IDE_FUNCTIONUTILS_H

#include "Common.h"
#include "IDECommon.h"

#include <vector>

namespace ide {

////////////////////////////////////////////////////////////////////////

typedef std::pair<std::string, std::string> Argument;
typedef std::vector<Argument>				ArgumentVec;

//**********************************************************************

void _IDE_API FunctionDecomposeArgs (ArgumentVec& result, const std::string& argList);
void _IDE_API FunctionDecomposeRetType (std::string& result, const std::string& retType);

const std::string _IDE_API FunctionGetArgList (const ArgumentVec& args);

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_FUNCTIONUTILS_H
