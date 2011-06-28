//july 2010
#ifndef HIGHLIGHT_RELATIVE_SLOTS_H
#define	HIGHLIGHT_RELATIVE_SLOTS_H


#include "AddOn.h"
#include "Colour.h"
#include "ColourAddOnData.h"
#include "EdgeRenderingInfo.h"
#include "LayersRenderingInfo.h"


namespace iviews {
	
class HighlightRelativeSlots : public AddOn {
public:
	typedef geo::Colour	Colour;

	~HighlightRelativeSlots(){}
	HighlightRelativeSlots (const Colour & c = Colour());


	void				SetSlotsColour (const Colour & c);
	const Colour &		GetSlotsColour (void) const;

	virtual void		Undo (LayersRenderingInfo * layers, ColourAddOnUndoData * undoData);
	virtual bool		CanAddInManager (const ColourAddOnData * data);
	virtual void		RemovedFromManager (void);
	virtual bool		CanRemoveFromManager (const ColourAddOnData * data);
			
	//if the return value is not null then caller is responsible to delete the pointer
	virtual ColourAddOnUndoData * 
						Apply (LayersRenderingInfo * layers, ColourAddOnData * data);

private:	
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList		EdgeRenderingInfoPtrList;
	typedef HighlightRelativeSlotsUndoData
			::SlotDebugInfoUndoRenderingDataPairList		SlotDebugInfoUndoRenderingDataPairList;

	Colour	slotsColour;
};


}	//namespace iviews 
#endif	//HIGHLIGHT_RELATIVE_SLOTS_H