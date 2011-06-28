/**
*	AppInit.h
*
*	-- Delta IDE --
*
*	AppInit class initializes the IDE's subsystems
*	and components.
*
*	Themistoklis Bourdenas <themis@ics.forth.gr>
*	March 2007
*/
#ifndef APP_INIT_H_
#define APP_INIT_H_

#include "Common.h"
#include "Singleton.h"

namespace ide
{
	class AppInit :
		public util::Singleton
	{
	public:
		///--- public API
		static void Initialize(void);
		static void CleanUp(void);

	private:
		///--- constructors / destructor
		AppInit(void) {}
		~AppInit(void) {}
	};
}

#endif	//APP_INIT_H_
