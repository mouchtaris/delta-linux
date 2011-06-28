/**
 *	ContactsImageList.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ContactsImageList.h"
#include <wx/imaglist.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// ContactsImageList class
//
void ContactsImageList::Initialize (void)
{
	const String imageResources[] = {
		_T("person"),
		_T("person_red"),
		_T("person_prince"),
		_T("person_bender"),
		_T("person_towelie"),
		_T("person_owl"),
		_T("person_notis"),
		_T("person_guitar"),
		_T("person_cartman"),
		_T("person_alig"),
	};

	s_imageList = new wxImageList(32, 32, true, SIZEOF_ARRAY(imageResources) + 1);
	wxBitmap defaultImage(_T("default"));
	for (uint i = 0; i < SIZEOF_ARRAY(imageResources); ++i) {
		wxBitmap image(imageResources[i]);
		s_imageList->Add((image.Ok() ? image : defaultImage));
	}
	s_imageList->Add(defaultImage);
}

//**********************************************************************

bool ContactsImageList::IsInitialized (void)
{
	return !!s_imageList;
}

//**********************************************************************

void ContactsImageList::CleanUp (void)
{
	delete s_imageList;
	s_imageList = 0;
}

//**********************************************************************

wxImageList* ContactsImageList::GetImageList (void)
{
	return s_imageList;
}

//**********************************************************************

const String* ContactsImageList::GetCaptionList (void)
{
	static const String captions[] = {
		_("A person"),
		_("A red person"),
		_("The Prince"),
		_("Bender the defender"),
		_("Wanna get high ?"),
		_("The Owl"),
		_("Nick Kyriazis"),
		_("Linux rocks !"),
		_("Respect my authoritah"),
		_("Is it cause I is black ?"),
		_("Who am I ?"),
	};
	assert(SIZEOF_ARRAY(captions) == s_imageList->GetImageCount());
	return captions;
}

//**********************************************************************

wxBitmap ContactsImageList::GetImage (int index)
{
	assert(s_imageList->GetImageCount());
	index = util::clamp(index, 0, s_imageList->GetImageCount() - 1);
	return s_imageList->GetBitmap(index);
}

//**********************************************************************

wxImageList* ContactsImageList::s_imageList = 0;

////////////////////////////////////////////////////////////////////////

} // namespace talkie
