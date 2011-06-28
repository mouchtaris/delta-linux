/**
 *	EditorModuleLoader.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EDITOR_EDITORMODULELOADER_H
#define EDITOR_EDITORMODULELOADER_H

#include "EditorUtils/EditorCommon.h"
#include "EditorInterfaces/LanguageModuleIface.h"

#include <map>

namespace editor {

////////////////////////////////////////////////////////////////////////

enum ModuleType {
	MT_LANGUAGE = 1 << 0,
};

////////////////////////////////////////////////////////////////////////

class ModuleInstance;

////////////////////////////////////////////////////////////////////////

class _EDITOR_API EditorModuleLoader {
public:
	struct ModuleEntry {
		ModuleEntry (ModuleInstance* instance, long flags = MT_LANGUAGE) :
			instance(instance), flags(flags), refCount(1) {}

		ModuleInstance* instance;
		long			flags;
		int				refCount;
	};

	static void Initialize (void);
	static void CleanUp (void);

	static LanguageModuleIface*	GetLanguageModule (const String& path = String());
	static void	DestroyLanguageModule (const String& path, LanguageModuleIface* module);

private:
	DISABLE_COPY(EditorModuleLoader);

	typedef std::map<const String, ModuleEntry> ModMap;
	static ModMap s_modMap;
};

////////////////////////////////////////////////////////////////////////

} // namespace editor

#endif // EDITOR_EDITORMODULELOADER_H
