
#include <assert.h>

#include <wx/image.h>
#include <wx/window.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/dcmemory.h>


#include "ImageSaver.h"


namespace iviews {

//-----------------------------------------------------------------------

ImageSaver::ImageSaver(const wxFont & font_) :
	font			(font_),
	hasError		(false),
	renderingApi	(),
	renderer		(&renderingApi)
{}

//-----------------------------------------------------------------------

void ImageSaver::Init (void) {
	hasError		= false;
	errorMessage	= wxT("");
}

//-----------------------------------------------------------------------

bool ImageSaver::HasError (void) const
	{ return hasError; }

//-----------------------------------------------------------------------

void ImageSaver::SetErrorMessage (const wxString & message) { 
	hasError		= true;
	errorMessage	= message; 
}

//-----------------------------------------------------------------------

const wxString&	ImageSaver::GetErrorMessage (void) const
	{ return errorMessage; }

//-----------------------------------------------------------------------

void ImageSaver::Save (const Rectangle & bb, const PrimitivePtrList & primitives) {
	Init();

	wxString fileName = SelectFile();
	
	if (fileName.IsEmpty())
		return;

	wxBitmap * bitmap = new wxBitmap(bb.GetWidth(), bb.GetHeight());
	DrawBitmap(*bitmap, primitives);
	
	if (!HasError() && SaveBitmap(*bitmap, fileName))
		wxMessageBox(wxT("Save graph as image succeeded"), wxT(""), wxICON_INFORMATION);
	else
		wxMessageBox(GetErrorMessage(), wxT("Error"), wxICON_ERROR);
	delete bitmap;	
}

//-----------------------------------------------------------------------

void ImageSaver::SetFont (const wxFont & font_)
	{ font = font_; }

//-----------------------------------------------------------------------

bool ImageSaver::SaveBitmap (wxBitmap & bitmap, const wxString & fileName) {
	assert(!fileName.IsEmpty() && bitmap.IsOk());

	if (!bitmap.ConvertToImage().SaveFile(fileName)) {
		SetErrorMessage(wxT("Problem with save"));	
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------

void ImageSaver::DrawBitmap (wxBitmap & bitmap, const PrimitivePtrList & primitives) {
	if (bitmap.IsOk()) {
		wxMemoryDC dc;
		if (dc.IsOk()){
			dc.SelectObject(bitmap);
			dc.Clear();
			dc.SetBackground(*wxMEDIUM_GREY_BRUSH);
			dc.SetFont(font);
			renderingApi.SetWxDC(&dc);
			renderer(primitives);
			dc.SelectObject(wxNullBitmap);
		}
		else
			SetErrorMessage(wxT("Problem with MemoryDC."));
	}
	else
		SetErrorMessage(wxT("Problem with bitmap. Maybe is too big"));
}

//-----------------------------------------------------------------------

wxString ImageSaver::SelectFile (void) const {
    return wxFileSelector( 
				wxT("Save Image"),
				wxEmptyString,
				wxEmptyString,
				(const wxChar *)NULL,
				wxT("BMP files (*.bmp)|*.bmp|")
				wxT("PNG files (*.png)|*.png|")
				wxT("JPEG files (*.jpg)|*.jpg"),
				wxFD_SAVE,
				wxGetActiveWindow()
			);
}
	

}	//namespace iviews 