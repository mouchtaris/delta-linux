/**
 *	ViewNoStorage.h
 *
 *	-- ide application --
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef IDE_VIEW_H
#define IDE_VIEW_H

#include "Common.h"
#include "MVCBase.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

template <class TModel, class TView, class TController>
class View : public ViewBase {
public:
	typedef TModel		ModelType;
	typedef TView		ViewType;
	typedef TController	ControllerType;

	View (void) : m_model(0), m_controller(0), m_ownController(false) {}
	virtual ~View (void) {
		if (m_model)
			m_model->RemoveView(static_cast<ViewType*>(this));
		if (m_ownController)
			delete m_controller;
	}

	// virtual void		Update (void) = 0;
	// virtual Window*	GetWindow (void) = 0;

	virtual ModelBase*		GetModelBase (void) { return m_model; }
	virtual ControllerBase*	GetControllerBase (void) { return m_controller; }

	virtual ModelType*		GetModel (void) { return m_model; }
	virtual ControllerType* GetController (void) { return m_controller; }

	virtual void ResetModel (void) {
		m_model = 0;
		this->ViewBase::ResetModel();
	}
	virtual void SetController (ControllerType* controller, bool own = false) {
		if (m_controller && m_ownController) // Replace controller
			delete m_controller;

		m_controller = controller;
		m_ownController = own;

		this->GetWindow()->SetDropTarget(controller->CreateDropTarget());

		this->OnSetController(controller, own);
		controller->SetMonitoredView(static_cast<ViewType*>(this));

		if (m_model)
			controller->SetMonitoredModel(m_model);
	}
	virtual void SetModel (ModelType* model) {
		m_model = model;
		model->AppendView(static_cast<ViewType*>(this));
		this->OnSetModel(model);

		if (m_controller)
			m_controller->SetMonitoredModel(model);
	}

protected:
	// virtual void OnResetModel (void) = 0;

	virtual void OnSetModel (ModelType* model) = 0;
	virtual void OnSetController (ControllerType* controller, bool own) = 0;

private:
	ModelType*		m_model;
	ControllerType*	m_controller;
	bool			m_ownController;
};

////////////////////////////////////////////////////////////////////////
// View with no storage that may be used for constructing MVC aggregates
//
template <class TModel, class TView, class TController>
class ViewNoStorage : public ViewBase {
public:
	typedef TModel		ModelType;
	typedef TView		ViewType;
	typedef TController	ControllerType;

	ViewNoStorage (void) {}
	virtual ~ViewNoStorage (void) {
		if (ModelType* model = this->GetModel())
			model->RemoveView(static_cast<ViewType*>(this));
	}

	// virtual void		Update (void) = 0;
	// virtual Window*	GetWindow (void) = 0;

	virtual ModelBase*		GetModelBase (void) { return this->GetModel(); }
	virtual ControllerBase*	GetControllerBase (void) { return this->GetController(); }

	virtual ModelType*		GetModel (void) = 0;
	virtual ControllerType* GetController (void) = 0;

	virtual void SetController (ControllerType* controller, bool own = true) {
		this->OnSetController(controller, own);
		controller->SetMonitoredView(static_cast<ViewType*>(this));
	}
	virtual void SetModel (ModelType* model) {
		this->OnSetModel(model);
		model->AppendView(static_cast<ViewType*>(this));

		assert(this->GetController());
		this->GetController()->SetMonitoredModel(model);
	}

protected:
	// virtual void OnResetModel (void) = 0;

	virtual void OnSetModel (ModelType* model) = 0;
	virtual void OnSetController (ControllerType* controller, bool own) = 0;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_VIEW_H
