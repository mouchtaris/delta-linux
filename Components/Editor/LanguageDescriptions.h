/**
 *	LanguageDescriptions.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_LANGUAGEDESCRIPTIONS_H
#define IDE_LANGUAGEDESCRIPTIONS_H

#include "Common.h"
#include "Singleton.h"
#include "LibraryAPIDecls.h"
#include "Properties.h"

namespace editor {
	class EditorWindow;
}

namespace ide {

////////////////////////////////////////////////////////////////////////

extern const String GetLanguageDescriptionFile(void);

////////////////////////////////////////////////////////////////////////

class LanguageDescriptions : public util::Singleton {
	DECLARE_SINGLETON(LanguageDescriptions);
public:
	conf::Property* GetHighlightingProperties (void) const;

	void SetLanguage (const String& uri, editor::EditorWindow* editor);

	void SetLanguageHighlighting (
			const String&			uri,
			editor::EditorWindow*	editor,
			const conf::Property*	highlightingProperties
		);

protected:
	void OnInitialize (void);
	void OnCleanUp (void);

private:
	typedef std::map<const String, const String> ExtMappings;

	const String& getLanguage (const String& uri);

	ExtMappings m_extMappings;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_LANGUAGEDESCRIPTIONS_H
