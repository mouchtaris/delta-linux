/**
 *	GotoSourcePoint.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif


#include <stdlib.h>
#include <string.h>
#include <wx/msgdlg.h>

#include "Call.h"
#include "Common.h"
#include "SlotFinder.h"
#include "TypesConverter.h"
#include "SlotRenderingInfo.h"
#include "GotoSourcePoint.h"


#define TOKENIZE_DELIMITERS ", '"


namespace iviews {

//-----------------------------------------------------------------------

GotoSourcePoint::GotoSourcePoint(void) {}

//-----------------------------------------------------------------------

bool GotoSourcePoint::operator()(VertexRenderingInfo * v)
{
	SlotRenderingInfo * slot = SlotFinder()(v, "{madein}");
	
	if (slot) 
		return ParseMadeinString(slot->GetSlotDebugInfo()->GetContentString());
	
	wxMessageBox(_("Vertex does not have slot with value {madein}"), _("Error"), wxICON_ERROR);
	return false;
}

//-----------------------------------------------------------------------

bool GotoSourcePoint::ParseMadeinString(const std::string & madeinString) {
	StringVector tokens;
	Tokenize(madeinString, tokens, TOKENIZE_DELIMITERS);

	//tokens[0] = file
	//tokens[1] = uri
	//tokens[3] = file line
	if (tokens.size() >= 4 && tokens[0] == "file") {	//4 magic number 
		int line		= atoi(tokens[3].c_str());
		std::string uri = tokens[1];
		ide::Call<void	(const String& uri, int line)>
						("GotoSourcePoint", "DeltaVM", "GotoSymbolicDocument")
						(std2str(uri), line);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------

void GotoSourcePoint::Tokenize(
		const std::string&	str,
		StringVector&		tokens,
		const std::string&	delimiters
	) {
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);	 // Skip delimiters at beginning.
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos); // Find first "non-delimiter".
	
	while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));			// Found a token, add it to the vector.
        lastPos = str.find_first_not_of(delimiters, pos);				// Skip delimiters.  Note the "not_of"
        pos		= str.find_first_of(delimiters, lastPos);				// Find next "non-delimiter"
    }
}


}