/**
 *	Aspect.h
 *
 *	-- IDE Aspect Component --
 *
 *	Aspect component is the component
 *  representing a script aspect transformation.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2012
 */
#ifndef PROJECT_MANAGER_ASPECT_H_
#define PROJECT_MANAGER_ASPECT_H_

#include "Common.h"
#include "Script.h"

namespace ide
{
	class Aspect :
		public Script
	{
		DECLARE_IDE_COMPONENT(Aspect);

	public:
		///--- type definitions

		///--- constructors / destructor
		Aspect(void);
		~Aspect(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION(void, MoveUp, (void));
		DECLARE_EXPORTED_FUNCTION(void, MoveDown, (void));

	protected:
		///--- protected API
	};
}

#endif	//PROJECT_MANAGER_ASPECT_H_
