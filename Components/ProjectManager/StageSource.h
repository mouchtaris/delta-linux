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
#include "Script.h"

namespace ide
{
	class StageSource :
		public Script
	{
		DECLARE_IDE_COMPONENT(StageSource);

	public:
		///--- type definitions

		///--- constructors / destructor
		StageSource(void);
		~StageSource(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION(virtual const std::string, GetType, (void));

		DECLARE_EXPORTED_FUNCTION(void, Open, (void));

	protected:
		///--- protected API
		virtual void ComponentAddedProperty (const std::string& id, conf::Property* prop);
	};
}

#endif	//PROJECT_MANAGER_STAGE_SOURCE_H_
