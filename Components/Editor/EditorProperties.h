/**
 *	EditorProperties.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_EDITORPROPERTIES_H
#define IDE_EDITORPROPERTIES_H

#include "Common.h"
#include "Singleton.h"
#include "LibraryAPIDecls.h"
#include "Properties.h"

class wxColour;

namespace editor {
	class EditorWindow;
}

namespace ide {

////////////////////////////////////////////////////////////////////////

class StyleProperty : public conf::AggregateProperty {
public:
	typedef StyleProperty ValueType;

	StyleProperty (
		const String&	label,
		wxColour		fgColor,
		wxColour		bgColor,
		const String&	desc = String()
	);
	~StyleProperty (void);

	void SetValue (const StyleProperty& p);
	
	void Apply (editor::EditorWindow* editor, int styleId) const;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_EDITORPROPERTIES_H
