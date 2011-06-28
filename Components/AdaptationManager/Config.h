/**
 *	Config.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Config component is a configuration resource
 *	for the IDE's Adaptation Manager that allows
 *  the user to select it's invocation priority.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	January 2009
 */
#ifndef IDE_CONFIG_COMPONENT_H_
#define IDE_CONFIG_COMPONENT_H_

#include "Common.h"

#include "TextFile.h"

namespace ide
{
	class Config :
		public TextFile
	{
		DECLARE_IDE_COMPONENT(Config);

	public:
		///--- type definitions

		///--- constructors / destructor
		Config(void);
		~Config(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION(void, MoveUp, (void));
		DECLARE_EXPORTED_FUNCTION(void, MoveDown, (void));
	};
}

#endif	//IDE_CONFIG_COMPONENT_H_
