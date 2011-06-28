/**
 *	ComponentSpy.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTSPY_H
#define IDE_COMPONENTSPY_H

#include "Common.h"
#include "Component.h"
#include "DockableComponent.h"
#include "ComponentBuilder.h"
#include "ComponentSpyWindow.h"
#include "LibraryAPIDecls.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class ComponentSpy : public ComponentSpyWindow, public DockableComponent {
	DECLARE_IDE_COMPONENT(ComponentSpy);

public:
	ComponentSpy (void) {}
	~ComponentSpy (void) {}

	//******************************************************************
	// Component virtual members implementation
	//
	virtual wxWindow* GenerateWindow (wxWindow* parent);
	virtual wxWindow* GetWindow (void);

	DECLARE_EXPORTED_STATIC_(void, View, (void), _("Show the Component Spy"));

protected:
	void ComponentAppliedChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed);

private:
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTSPY_H
