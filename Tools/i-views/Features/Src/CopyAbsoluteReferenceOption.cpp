/**
 *	CopyAbsoluteReferenceOption.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <wx/string.h>
#include <wx/dataobj.h>
#include <wx/clipbrd.h>

#include "TypesConverter.h"
#include "CopyAbsoluteReferenceOption.h"

namespace iviews {

//-----------------------------------------------------------------------

CopyAbsoluteReferenceOption::CopyAbsoluteReferenceOption(void) {}

//-----------------------------------------------------------------------

CopyAbsoluteReferenceOption::~CopyAbsoluteReferenceOption(){
	if (wxTheClipboard->Open()){
		wxTheClipboard->Clear();
		wxTheClipboard->Close();
	}
}

//-----------------------------------------------------------------------

void CopyAbsoluteReferenceOption::operator()(VertexRenderingInfo * v, VertexToDebugInfoMapping * mapping){
	assert(v);
	assert(mapping);
	
	if (wxTheClipboard->Open()){
		VertexToDebugInfoMapping::const_iterator i;
		if ( (i = mapping->find(v->GetSourceVertex())) != mapping->end() )
			wxTheClipboard->SetData( new wxTextDataObject(std2str(i->second.GetAbsolutRef())) );
		wxTheClipboard->Close();
	}
}


}	//namespace iviews