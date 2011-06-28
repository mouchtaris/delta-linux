/**
 *	IDECore.h
 *
 *	-- Delta IDE Core --
 *
 *	Core IDE dll exporting communication interface
 *	of the IDE with its plugins/components.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_CORE_H_
#define IDE_CORE_H_

#include "Common.h"
#include "IDECommon.h"

namespace ide
{
	//----------------------------
	//-- class IDECore

	class _IDE_API IDECore
	{
	public:
		///--- type definitions

		///--- public API
		static bool Initialize(const std::string& componentConfiguration, int32 messageRouterServerNegotiationPort);
		static void CleanUp(void);
		static bool Run(const std::string& profile, const std::string& workspace);

		static const std::string GetInstallationDir(void);
	private:
		///--- constructors / destructor
		IDECore(void) {}
		~IDECore(void) {}

		///--- private members
		static std::string installationDirectory;
	};
}

#endif	//IDE_CORE_H_
