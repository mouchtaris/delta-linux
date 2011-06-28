/**
 *	BitmapRegistry.h
 *
 *	-- ResourceRegistries --
 *
 *	Central point for bitmap registration.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef COMPONENT_BITMAP_REGISTRY_H_
#define COMPONENT_BITMAP_REGISTRY_H_

#include "Common.h"
#include "IDECommon.h"

#include "Singleton.h"

#include "UndefineAllWin32Shit.h"	///< wx show us MERCY! if I remove this header the main window will not appear due to wxImage include
#include <wx/image.h>

#include <map>

namespace ide
{
	//----------------------------
	//-- class BitmapRegistry

	class _IDE_API BitmapRegistry :
		public util::Singleton
	{
		DECLARE_SINGLETON(BitmapRegistry);

	public:
		///--- type definitions

		///--- public API
		void Insert(const String& id, const wxImage& bmp);
		void Remove(const String& id);

		const wxImage& Get(const String& id);
		bool Exists(const String& id);

	private:
		void OnInitialize(void) { wxInitAllImageHandlers(); }
		void OnCleanup(void) { bitmaps.clear(); }

		///--- private members
		typedef std::map<String, wxImage> BitmapMapping;
		BitmapMapping bitmaps;
	};
}

#endif	//COMPONENT_BITMAP_REGISTRY_H_
