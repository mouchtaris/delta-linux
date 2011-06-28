/**
 *	ImageList.h
 *
 *	-- Generic Widgets --
 *
 *	ImageList alternative that allows image retrival based
 *	on an alternative ImageKey instead of index positions.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef IDE_IMAGE_LIST_H_
#define IDE_IMAGE_LIST_H_

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "UndefineAllWin32Shit.h"

#include <wx/imaglist.h>

#include <map>

namespace ide
{
	class _BASE_API ImageList :
		public wxImageList
	{
	public:
		///--- type definitions
		typedef String ImageKey;

		///--- constructors / destructor
		ImageList(void) {}
		ImageList(int width, int height)
			: wxImageList(width, height) {}
		~ImageList(void) {}

		///--- public API
		int Add(const ImageKey& key, const wxBitmap& bitmap);
		bool Remove(const ImageKey& key);

		int ResolveImageIndex(const ImageKey& key) const;
		wxBitmap GetBitmap(const ImageKey& key) const;
		const ImageKey& GetImageKeyByIndex(int index) const;

	private:
		///--- private members
		typedef std::map<ImageKey, int> ImageMapping;
		ImageMapping resolve;
	};
}

#endif	//IDE_IMAGE_LIST_H_
