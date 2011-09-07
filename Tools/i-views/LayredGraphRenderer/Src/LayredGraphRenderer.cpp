/*	LayredGraphRenderer.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	july 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif


#include <assert.h>
#include "LayredGraphRenderer.h"

namespace iviews {

//-----------------------------------------------------------------------

void LayredGraphRenderer::SetRenderingAPI (RenderingAPI * api_)
	{ api = api_; }

//-----------------------------------------------------------------------

void LayredGraphRenderer::operator()(const PrimitivePtrList & primitiveList) {
	for(PrimitivePtrList::const_iterator i = primitiveList.begin(); i != primitiveList.end(); ++i) {
		assert(*i);
		if ((*i)->IsShowing())
			switch( (*i)->Type()){
				case Primitive::RECTANGLE:
					api->DrawRectangle(static_cast<Rectangle *>((*i)));
					break;
				case Primitive::TEXT:
					api->DrawText(static_cast<Text *>((*i)));
					break;
				case Primitive::ARROW:
					api->DrawArrow(static_cast<Arrow *>((*i)));
					break;
				case Primitive::CURVE_ARROW:
					api->DrawCurveArrow(static_cast<CurveArrow *>((*i)));
					break;
				case Primitive::SPLICE_ARROW:
					api->DrawSpliceArrow(static_cast<SpliceArrow *>((*i)));
					break;
				default:
					assert(0);
			};
	}
	
}


}	//namespace iviews