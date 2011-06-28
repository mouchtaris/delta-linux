/**
 *	PropertyTable.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "PropertyTable.h"
#include "XMLPropertyVisitor.h"
#include "Adaptors.h"
#include "Algorithms.h"

#include <boost/foreach.hpp>

#include <wx/log.h>

namespace conf {

////////////////////////////////////////////////////////////////////////

bool PropertyTable::LoadValues (const String& uri)
{
#ifdef NDEBUG
	wxLogNull shutDownWarnings;
#endif

	wxXmlDocument doc;
	if (!doc.Load(uri))
		return false;

	xml::Node root(doc.GetRoot());
	XMLValueLoader loader;
	loader.SetParent(root);
	const PropertyMap& propertyMap = this->GetPropertyMap();
	BOOST_FOREACH(const PropertyMap::value_type& propPair, propertyMap)
		propPair.second->Accept(propPair.first, &loader);

	m_dirty = false;
	return true;
}

//**********************************************************************

bool PropertyTable::SaveValues (const String& uri)
{
	if (!m_dirty)
		return true;

	xml::Node root;
	root.Create(_T("PropertyTable"));

	XMLValueSaver saver;
	saver.SetParent(root);
	const PropertyMap& propertyMap = this->GetPropertyMap();
	BOOST_FOREACH(const PropertyMap::value_type& propPair, propertyMap)
		propPair.second->Accept(propPair.first, &saver);

	wxXmlDocument doc;
	doc.SetFileEncoding(_T("utf-8"));
	doc.SetRoot(root.NativeType());
	m_dirty = !doc.Save(uri, 2);

	return !m_dirty;
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
