/**
 *	XMLConfigurationLoader.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifndef	XML_CONFIGURATION_LOADER_H
#define	XML_CONFIGURATION_LOADER_H

#include <wx/string.h>
#include <wx/xml/xml.h>

#include "Colour.h"
#include "LayoutParams.h"
#include "TypesConverter.h"
#include "RenderingParams.h"

namespace iviews {
class XMLConfigurationLoader {
public:
	bool		operator() (
					const wxString &	path, 
					LayoutParams &		lparams, 
					RenderingParams &	rParams
				);

private:
	bool		GetLayoutParams (wxXmlNode *child, LayoutParams & lparams);
	bool		GetRenderingParams (wxXmlNode *child, RenderingParams & rParams);

	
	bool		ParseBoolean (const wxString & value, const wxString & type);
	long		ParseLong (const wxString & value, const wxString & type);
	double		ParseDouble (const wxString & value, const wxString & type);
	std::string ParseString (const wxString & value, const wxString & type);
	geo::Colour	ParseColour (const wxString & value, const wxString & type);

};

}	//namespace iviews 

#endif	//XML_CONFIGURATION_LOADER_H