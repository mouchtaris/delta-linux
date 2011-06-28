/**
 *	DynamicCodeManager.cpp
 *
 *	-- Managing of the dynamic code files --
 *
 *	Dynamic code manager front end.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	April 2009
 */

#include "DynamicCodeManager.h"
#include "ComponentRegistry.h"
#include "Call.h"
#include "CommonGUIUtils.h"

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	DynamicCodeManager::DynamicCodeFileMap DynamicCodeManager::dynamicFiles;
	static const std::string s_classId = "DeltaVM";

	//-------------------------------------------------------//
	//---- class DynamicCodeManager -------------------------//

	DynamicCodeManager::DynamicCodeFile::DynamicCodeFile(const std::string& symbolic, const std::string& code) :
		symbolic(symbolic), code(code)
	{
		Component* shell = ComponentRegistry::Instance().GetFocusedInstance("Shell");
		assert(shell);
		GUI_SCOPED_FREEZE(shell->GetWindow());
		editor = Call<const Handle& (void)>(s_classId, "EditorManager", "NewEditor")();
		const String uri = util::std2str(GetURI());
		Call<void (const String&)>(s_classId, editor, "SetURI")(uri);
		Call<void (const String&)>(s_classId, editor, "SetLanguageSettings")(uri);
		Call<void (const String&)>(s_classId, editor, "SetText")(util::std2str(code));
		Call<void (bool)>(s_classId, editor, "SetReadOnly")(true);
		Call<void (bool)>(s_classId, editor, "SetModified")(false);
	}

	const std::string DynamicCodeManager::DynamicCodeFile::GetURI(void) const
		{ return symbolic + ".dsc"; }

	//-----------------------------------------------------------------------

	void DynamicCodeManager::Register(const std::string& symbolic, const std::string& code)
	{
		dynamicFiles[symbolic] = DynamicCodeFile(symbolic, code);
		//Now that the dynamic file is registered, reset the uri to trigger a FileNameChanged 
		//that will this time be properly handled regarding breakpoint loading.
		const Handle& editor = dynamicFiles[symbolic].GetEditor();
		const String uri = util::std2str(dynamicFiles[symbolic].GetURI());
		Call<void (const String&)>(s_classId, editor, "SetURI")(uri);
	}

	//-----------------------------------------------------------------------

	void DynamicCodeManager::Update(const std::string& symbolic, const std::string& code)
	{
		Component* shell = ComponentRegistry::Instance().GetFocusedInstance("Shell");
		assert(shell);
		GUI_SCOPED_FREEZE(shell->GetWindow());
		DynamicCodeFileMap::iterator i = dynamicFiles.find(symbolic);
		assert(i != dynamicFiles.end());
		const Handle& editor = i->second.GetEditor();
		Call<void (bool)>(s_classId, editor, "SetReadOnly")(false);
		Call<void (const String&)>(s_classId, editor, "SetText")(util::std2str(code));
		Call<void (bool)>(s_classId, editor, "SetReadOnly")(true);
		Call<void (bool)>(s_classId, editor, "SetModified")(false);
		const String uri = util::std2str(i->second.GetURI());
		Call<void (const String&)>(s_classId, editor, "SetURI")(uri);
	}

	//-----------------------------------------------------------------------

	void DynamicCodeManager::Cancel(const std::string& symbolic)
	{
		DynamicCodeFileMap::iterator i = dynamicFiles.find(symbolic);
		if (i != dynamicFiles.end())
			dynamicFiles.erase(i);
	}

	//-----------------------------------------------------------------------

	void DynamicCodeManager::Cancel(const Handle& editor)
	{
		assert(editor.GetClassId() == "Editor");
		for (DynamicCodeFileMap::iterator i = dynamicFiles.begin(); i != dynamicFiles.end(); ++i)
			if (i->second.GetEditor() == editor) {
				dynamicFiles.erase(i);
				break;
			}
	}

	//-----------------------------------------------------------------------

	const Handle DynamicCodeManager::GetEditor(const std::string& symbolic)
	{
		DynamicCodeFileMap::iterator i = dynamicFiles.find(symbolic);
		return i != dynamicFiles.end() ? i->second.GetEditor() : Handle();
	}

	//-----------------------------------------------------------------------
	
	const std::string DynamicCodeManager::GetURI(const std::string& symbolic)
	{
		DynamicCodeFileMap::iterator i = dynamicFiles.find(symbolic);
		return i != dynamicFiles.end() ? i->second.GetURI() : "";
	}

	//-----------------------------------------------------------------------
	
	const std::string DynamicCodeManager::GetSymbolicURI(const std::string& uri)
	{
		BOOST_FOREACH(const DynamicCodeFileMap::value_type& entry, dynamicFiles)
			if (entry.second.GetURI() == uri)
				return entry.first;
		return "";
	}

	//-----------------------------------------------------------------------
	
	const std::string DynamicCodeManager::GetSymbolicURI(const Handle& editor)
	{
		BOOST_FOREACH(const DynamicCodeFileMap::value_type& entry, dynamicFiles)
			if (entry.second.GetEditor() == editor)
				return entry.first;
		return "";
	}
}