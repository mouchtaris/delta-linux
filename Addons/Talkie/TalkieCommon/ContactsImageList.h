/**
 *	ContactsImageList.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_CONTACTSIMAGELIST_H
#define TALKIE_CONTACTSIMAGELIST_H

#include "Common.h"
class wxImageList;
class wxBitmap;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class ContactsImageList {
public:
	static void Initialize (void);
	static bool IsInitialized (void);
	static void CleanUp (void);

	static wxImageList*		GetImageList (void);
	static const String*	GetCaptionList (void);
	static wxBitmap			GetImage (int index);

private:
	DISABLE_CONSTRUCTION(ContactsImageList);

	static wxImageList* s_imageList;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_CONTACTSIMAGELIST_H
