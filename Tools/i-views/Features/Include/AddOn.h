//sempteber 2010

#ifndef	ADD_ON_H
#define	ADD_ON_H


#include "CommonTypes.h"
#include "ColourAddOnData.h"
#include "LayersRenderingInfo.h"


namespace iviews {
	
class AddOn {
public:
	AddOn (void);
	id_t Id (void) const ;

	virtual void	Undo (LayersRenderingInfo * layers, ColourAddOnUndoData * undoData) = 0;
	virtual ColourAddOnUndoData * 
					Apply (LayersRenderingInfo * layers, ColourAddOnData * data) = 0;

	virtual void 	RemovedFromManager (void) = 0;
	virtual bool 	CanAddInManager (const ColourAddOnData * data) = 0;
	virtual bool 	CanRemoveFromManager (const ColourAddOnData * data) = 0;

	virtual bool 	operator== (const AddOn * addOn) const;
	virtual bool 	operator!= (const AddOn * addOn) const;

protected:
	id_t id;

};

}	//namespace iviews 


#endif	//ADD_ON_H