/**
 *	MVCBase.h
 *
 * See Docs/MVC.txt for further information
 *
 *	-- ide application --
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef IDE_MVCBASE_H
#define IDE_MVCBASE_H

#include "Common.h"
#include <boost/function.hpp>
#include <wx/window.h>

////////////////////////////////////////////////////////////////////////
// Fwd decls & typedefs
//
class wxDropTarget;

typedef wxWindow		Window;
typedef wxDropTarget	DropTarget;

namespace ide {

////////////////////////////////////////////////////////////////////////

class ModelBase;
class ControllerBase;

////////////////////////////////////////////////////////////////////////

class ViewBase {
public:
	ViewBase (void) {}
	virtual ~ViewBase (void) {}

	virtual void Update (void) = 0;

	virtual Window*			GetWindow (void) = 0;
	virtual ModelBase*		GetModelBase (void) = 0;
	virtual ControllerBase*	GetControllerBase (void) = 0;

	virtual void ResetModel (void);

	// View common attributes
	//
	void FreezeUpdates (void);
	void ThawUpdates (void);

protected:
	virtual void OnResetModel (void) = 0;
};

////////////////////////////////////////////////////////////////////////

class ModelBase {
public:
	typedef boost::function<void (ViewBase*)> ViewBaseFunc;

	ModelBase (void) {}
	virtual ~ModelBase (void) {}

	virtual void UpdateAllOtherViews (ViewBase* current) = 0;
	virtual void UpdateAllViews (void) = 0;

	virtual void ApplyToAllViewBases (const ViewBaseFunc& func) = 0;
	virtual void ClearViews (void) = 0;
};

////////////////////////////////////////////////////////////////////////

class ControllerBase {
public:
	ControllerBase (void) {}
	virtual ~ControllerBase (void) {}

	virtual ModelBase*	GetMonitoredModelBase (void) = 0;
	virtual ViewBase*	GetMonitoredViewBase (void) = 0;

	// Drop target support. Caller deletes
	//
	virtual DropTarget*	CreateDropTarget (void) { return 0; }

	virtual void PopupContextMenu (void) = 0;
	virtual void ResetMonitoredModel (void);

protected:
	virtual void OnResetMonitoredModel (void) = 0;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MVCBASE_H
