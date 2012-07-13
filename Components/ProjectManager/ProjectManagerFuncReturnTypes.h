/**
 *	ProjectManagerFuncReturnTypes.h
 *
 *	-- Return types for functions of the ProjectManager component --
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2012
 */
#ifndef IDE_PROJECT_MANAGER_FUNC_RETURN_TYPES_H_
#define IDE_PROJECT_MANAGER_FUNC_RETURN_TYPES_H_

#include "Common.h"
#include "ComponentHandle.h"
#include <boost/tuple/tuple.hpp>

typedef boost::tuple<bool, ide::HandleList, StringList>	ScriptDependencies;

#endif	//IDE_PROJECT_MANAGER_FUNC_RETURN_TYPES_H_