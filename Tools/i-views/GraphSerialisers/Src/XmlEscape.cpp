/**
 *	XmlEscape.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#include "XmlEscape.h"
#include "StringUtils.h"

namespace iviews {

bool		XmlEscape::isInitialised;
bool		XmlEscape::isCleanedUp;
XmlEscape::XMLEntities		
			XmlEscape::xmlEntities;

//-----------------------------------------------------------------------

void XmlEscape::Initialise (void) {
	if (!IsInitialised()) {
		xmlEntities['"']	= "&quot;";
		xmlEntities['&']	= "&amp;";
		xmlEntities['\'']	= "&apos;";
		xmlEntities['<']	= "&lt;";
		xmlEntities['>']	= "&gt;";
		isInitialised = true;
	}
}

//-----------------------------------------------------------------------

void XmlEscape::CleanUp (void) {
	if (!IsCleanedIp()) {
		isInitialised	= false;
		isCleanedUp		= true;
	}
}

//-----------------------------------------------------------------------

bool XmlEscape::IsInitialised (void) 
	{ return isInitialised; }

//-----------------------------------------------------------------------

bool XmlEscape::IsCleanedIp (void)
	{ return isCleanedUp; }

//-----------------------------------------------------------------------

std::string	XmlEscape::StringToXMLString (const std::string & str) {
	if (!IsInitialised())
		Initialise();
	std::string xmlString;
	XMLEntities::const_iterator entitie;

	for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
		if ( (entitie  = xmlEntities.find(*i)) != xmlEntities.end() )
			xmlString.append(entitie->second);
		else
			xmlString.append(utility::string_cast(*i));

	return xmlString;		
}




}	//namespace iviews