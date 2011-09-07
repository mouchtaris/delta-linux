/**
 *	SlotFinder.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifndef	SLOT_FINDER_H
#define SLOT_FINDER_H

#include <string>
#include "RenderingTypes.h"
#include "SlotRenderingInfo.h"
#include "LayersRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {
	
class SlotFinder {
public:
	SlotRenderingInfo * operator()(
							LayersRenderingInfo * layersRenInfo, 
							const coord_t & x, 
							const coord_t & y
						);

	SlotRenderingInfo * operator()(
							VertexRenderingInfo * v, 
							const coord_t & x, 
							const coord_t & y
						);

	SlotRenderingInfo * operator()(
							VertexRenderingInfo * v, 
							const std::string & text
						);

private:
	typedef SlotRenderingInfo::SlotRenderingInfoPtrList SlotRenderingInfoPtrList;

};

}	//namespace iviews

#endif	//SLOT_FINDER_H