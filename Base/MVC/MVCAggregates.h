/**
 *	MVCAggregates.h
 *
 *	The commented functions should be redefined
 *
 *	-- ide application --
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef IDE_MVCAGGREGATES_H
#define IDE_MVCAGGREGATES_H

#include "Common.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// ViewController
//
template <class TModel, class TView, class TController>
class ViewController : public TView, public TController {
public:
	typedef TModel		ModelType;
	typedef TView		ViewType;
	typedef TController	ControllerType;

	ViewController (void) : m_model(0) { this->SetController(this); }
	virtual ~ViewController (void) {}

	// virtual void		Update (void) = 0;
	// virtual Window*	GetWindow (void) = 0;

	virtual ModelType*		GetModel (void) { return m_model; }
	virtual ControllerType*	GetController (void) { return static_cast<ControllerType*>(this); }

	virtual ModelType*	GetMonitoredModel (void) { return m_model; }
	virtual ViewType*	GetMonitoredView (void) { return static_cast<ViewType*>(this); }

protected:
	// virtual void OnResetModel (void) = 0;
	virtual void OnResetMonitoredModel (void) { m_model = 0; }

	// virtual void OnSetModel (ModelType* model) = 0;
	virtual void OnSetController (ControllerType* controller, bool own) {}

	virtual void OnSetMonitoredModel (ModelType* model) { m_model = model; }
	virtual void OnSetMonitoredView (ViewType* view) {}

private:
	ModelType* m_model;
};

////////////////////////////////////////////////////////////////////////
// ModelViewController
//
template <class TModel, class TView, class TController>
class ModelViewController : public TModel, public TView, public TController {
public:
	typedef TModel		ModelType;
	typedef TView		ViewType;
	typedef TController	ControllerType;

	ModelViewController (void) {
		this->SetController(this);
		this->SetModel(this);
	}
	virtual ~ModelViewController (void) {}

	// virtual void		Update (void) = 0;
	// virtual Window*	GetWindow (void) = 0;

	virtual ModelType*		GetModel (void) { return static_cast<ModelType*>(this); }
	virtual ControllerType*	GetController (void) { return static_cast<ControllerType*>(this); }

	virtual ModelType*	GetMonitoredModel (void) { return static_cast<ModelType*>(this); }
	virtual ViewType*	GetMonitoredView (void) { return static_cast<ViewType*>(this); }

protected:
	// virtual void OnResetModel (void) = 0;
	virtual void OnResetMonitoredModel (void) {}

	// virtual void OnSetModel (ModelType* model) = 0;
	virtual void OnSetController (ControllerType* controller) {}

	virtual void OnSetMonitoredModel (ModelType* model) {}
	virtual void OnSetMonitoredView (ViewType* view) {}
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MVCAGGREGATES_H
