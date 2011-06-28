#ifndef OBJECTS_GROUPING_H
#define OBJECTS_GROUPING_H

#include <map>
#include <string>
#include "AddOn.h"
#include "Vertex.h"
#include "VertexToDebugInfoMapping.h"

namespace iviews {
	
class ObjectsGrouping : public AddOn {
public:
	typedef graphs::VertexToDebugInfoMapping VertexToDebugInfoMapping;

	void operator() (VertexToDebugInfoMapping * debugInfo);

private:
	typedef graphs::Vertex						Vertex;
	typedef Vertex::VerticesPtrList				VerticesPtrList;
	typedef dinfo::DebugInfo::SlotDebugInfoList	SlotDebugInfoList;
	//key madein 
	//all vertices that have the same made in 
	typedef std::map<std::string, VerticesPtrList> MadeinToVertices;

	void AddConditionalInMadeinToVertices(
		Vertex * vertex, 
		const SlotDebugInfoList & slots
	);

	MadeinToVertices mapping;

};

}	//namespace iviews

#endif	//OBJECTS_GROUPING_H