/**
 *	BitmapRegistry.cpp
 *
 *	-- ResourceRegistries --
 *
 *	Central point for bitmap registration.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "BitmapRegistry.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class BitmapRegistry -----------------------------//

	IMPLEMENT_SINGLETON(BitmapRegistry);

	//-----------------------------------------------------------------------

	void BitmapRegistry::Insert(const String& id, const wxImage& bmp)
	{
		bitmaps[id] = bmp;
	}

	//-----------------------------------------------------------------------

	void BitmapRegistry::Remove(const String& id)
	{
		bitmaps.erase(id);
	}

	//-----------------------------------------------------------------------

	const wxImage& BitmapRegistry::Get(const String& id)
	{
		static wxImage null_entity;

		BitmapMapping::iterator it = bitmaps.find(id);
		return (it == bitmaps.end()) ? null_entity : it->second;
	}

	//-----------------------------------------------------------------------

	bool BitmapRegistry::Exists(const String& id)
	{
		BitmapMapping::iterator it = bitmaps.find(id);
		return (it == bitmaps.end()) ? false : true;
	}

	//-----------------------------------------------------------------------
}
