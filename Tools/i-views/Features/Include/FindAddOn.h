#ifndef	FIND_ADD_ON_H
#define	FIND_ADD_ON_H

#include "AddOn.h"
#include "Vertex.h"
#include "Colour.h"
#include "SlotRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {

class FindAddOn : public AddOn {
public:
	typedef graphs::Vertex Vertex;

	FindAddOn(const geo::Colour & c = geo::Colour(0, 255, 255)); //OK

	geo::Colour			GetSelectionColour(void); //OK
	void				SetSelectionColour(const geo::Colour & c); //OK


	virtual void		Undo(LayersRenderingInfo * layers, ColourAddOnUndoData * undoData); //OK
	virtual ColourAddOnUndoData * 
						Apply(LayersRenderingInfo * layers, ColourAddOnData * data);	//OK
	
	virtual void 		RemovedFromManager(void);
	virtual bool 		CanAddInManager(const ColourAddOnData * data);
	virtual bool 		CanRemoveFromManager(const ColourAddOnData * data);


private:
	typedef SlotRenderingInfo::SlotRenderingInfoPtrList SlotRenderingInfoPtrList;

	SlotRenderingInfo* GetNthSlotRenderingInfoFromList(
							SlotRenderingInfoPtrList & slots,
							const int distance
						);

	FindUndoData*		ApplyInVertexWithNoContents(
							FindAddOnData		* findAddOnData,
							VertexRenderingInfo * vertex
						);

	FindUndoData*		ApplyInVertexWithFullContents(
							FindAddOnData		* findAddOnData,
							VertexRenderingInfo * vertex
						);

	void				 SetSlotOldColour(
							FindUndoData		* findUndoData,
							VertexRenderingInfo * vertex
						);



	geo::Colour selectionColour;

};

}	//namespace iviews

#endif	//FIND_ADD_ON_H