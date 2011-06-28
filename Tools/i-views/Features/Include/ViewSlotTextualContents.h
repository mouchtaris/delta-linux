//july 2010
//Open a window that contains informations about the slot' s contents
#ifndef	VIEW_SLOT_TEXTUAL_CONTENTS_H
#define	VIEW_SLOT_TEXTUAL_CONTENTS_H


#include "TypesConverter.h"
#include "RenderingTypes.h"
#include "SlotRenderingInfo.h"

namespace iviews {
class ViewSlotTextualContents {

public:
	ViewSlotTextualContents(void);

	SlotRenderingInfo * operator()(
							Window * parent,
							VertexRenderingInfo * v, 
							const coord_t & x, 
							const coord_t & y
						);
};

}	//namespace iviews

#endif	VIEW_SLOT_TEXTUAL_CONTENTS_H