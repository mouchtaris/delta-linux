/**
 *	EditorModuleLoader.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "EditorModuleLoader.h"
#include "EditorInterfaces/LanguageModuleIface.h"

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

namespace editor {

////////////////////////////////////////////////////////////////////////
// ModuleInstance class
//
class ModuleInstance {
public:
	ModuleInstance (const String& path);
	~ModuleInstance (void);

	void* Create (const std::string& factory);

private:
	HINSTANCE m_handle;
};

//**********************************************************************

ModuleInstance::ModuleInstance (const String& path)
{
#ifndef NDEBUG
	String realPath = path + _T("_d.dll");
#else
	String realPath = path + _T(".dll");	
#endif

	m_handle = LoadLibraryW(realPath.c_str());
}

//**********************************************************************

ModuleInstance::~ModuleInstance (void)
{
	if (m_handle)
		FreeLibrary(m_handle);
}

//**********************************************************************

void* ModuleInstance::Create (const std::string& factory)
{
	typedef void* (*FactoryFunc)(void);

	if (m_handle) {
		if (FactoryFunc func = (FactoryFunc) GetProcAddress(m_handle, factory.c_str()))
			return func();
		return 0;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Null Language Module
//
class NullLanguageModule : public LanguageModuleIface {
public:
	virtual void ContextMenuOpened (uint x, uint y) {}

	virtual void ContentSetup (void) {}
	virtual void ContentCleared (void) {}

	virtual void ContentAdded (uint, uint, uint) {}
	virtual void ContentDeleted (uint, uint, uint) {}
	virtual void ContentOffsetChanged (uint, int) {}

	// Added for i-sense (AS).
	virtual void CursorRepositioned (uint atPos) {}
	virtual void MouseHoverStarted  (uint atPos){}
	virtual void MouseHoverEnded  (void){}
	virtual const std::string GetCursorFocusInformation (void) const { return "no focus information"; }

	virtual void CharacterAdded (int keyCode) {}
	virtual bool GetInfoForPosition (uint, uint*, uint*, int*, String*) { return false; }
	virtual void CancelDisplayedInfo (void) {}
	virtual void ClearIndicators (void) {}
	virtual void SetExtraLibraryDefinitions (const StringList&) {}
	virtual void SetByteCodeLoadingPath (const std::string& path) {}
	virtual const IntList GetHotspots (void) const { return IntList(); }
	virtual const SerializedEditorAST GetAST (void) const { return SerializedEditorAST((void *) 0, 0); }
	virtual void SharedWriteAST (void) const {}
	virtual bool  GotoGlobalDefinition (const std::string& id) { return false; }
};

//**********************************************************************

static NullLanguageModule s_nullLangModule;

////////////////////////////////////////////////////////////////////////
// EditorModuleLoader class
//
void EditorModuleLoader::Initialize (void)
{

}

//**********************************************************************

void EditorModuleLoader::CleanUp (void)
{
	ModMap::iterator iter = s_modMap.begin();
	for (; iter != s_modMap.end(); ++iter)
		delete iter->second.instance;
	s_modMap.clear();
}

//**********************************************************************

LanguageModuleIface* EditorModuleLoader::GetLanguageModule (const String& path)
{
	if (path.empty())
		return &s_nullLangModule;

	ModMap::iterator iter = s_modMap.find(path);
	if (iter != s_modMap.end()) {
		assert(iter->second.instance);
		if (void* object = iter->second.instance->Create("CreateLanguageModule")) {
			++iter->second.refCount;
			iter->second.flags |= MT_LANGUAGE;
			return (LanguageModuleIface*) object;
		}
		return &s_nullLangModule;
	}
	else {
		ModuleInstance* mod = new ModuleInstance(path);
		if (void* object = mod->Create("CreateLanguageModule")) {
			s_modMap.insert(std::make_pair(path, ModuleEntry(mod, MT_LANGUAGE)));
			return (LanguageModuleIface*) object;
		}
		else {
			delete mod;
			return &s_nullLangModule;
		}
	}
}

//**********************************************************************

void EditorModuleLoader::DestroyLanguageModule (const String& path, LanguageModuleIface* module)
{
	if (!module)
		return;

	ModMap::iterator iter = s_modMap.find(path);
	if (iter != s_modMap.end()) {
		assert(iter->second.flags & MT_LANGUAGE);
		assert(iter->second.refCount > 0);
		delete module;
		if (!--iter->second.refCount) {
			delete iter->second.instance;
			s_modMap.erase(iter);
		}
	}
	else
		assert(module == &s_nullLangModule);
}

//**********************************************************************

EditorModuleLoader::ModMap EditorModuleLoader::s_modMap;

////////////////////////////////////////////////////////////////////////

} // namespace editor
