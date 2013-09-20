/**
 *	StageSource.h
 *
 *	-- IDE StageSource Component --
 *
 *	StageSource component is the component
 *  representing a staged compilation output.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2011
 */
#ifndef PROJECT_MANAGER_STAGE_SOURCE_H_
#define PROJECT_MANAGER_STAGE_SOURCE_H_

#include "Common.h"
#include "AttachedScript.h"

namespace ide
{
	class StageSource :
		public AttachedScript
	{
		DECLARE_IDE_COMPONENT(StageSource);

	public:
		///--- constructors / destructor
		StageSource(void);
		~StageSource(void);
	};
}

#endif	//PROJECT_MANAGER_STAGE_SOURCE_H_
