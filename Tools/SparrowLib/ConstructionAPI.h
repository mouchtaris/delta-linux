/**
 *	ConstructionAPI.h
 *
 *	-- Sparrow library for Delta --
 *
 *	Exported Component Construction API for Sparrow.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	August 2007
 */
#ifndef SPARROW_CONSTRUCTION_API_FOR_DELTA_H_
#define SPARROW_CONSTRUCTION_API_FOR_DELTA_H_

#include "SparrowLibCommon.h"
#include "ComponentHandle.h"

namespace ide
{
	class Component;

	//----------------------------
	//-- class ConstructionAPI

	class _SPARROW_LIB_API ConstructionAPI
	{
	public:
		///--- public API
		static void Install(void);
		static void Uninstall(void);
	};
}

#endif	//SPARROW_CONSTRUCTION_API_FOR_DELTA_H_
