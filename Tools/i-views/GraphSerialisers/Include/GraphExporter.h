/**
 *	GraphExporter.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	January 2011
 */

#ifndef	GRAPH_EXPORTER_H 
#define	GRAPH_EXPORTER_H

#include <wx/string.h>

#include "GraphToSvg.h"
#include "Graph.h"
#include "Vertex.h"
#include "Primitive.h"
#include "RenderingTypes.h"
#include "LayredGraphRenderer.h"
#include "GraphToDot.h"
#include "VertexToDebugInfoMapping.h"

namespace iviews {

class GraphExporter {
public:
	typedef Primitive::PrimitivePtrList PrimitivePtrList;
	
	enum ExportedFileFormats {
		SVG_FORMAT,
		DOT_FORMAT
	};



	~GraphExporter (){}
	GraphExporter (void);
	
	void		 ExportToSVG (
					const length_t			width, 
					const length_t			height, 
					const PrimitivePtrList& primitives
				);

	void		 ExportToDot (
					const graphs::Graph&					g,
					const graphs::Vertex&					start,
					const graphs::VertexToDebugInfoMapping&	debugInfo
				);

private:
	wxString	SelectFile (const wxString & format) const;
	bool		WriteStringToFile (const std::string & str, const std::string & fileName);


	GraphToSvg svg;
	GraphToDot dot;
	LayredGraphRenderer renderer;


};

}	//namespace iviews

#endif	//EXPORTET_H