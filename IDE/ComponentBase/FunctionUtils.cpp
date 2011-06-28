/**
 *	FunctionUtils.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "FunctionUtils.h"
#include "Algorithms.h"
#include "StringUtils.h"

#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

static const std::string _UndecorateType (const StdStringVec& type, size_t len)
{
	static std::string sauce[] = { "&", "*", "const", "volatile" };
	static std::string* sauceEnd = sauce + SIZEOF_ARRAY(sauce);

	while (len--) {
		if (std::find(sauce, sauceEnd, type[len]) == sauceEnd)
			return util::strip(type[len], "*&");
	}
	return std::string("void");
}

////////////////////////////////////////////////////////////////////////

void FunctionDecomposeArgs (ArgumentVec& result, const std::string& argList)
{
	std::string normalizedArgList = argList;
	if (argList.length() > 1 && argList[0] == '(')
		normalizedArgList = argList.substr(1, argList.length() - 2);

	std::list<std::string> args;
	util::stringtokenizer(args, normalizedArgList, ",");

	std::list<std::string>::const_iterator iter = args.begin();
	for (; iter != args.end(); ++iter) {
		StdStringVec arg;
		util::stringtokenizer(arg, *iter, UTIL_WHITESPACE);

		Argument resultArg;

		if (arg.size() == 1) {
			if (*iter == "void")
				return;
			resultArg.first = *iter;
		}
		else {
			resultArg.first = _UndecorateType(arg, arg.size() - 1);
			resultArg.second = arg[arg.size() - 1];
		}
		result.push_back(resultArg);
	}
}

//**********************************************************************

void FunctionDecomposeRetType (std::string& result, const std::string& retType)
{
	StdStringVec type;
	util::stringtokenizer(type, retType, UTIL_WHITESPACE);
	result = _UndecorateType(type, type.size());
}

//**********************************************************************

const std::string FunctionGetArgList (const ArgumentVec& args)
{
	ArgumentVec::const_iterator iter = args.begin();
	if (iter == args.end())
		return "";

	std::string result = iter->first;
	if (!iter->second.empty())
		result += " " + iter->second;
	while (++iter != args.end()) {
		result += ", " + iter->first;
		if (!iter->second.empty())
			result += " " + iter->second;
	}
	return result;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
