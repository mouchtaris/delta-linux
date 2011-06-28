#include <assert.h>
#include "ObjectsGrouping.h"

namespace iviews {


void ObjectsGrouping::operator() (VertexToDebugInfoMapping * debugInfo) {
	assert(debugInfo);
	
	mapping.clear();
	
	VertexToDebugInfoMapping::iterator i;

	for(i = debugInfo->begin(); i != debugInfo->end(); ++i) 
		AddConditionalInMadeinToVertices(i->first, i->second.GetAllSlots());

}

//-----------------------------------------------------------------------

void ObjectsGrouping::AddConditionalInMadeinToVertices(
		Vertex * vertex, 
		const SlotDebugInfoList & slots
	) {
	assert(vertex);
	
	for(SlotDebugInfoList::const_iterator slot = slots.begin(); slot != slots.end(); ++slot) {
		if ((*slot).GetKeyString() == "{madein}") {
			MadeinToVertices::iterator vertices = mapping.find((*slot).GetContentString());

			if (vertices == mapping.end())
				mapping[(*slot).GetContentString()].push_back(vertex);
			else
				vertices->second.push_back(vertex);
			break;
		}
	}
}




}	//namespace iviews 