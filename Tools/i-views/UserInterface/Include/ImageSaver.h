/**
 *	ImageSaver.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	November 2010
 */

#ifndef	IMAGE_SAVER_H
#define	IMAGE_SAVER_H

#include <wx/font.h>
#include <wx/bitmap.h>
#include <wx/string.h>


#include "Primitive.h"
#include "Rectangle.h"
#include "WxRenderingAPI.h"
#include "LayredGraphRenderer.h"

namespace iviews {

class ImageSaver {
public:
	typedef Primitive::PrimitivePtrList PrimitivePtrList;
	~ImageSaver(){}
	ImageSaver(const wxFont & font);

	void				Save (const Rectangle & bb, const PrimitivePtrList & primitives);
	void				SetFont (const wxFont & font);
	bool				HasError (void) const;
	const wxString&		GetErrorMessage (void) const;


private:
	void				Init (void);
	void				DrawBitmap (wxBitmap & bitmap, const PrimitivePtrList & primitives);
	void				SetErrorMessage (const wxString & message);
	bool				SaveBitmap (wxBitmap & bitmap, const wxString & fileName);
	wxString			SelectFile (void) const;


	wxFont				font;
	bool				hasError;
	wxString			errorMessage;
	WxRenderingAPI		renderingApi;
	LayredGraphRenderer renderer;

};



}	//namespace iviews

#endif	//IMAGE_SAVER_H
