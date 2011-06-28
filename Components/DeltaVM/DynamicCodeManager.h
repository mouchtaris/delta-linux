/**
 *	DynamicCodeManager.h
 *
 *	-- Managing of the dynamic code files --
 *
 *	Dynamic code manager front end.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	April 2009
 */
#ifndef IDE_DYNAMIC_DOCE_MANAGER_H_
#define IDE_DYNAMIC_DOCE_MANAGER_H_

#include "Common.h"
#include "Component.h"
#include "ComponentHandle.h"

#include <map>

namespace ide
{
	//----------------------------
	//-- class DynamicCodeManager

	class DynamicCodeManager {
	private:
		class DynamicCodeFile {
		private:
			std::string symbolic;
			std::string code;
			Handle editor;

		public:
			DynamicCodeFile(void) {}
			DynamicCodeFile(const std::string& symbolic, const std::string& code);
			~DynamicCodeFile() {}

			const std::string&	GetSymbolicURI	(void) const { return symbolic; }
			const std::string&	GetCode			(void) const { return code; }
			const Handle&		GetEditor		(void) const { return editor; }
			const std::string	GetURI			(void) const;
		};

		typedef std::map<std::string, DynamicCodeFile> DynamicCodeFileMap;
		static DynamicCodeFileMap dynamicFiles;

	public:
		static void Register(const std::string& symbolic, const std::string& code);
		static void Update	(const std::string& symbolic, const std::string& code);
		static void Cancel	(const std::string& symbolic);
		static void Cancel	(const Handle& editor);

		static const Handle			GetEditor		(const std::string& symbolic);
		static const std::string	GetURI			(const std::string& symbolic);
		static const std::string	GetSymbolicURI	(const std::string& uri);
		static const std::string	GetSymbolicURI	(const Handle& editor);
	};
}

#endif	//IDE_DYNAMIC_DOCE_MANAGER_H_