/**
 *	StageResult.h
 *
 *	-- IDE StageResult Component --
 *
 *	StageResult component represents the
 *  result produced by a compilation stage.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2013
 */
#ifndef PROJECT_MANAGER_STAGE_RESULT_H_
#define PROJECT_MANAGER_STAGE_RESULT_H_

#include "Common.h"
#include "AttachedScript.h"

namespace ide
{
	class StageResult :
		public AttachedScript
	{
		DECLARE_IDE_COMPONENT(StageResult);

	public:
		///--- constructors / destructor
		StageResult(void);
		~StageResult(void);
	};
}

#endif	//PROJECT_MANAGER_STAGE_RESULT_H_
