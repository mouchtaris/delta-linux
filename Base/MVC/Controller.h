/**
 *	ControllerNoStorage.h
 *
 *	-- ide application --
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef IDE_CONTROLLER_H
#define IDE_CONTROLLER_H

#include "Common.h"
#include "MVCBase.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// Controller
//
template <class TModel, class TView, class TController>
class Controller : public ControllerBase {
public:
	typedef TModel		ModelType;
	typedef TView		ViewType;
	typedef TController	ControllerType;

	Controller (void) : m_model(0), m_view(0) {}
	virtual ~Controller (void) {}

	virtual ModelBase*	GetMonitoredModelBase (void) { return m_model; }
	virtual ViewBase*	GetMonitoredViewBase (void) { return m_view; }

	virtual ModelType*	GetMonitoredModel (void) { return m_model; }
	virtual ViewType*	GetMonitoredView (void) { return m_view; }

protected:
	template <class TModel, class TView, class TController> friend class View;
	template <class TModel, class TView, class TController> friend class ViewNoStorage;

	virtual void ResetMonitoredModel (void) {
		m_model = 0;
		this->ControllerBase::ResetMonitoredModel();
	}
	virtual void SetMonitoredModel (ModelType* model) {
		m_model = model;
		this->OnSetMonitoredModel(model);
	}
	virtual void SetMonitoredView (ViewType* view) {
		m_view = view;
		this->OnSetMonitoredView(view);
	}

	// virtual void OnResetMonitoredModel (void) = 0;

	virtual void OnSetMonitoredModel (ModelType* model) = 0;
	virtual void OnSetMonitoredView (ViewType* view) = 0;

private:
	ModelType*	m_model;
	ViewType*	m_view;
};

////////////////////////////////////////////////////////////////////////
// Controller with no storage that may be used for constructing MVC aggregates
//
template <class TModel, class TView, class TController>
class ControllerNoStorage : public ControllerBase {
public:
	typedef TModel		ModelType;
	typedef TView		ViewType;
	typedef TController	ControllerType;

	ControllerNoStorage (void) {}
	virtual ~ControllerNoStorage (void) {}

	virtual ModelBase*	GetMonitoredModelBase (void) { return this->GetMonitoredModel(); }
	virtual ViewBase*	GetMonitoredViewBase (void) { return this->GetMonitoredView(); }

	virtual ModelType*	GetMonitoredModel (void) = 0;
	virtual ViewType*	GetMonitoredView (void) = 0;

protected:
	template <class TModel, class TView, class TController> friend class View;
	template <class TModel, class TView, class TController> friend class ViewNoStorage;

	virtual void SetMonitoredModel (ModelType* model) { this->OnSetMonitoredModel(model); }
	virtual void SetMonitoredView (ViewType* view) { this->OnSetMonitoredView(view); }

	// virtual void OnResetMonitoredModel (void) = 0;

	virtual void OnSetMonitoredModel (ModelType* model) = 0;
	virtual void OnSetMonitoredView (ViewType* view) = 0;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_CONTROLLER_H
