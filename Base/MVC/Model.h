/**
 *	Model.h
 *
 *	-- ide application --
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#ifndef IDE_MODEL_H
#define IDE_MODEL_H

#include "Common.h"
#include "Algorithms.h"
#include "MVCBase.h"

#include <boost/lambda/lambda.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

template <class TModel, class TView>
class Model : public ModelBase {
public:
	typedef TModel								ModelType;
	typedef TView								ViewType;
	typedef std::list<ViewType*>				ViewList;
	typedef boost::function<void (ViewType*)>	ViewFunc;

	Model (void) {}
	virtual ~Model (void)
		{ util::for_all(m_views, std::mem_fun(&ViewBase::ResetModel)); }

	//******************************************************************

	virtual void UpdateAllOtherViews (ViewBase* current) {
		util::for_all_if(m_views, std::mem_fun(&ViewBase::Update),
			boost::lambda::_1 != current);
	}
	virtual void UpdateAllViews (void)
		{ util::for_all(m_views, std::mem_fun(&ViewBase::Update)); }

	virtual void ApplyToAllViewBases (const ViewBaseFunc& func)
		{ util::for_all(m_views, func); }

	virtual void ClearViews (void) { m_views.clear(); }

	//******************************************************************

	void ApplyToAllViews (const ViewFunc& func) { util::for_all(m_views, func); }
	void ApplyToAllViewsSafely (const ViewFunc& func) { util::for_all_safe(m_views, func); }

	//******************************************************************

	virtual ViewList&		GetViews (void) { return m_views; }
	virtual const ViewList& GetViews (void) const { return m_views; }

protected:
	template <class TModel, class TView, class TController> friend class View;
	template <class TModel, class TView, class TController> friend class ViewNoStorage;

	virtual void AppendView (ViewType* view) { m_views.push_back(view); }
	virtual void RemoveView (ViewType* view) { m_views.remove(view); }

private:
	ViewList m_views;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MODEL_H
