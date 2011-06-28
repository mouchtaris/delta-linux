/**
 *	ModelFilter.h
 *
 *	-- ide application --
 *
 *	December 2005
 */

#ifndef IDE_MODELFILTER_H
#define IDE_MODELFILTER_H

#include "Model.h"
#include <list>

namespace ide {

////////////////////////////////////////////////////////////////////////

template <class TView, class TModel>
class ModelFilter : public Model<TView> {
public:
	typedef TModel					ModelType;
	typedef std::list<ModelType*>	ModelList;

	ModelFilter (void) {}
	virtual ~ModelFilter (void) {}

	//******************************************************************

	ModelList&			GetModels (void) { return m_models; }
	const ModelList&	GetModels (void) const { return m_models; }

	void AppendView (ModelType* model) { m_models.push_back(model); }
	void RemoveView (ModelType* model) { m_models.remove(model);

	//******************************************************************
	// Helper methods
	//
	template <class TFunc>
	TFunc ForAllModels (PARAM(TFunc) func) { return util::for_all(m_models, func); }

	template <class TFunc>
	TFunc ForAllModelsSafe (PARAM(TFunc) func) { return util::for_all_safe(m_models, func); }

private:
	ModelList m_models;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MODELFILTER_H
