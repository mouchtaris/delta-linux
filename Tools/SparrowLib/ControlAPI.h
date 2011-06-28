/**
 *	ControlAPI.cpp
 *
 *	-- Sparrow library for Delta --
 *
 *	Exported Component Control API for Sparrow.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef SPARROW_CONTROL_API_FOR_DELTA_H_
#define SPARROW_CONTROL_API_FOR_DELTA_H_

#include "SparrowLibCommon.h"
#include "ComponentHandle.h"

class DeltaVirtualMachine;

namespace ide
{
	class Component;

	//----------------------------
	//-- class ControlAPI

	class _SPARROW_LIB_API ControlAPI
	{
	public:
		///--- public API
		static void Install(void);
		static void Uninstall(void);
	private:
		static void Print(DeltaVirtualMachine* vm);
	};
}

#endif	//SPARROW_CONTROL_API_FOR_DELTA_H_
