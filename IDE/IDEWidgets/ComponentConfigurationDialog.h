/**
 *	ComponentConfigurationDialog.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTCONFIGURATIONDIALOG_H
#define IDE_COMPONENTCONFIGURATIONDIALOG_H

#include "Common.h"
#include "IDECommon.h"
#include "DefaultGUIGenerator.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;
class ComponentEntry;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentConfigurationDialog {
public:
	typedef conf::DefaultGUIGenerator::ChangeCallback ChangeCallback;

	ComponentConfigurationDialog (wxWindow* parent = 0);
	~ComponentConfigurationDialog (void) {}

	bool Configure (void);
	bool Configure (ComponentEntry& entry);
	bool Configure (const std::string& classId);

	bool ConfigureInstance (Component* component);

	void SetOnConfigurationDialogChange(ChangeCallback cb);
	void ApplyConfiguration (void);

private:
	wxWindow*					m_parent;
	conf::DefaultGUIGenerator	m_dgg;
	ChangeCallback				m_changeCallback;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTCONFIGURATIONDIALOG_H
