/**
 *	SlotTooltip.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	September 2010
 */

#ifndef SLOT_TOOLTIP_H
#define SLOT_TOOLTIP_H


#include "TipTimer.h"
#include "TypesConverter.h"
#include "VertexRenderingInfo.h"


namespace iviews {

class SlotTooltip {
public:

	SlotTooltip(long delay);
	
	bool operator() (
			Window * parent, 
			VertexRenderingInfo * v, 
			const geo::Point<coord_t> & mouseOverVertexPos
		);

	void Stop (void);
	
private:
	long delay;
	TipTimer	timer;
};

}	//namespace iviews

#endif //SLOT_TOOLTIP_H