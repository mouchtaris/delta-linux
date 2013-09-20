/**
 *	AspectResult.h
 *
 *	-- IDE AspectResult Component --
 *
 *	AspectResult component represents 
 *  the result of an aspect transformation.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2013
 */
#ifndef PROJECT_MANAGER_ASPECT_RESULT_H_
#define PROJECT_MANAGER_ASPECT_RESULT_H_

#include "Common.h"
#include "AttachedScript.h"

namespace ide
{
	class AspectResult :
		public AttachedScript
	{
		DECLARE_IDE_COMPONENT(AspectResult);

	public:
		///--- constructors / destructor
		AspectResult(void);
		~AspectResult(void);
	};
}

#endif	//PROJECT_MANAGER_ASPECT_RESULT_H_
