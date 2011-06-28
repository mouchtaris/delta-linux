/**
 *	DeltaVMFuncReturnTypes.h
 *
 *	-- Managing of the dynamic code files --
 *
 *	Dynamic code manager front end.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	April 2009
 */
#ifndef IDE_DELTAVM_FUNC_RETURN_TYPES_H_
#define IDE_DELTAVM_FUNC_RETURN_TYPES_H_

#include "Common.h"
#include <boost/tuple/tuple.hpp>

typedef std::list<StringList>											StringListList;

typedef boost::tuple<std::string, bool, std::string, bool, std::string>	ObjectNodeTuple;
typedef std::list<ObjectNodeTuple>										ObjectNodeList;
typedef std::map<std::string, ObjectNodeList>							ObjectGraphMap;
typedef boost::tuple<ObjectGraphMap, std::string, bool>					ObjectGraphData;

#endif	//IDE_DELTAVM_FUNC_RETURN_TYPES_H_