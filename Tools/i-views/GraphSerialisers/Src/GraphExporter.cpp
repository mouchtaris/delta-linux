/**
 *	GraphExporter.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	January 2011
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <string>
#include <fstream>

#include <wx/filedlg.h>

#include "GraphExporter.h"
#include "TypesConverter.h"


namespace iviews {

//-----------------------------------------------------------------------

GraphExporter::GraphExporter (void) :
	svg(),
	dot(),
	renderer (&svg)
{}

//-----------------------------------------------------------------------

void GraphExporter::ExportToSVG (const length_t width, const length_t height, const PrimitivePtrList & primitives) {
	std::string file = str2std(SelectFile(wxT("SVG files (*.svg)|*.svg|")));
	svg.BeginDrawing(width, height);
	renderer(primitives);
	svg.EndDrawing();
	WriteStringToFile(svg.GetSvgStringRepresentation(), file);
}

//-----------------------------------------------------------------------

void GraphExporter::ExportToDot (
		const graphs::Graph&					g,
		const graphs::Vertex&					start,
		const graphs::VertexToDebugInfoMapping&	debugInfo
	) {
	dot.WriteToFile(
		str2std(SelectFile(wxT("DOT files (*.dot)|*.dot|"))),
		g,
		start, 
		true, 
		debugInfo
	);
}

//-----------------------------------------------------------------------

wxString GraphExporter::SelectFile (const wxString & format) const {
    return wxFileSelector( 
				wxT("Export to:"),
				wxEmptyString,
				wxEmptyString,
				(const wxChar *)NULL,
				format,
				wxFD_SAVE,
				wxGetActiveWindow()
			);
}

//-----------------------------------------------------------------------

bool GraphExporter::WriteStringToFile (const std::string & str, const std::string & fileName) {
	bool result = false;
	
	std::fstream filestr(fileName.c_str(), std::fstream::out);
	if (filestr.is_open()) {
		filestr << str;
		filestr.close();
		result = true;
	}
	
	return result;
}


}	//namespace iviews 