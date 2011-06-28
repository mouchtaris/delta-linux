/**
 *	ExtSerialization.h
 *
 *	-- Utility Library --
 *	All code in here should be part of the Serialization header.
 *  Nevertheless, including the <wx/font> or <wx/color> there causes the application
 *  to launch without creating a visible window (no idea why).
 *  So instead, this header should be used anywhere one of the following  
 *  classes is used in the component system: wxFont, wxColour wxDateTime
 *	May 2010
 */

#ifndef UTIL_EXT_SERIALIZATION_H
#define UTIL_EXT_SERIALIZATION_H

// Disable warning regarding conversion from size_t to unsigned int as it
// is used in the boost serialization headers and produces many warnings.
#pragma warning(disable: 4267)	

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/base_object.hpp>

namespace util {

////////////////////////////////////////////////////////////////////////
// Typedefs
//
typedef boost::archive::text_iarchive default_iarchive;
typedef boost::archive::text_oarchive default_oarchive;

} // namespace util

#ifdef WXWIDGETS
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/datetime.h>

namespace boost {
namespace serialization {

////////////////////////////////////////////////////////////////////////

template<class Archive>
void save (Archive& ar, const wxFont& font, const unsigned int PORT_UNUSED_PARAM(version))
	{ ar << util::str2std(font.GetNativeFontInfoDesc()); }

template<class Archive>
void load (Archive& ar, wxFont& font, const unsigned int PORT_UNUSED_PARAM(version))
{
	std::string nativeFontDesc;
	ar >> nativeFontDesc;
	font.SetNativeFontInfo(util::std2str(nativeFontDesc));
}

//**********************************************************************

template<class Archive>
void save (Archive& ar, const wxColour& color, const unsigned int PORT_UNUSED_PARAM(version))
{
	const unsigned int value = static_cast<unsigned int>(color.GetPixel());
	ar << value;
}

template<class Archive>
void load (Archive& ar, wxColour& color, const unsigned int PORT_UNUSED_PARAM(version))
{
	unsigned int pixel;
	ar >> pixel;
	color.Set(pixel);
}

//**********************************************************************

template<class Archive>
void save (Archive& ar, const wxDateTime& date, const unsigned int PORT_UNUSED_PARAM(version))
{
	const time_t value = date.GetTicks();
	ar << value;
}

template<class Archive>
void load (Archive& ar, wxDateTime& date, const unsigned int PORT_UNUSED_PARAM(version))
{
	time_t ticks;
	ar >> ticks;
	date.Set(ticks);
}

//**********************************************************************

} // namespace serialization
} // namespace boost

//**********************************************************************

BOOST_SERIALIZATION_SPLIT_FREE(wxFont);
BOOST_SERIALIZATION_SPLIT_FREE(wxColour);
BOOST_SERIALIZATION_SPLIT_FREE(wxDateTime);

#endif // WXWIDGETS

////////////////////////////////////////////////////////////////////////

#endif // UTIL_EXT_SERIALIZATION_H
