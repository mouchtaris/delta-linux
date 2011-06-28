/**
 *	LanguageDescriptions.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "LanguageDescriptions.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "EditorProperties.h"
#include "StringUtils.h"
#include "IDECore.h"
#include "xml.h"

#include "EditorMain/EditorWindow.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>

////////////////////////////////////////////////////////////////////////
// Module initialization and destruction
//
UTIL_C_EXPORT void Initialize (void)
{
	ide::LanguageDescriptions::Initialize();
}

//**********************************************************************

UTIL_C_EXPORT void CleanUp (void)
{
	ide::LanguageDescriptions::CleanUp();
}

namespace ide {

////////////////////////////////////////////////////////////////////////

const String GetLanguageDescriptionFile(void) {
	return util::std2str(
		util::normalizepath(
			IDECore::GetInstallationDir() + "/EditorLanguages.xml"
		)
	);
}

////////////////////////////////////////////////////////////////////////

static wxColour _ParseColourRGB (const String& strColor)
{
	if (strColor.empty())
		return wxNullColour;

	StringList result;
	boost::split(result, strColor, boost::is_space());

	StringList::iterator iter = result.begin(), end = result.end();
	if (iter != end) {
		int r = boost::lexical_cast<int>(*iter++);
		int g = iter != end ? boost::lexical_cast<int>(*iter++) : 0;
		int b = iter != end ? boost::lexical_cast<int>(*iter) : 0;
		return wxColour(r, g, b);
	}
	return wxNullColour;
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(LanguageDescriptions);

//**********************************************************************

void LanguageDescriptions::OnInitialize(void)
{
	wxXmlDocument doc;
	if (!doc.Load(GetLanguageDescriptionFile()))
		return;

	xml::Node root(doc.GetRoot());
	xml::Node::iterator iter = root.ChildBegin(_T("Language"));
	xml::Node::iterator end = root.ChildEnd(_T("Language"));
	for (; iter != end; ++iter) {

		// Get language name & extensions
		//
		const String language = iter->GetProperty(_T("name"));

		StringList result;
		boost::split(result, iter->GetProperty(_T("ext")), boost::is_space());

		// Register languages
		//
		BOOST_FOREACH(const String& ext, result)
			m_extMappings.insert(std::make_pair(ext, language));
	}
}

//**********************************************************************

void LanguageDescriptions::OnCleanUp(void)
{

}

//**********************************************************************

conf::Property* LanguageDescriptions::GetHighlightingProperties (void) const
{
	conf::AggregateProperty* highlightProp = new conf::AggregateProperty(
			_("Highlighting"), _("Highlighting options per language")
		);

	wxXmlDocument doc;
	if (!doc.Load(GetLanguageDescriptionFile()))
		return 0;

	xml::Node root(doc.GetRoot());
	xml::Node::iterator iter = root.ChildBegin(_T("Language"));
	xml::Node::iterator end = root.ChildEnd(_T("Language"));
	for (; iter != end; ++iter) {

		const String language = iter->GetProperty(_T("name"));

		// Set language specific properties
		//
		conf::AggregateProperty* langProp = new conf::AggregateProperty(language);

		xml::Node::iterator styleIter = iter->ChildBegin(_T("StyleDef"));
		xml::Node::iterator styleIterEnd = iter->ChildEnd(_T("StyleDef"));
		for (; styleIter != styleIterEnd; ++styleIter) {

			StyleProperty* styleProp = new StyleProperty(
					styleIter->GetProperty(_T("name")),
					_ParseColourRGB(styleIter->GetProperty(_T("fgColor"))),
					_ParseColourRGB(styleIter->GetProperty(_T("bgColor")))					
				);

			langProp->AddProperty(util::str2std(styleIter->GetProperty(_T("id"))), styleProp);
		}
		highlightProp->AddProperty(util::str2std(language), langProp);
	}
	return highlightProp;
}

//*********************************************************************

void LanguageDescriptions::SetLanguage (const String& uri, editor::EditorWindow* editor)
{
	const String& language = this->getLanguage(uri);

	// Clear previous lexer
	//
	editor->SetLexer(wxSCI_LEX_NULL);
	editor->UnLoadLanguageModule();

	// Read the XML file and set the attributes
	//
	wxXmlDocument doc;
	if (language == _T("default") || !doc.Load(GetLanguageDescriptionFile()))
		return;

	xml::Node root(doc.GetRoot());
	xml::Node::iterator iter = root.ChildBegin(_T("Language"));
	xml::Node::iterator end = root.ChildEnd(_T("Language"));
	for (; iter != end; ++iter) {

		if (iter->GetProperty(_("name")) == language) {

			// Set language lexer
			//
			const String lexerStr = iter->GetProperty(_T("lexerId"));
			if (!lexerStr.empty())
				editor->SetLexer(boost::lexical_cast<int>(lexerStr));
			else {
				const String lexerName = iter->GetProperty(_T("lexerName"));
				int oldLexer = editor->GetLexer();
				if (!lexerName.empty())
					editor->SetLexerLanguage(lexerName);

				if (editor->GetLexer() == oldLexer)
					editor->SetLexer(wxSCI_LEX_NULL);
			}

			// Set language keywords
			//
			xml::Node::iterator keyIter = iter->ChildBegin(_T("Keywords"));
			xml::Node::iterator keyIterEnd = iter->ChildEnd(_T("Keywords"));
			for (int seqId = 0; keyIter != keyIterEnd; ++keyIter, ++seqId) {
				const String strId = keyIter->GetProperty(_T("id"));
				int id = strId.empty() ? seqId : boost::lexical_cast<int>(strId);
				editor->SetKeyWords(id, keyIter->GetContent());
			}

			// Set parser properties
			//
			xml::Node::iterator propIter = iter->ChildBegin(_T("Property"));
			xml::Node::iterator propIterEnd = iter->ChildEnd(_T("Property"));
			for (; propIter != propIterEnd; ++propIter) {
				editor->SetProperty(propIter->GetProperty(_T("name")),
					propIter->GetProperty(_T("value")));
			}

			// Load language extension modules
			//
			xml::Node langExt = iter->GetChild(_T("LangExt"));
			if (!langExt.isNull())
				editor->LoadLanguageModule(langExt.GetProperty(_T("path")));

			// bail-out
			//
			break;
		}
	}
}

//*********************************************************************

void LanguageDescriptions::SetLanguageHighlighting (
		const String&			uri,
		editor::EditorWindow*	editor,
		const conf::Property*	highlightingProperties
	)
{
	const String& language = this->getLanguage(uri);

	if (language == _T("default"))
		return;

	// Set styles for the specific language
	//
	const conf::AggregateProperty* styleProp =
		conf::safe_prop_cast<const conf::AggregateProperty>(
			conf::safe_prop_cast<const conf::AggregateProperty>(
					highlightingProperties
			)->GetProperty(util::str2std(language))
		);

	BOOST_FOREACH(const conf::PropertyMap::value_type& entry, styleProp->GetPropertyMap()) {

		StdStringList result;
		boost::split(result, entry.first, boost::is_space());

		BOOST_FOREACH(const std::string& idStr, result) {
			int id = boost::lexical_cast<int>(idStr);
			conf::safe_prop_cast<const StyleProperty>(entry.second)->Apply(editor, id);
		}
	}
}

//*********************************************************************

const String& LanguageDescriptions::getLanguage (const String& uri)
{
	static const String defaultLang(_T("default"));

	String::size_type extPos = uri.find_last_of(_T('.'));
	extPos = (extPos == String::npos) ? String::npos : extPos + 1;
	const String extension = uri.substr(extPos, uri.length() - extPos);

	ExtMappings::const_iterator iter = m_extMappings.find(extension);
	if (iter != m_extMappings.end())
		return iter->second;
	return defaultLang;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
