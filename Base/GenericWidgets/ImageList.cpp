/**
 *	ImageList.cpp
 *
 *	-- Generic Widgets --
 *
 *	ImageList alternative that allows image retrival based
 *	on an alternative ImageKey instead of index positions.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#include "ImageList.h"

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- class ImageList ----------------------------------//

	int ImageList::Add(const ImageKey& key, const wxBitmap& bitmap)
	{
		resolve[key] = GetImageCount();
		return wxImageList::Add(bitmap);
	}

	//-----------------------------------------------------------------------

	bool ImageList::Remove(const ImageKey& key)
	{
		//-- NOTE: intentionally not erasing bitmap from wxImageList as it would only
		//	 corrupt the numbering, without any actual effect

		ImageMapping::iterator it = resolve.find(key);
		if (it == resolve.end())
			return false;

		//int index = it->second;
		resolve.erase(it);
		return true;
	}

	//-----------------------------------------------------------------------

	int ImageList::ResolveImageIndex(const ImageKey& key) const
	{
		ImageMapping::const_iterator it = resolve.find(key);
		if (it != resolve.end())
			return it->second;
		return -1;
	}

	//-----------------------------------------------------------------------

	wxBitmap ImageList::GetBitmap(const ImageKey& key) const
	{
		ImageMapping::const_iterator it = resolve.find(key);
		if (it != resolve.end())
			return wxImageList::GetBitmap(it->second);
		return wxBitmap();
	}

	//-----------------------------------------------------------------------

	const ImageList::ImageKey& ImageList::GetImageKeyByIndex(int index) const
	{
		static ImageKey null_entity;

		BOOST_FOREACH(const ImageMapping::value_type& pair, resolve)
			if (pair.second == index)
				return pair.first;

		return null_entity;
	}

	//-----------------------------------------------------------------------
}
