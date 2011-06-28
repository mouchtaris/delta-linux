/**
 *	Serialization.h
 *
 *	-- Utility Library --
 *	TODO (29/11/2010): the serialization functions to be moved within a central project where
 *  types and conversions offered by components are placed in distinct headers.
 *	December 2005
 */

#ifndef UTIL_SERIALIZATION_H
#define UTIL_SERIALIZATION_H

#include "Portability.h"
#include "StringDefs.h"
#include "BinaryBuffer.h"

#include <boost/array.hpp>

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

namespace boost {
namespace serialization {

////////////////////////////////////////////////////////////////////////
// Serialization of wxString
//
template<class Archive>
void save (Archive& ar, const wxString& str, const unsigned int PORT_UNUSED_PARAM(version))
	{ ar << util::str2std(str); }

template<class Archive>
void load (Archive& ar, wxString& str, const unsigned int PORT_UNUSED_PARAM(version))
{
	std::string tmp;
	ar >> tmp;
	str = util::std2str(tmp);
}

} // namespace serialization
} // namespace boost

BOOST_SERIALIZATION_SPLIT_FREE(wxString);

#endif // WXWIDGETS

namespace boost {
namespace serialization {

////////////////////////////////////////////////////////////////////////
// Serialization of boost::array
//
template<class Archive, class Type, int N>
inline void serialize(Archive& ar, boost::array<Type, N>& array, const unsigned int version)
	{ ar & array.elems; }

////////////////////////////////////////////////////////////////////////
// Serialization of BinaryBuffer
//
template<class Archive>
void save (Archive& ar, const BinaryBuffer& buffer, const unsigned int version)
{
	const unsigned size = buffer.GetSize();
	void *data = buffer.GetData();
	ar << size;
	for(unsigned i = 0; i < size; ++i)
		ar << ((char *) data)[i];
}
template<class Archive>
void save (Archive& ar, const InputBinaryBuffer& buffer, const unsigned int version)
	{ save(ar, (const BinaryBuffer&) buffer, version); }

template<class Archive>
void save (Archive& ar, const SerializedEditorAST& buffer, const unsigned int version)
	{ save(ar, (const BinaryBuffer&) buffer, version); }

template<class Archive>
void load (Archive& ar, BinaryBuffer& buffer, const unsigned int version)
{
	unsigned size;
	ar >> size;
	void *data = buffer.AllocateSize(size);
	for(unsigned i = 0; i < size; ++i)
		ar >> ((char *) data)[i];
}

template<class Archive>
void load (Archive& ar, InputBinaryBuffer& buffer, const unsigned int version)
	{ load(ar, (BinaryBuffer&) buffer, version); }

template<class Archive>
void load (Archive& ar, SerializedEditorAST& buffer, const unsigned int version)
	{ load(ar, (BinaryBuffer&) buffer, version); }

} // namespace serialization
} // namespace boost

BOOST_SERIALIZATION_SPLIT_FREE(BinaryBuffer);
BOOST_SERIALIZATION_SPLIT_FREE(InputBinaryBuffer);
BOOST_SERIALIZATION_SPLIT_FREE(SerializedEditorAST);

// ATTENTION(29/11/2010): Do not put other overloaded load /save in here. Place them
// in separate component-specific headers.

////////////////////////////////////////////////////////////////////////

#endif // UTIL_SERIALIZATION_H
