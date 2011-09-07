/**
 *	BookmarkManager.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Sempteber 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/dcclient.h>

#include "Rectangle.h"
#include "IviewsCanvas.h"
#include "IviewsWindow.h"
#include "BookmarkManager.h"

namespace iviews {

//-----------------------------------------------------------------------

BookmarkManager::BookmarkManager(
	IviewsCanvas		* owner_, 
	LayoutCalculator	* calculator_
) :	owner(owner_),
	calculator(calculator_),
	lockBookmarks(false),
	bookmarkIndex(0),
	bookmarks()
{}

//-----------------------------------------------------------------------

void BookmarkManager::Initialise (void) {
	lockBookmarks = false;
	bookmarkIndex = 0;
}

//-----------------------------------------------------------------------

void BookmarkManager::CleanUp (void) {
	lockBookmarks = false;
	bookmarkIndex = 0;
	bookmarks.clear();
}

//-----------------------------------------------------------------------

void BookmarkManager::UpdateBookmarks(LayersRenderingInfo * layers) {
	BookmarksVec::iterator candidateForDeletion, i = bookmarks.begin();

	while(i != bookmarks.end()) 
		if (VertexRenderingInfo * v = layers->GetVertexRenderingInfo((candidateForDeletion = i++)->first))
			calculator->AddBookmark(v);
		else
			bookmarks.erase(candidateForDeletion);

	if (bookmarkIndex >= bookmarks.size())
		bookmarkIndex = 0;
}

//-----------------------------------------------------------------------

bool BookmarkManager::HasBookmarks(void) 
	{ return bookmarks.empty(); }

//-----------------------------------------------------------------------

void BookmarkManager::EnableBookmarks() 
	{ lockBookmarks = false; }

//-----------------------------------------------------------------------

void BookmarkManager::DisableAllBookmarks()
	{ lockBookmarks = true; }

//-----------------------------------------------------------------------

void BookmarkManager::ClearBookmarks(LayersRenderingInfo * layers) {
	assert(calculator && layers);
	int answer = wxMessageBox(
					_("Are you sure you want to delete all of the bookmak(s)?"), 
					_("Confirm message"), 
					wxYES_NO | wxCANCEL
				);
	
	if (answer == wxYES) {
		for (BookmarksVec::iterator i = bookmarks.begin(); i != bookmarks.end(); ++i)
			if (VertexRenderingInfo * v = layers->GetVertexRenderingInfo(i->first))
				calculator->RemoveBookmark(v);
		bookmarks.clear();
		owner->Refresh();
	}
}

//-----------------------------------------------------------------------

bool BookmarkManager::NextBookmark(void) {	
	if (lockBookmarks || !bookmarks.size())
		return false;

	assert(bookmarkIndex < bookmarks.size());
	GotoBookmark(bookmarks[bookmarkIndex].second);
	bookmarkIndex = ++bookmarkIndex % bookmarks.size();
	return true;
}

//-----------------------------------------------------------------------

bool BookmarkManager::PreviousBookmark(void) {	
	if (lockBookmarks || !bookmarks.size())
		return false;

	assert(bookmarkIndex < bookmarks.size());
	GotoBookmark(bookmarks[bookmarkIndex].second);
	if (!bookmarkIndex)
		bookmarkIndex = bookmarks.size() - 1;
	else
		bookmarkIndex = ++bookmarkIndex % bookmarks.size();
	return true;
}

//-----------------------------------------------------------------------

void BookmarkManager::ToggleBookmark(VertexRenderingInfo * v) {
	assert(calculator && v);
	v->HasBookmark() ? RemoveBookmark(v) : AddBookmark(v);
}

////////////////////////////////////////////////////
//	private methos API implementation

//-----------------------------------------------------------------------

void BookmarkManager::AddBookmark(VertexRenderingInfo * v) {
	const ZoomParameters & zoomParams = owner->GetZoomManager().GetZoomParams();
	
	int	xStep, yStep;
	owner->GetScrollPixelsPerUnit(&xStep, &yStep);
	
	wxPoint scollPos;
	owner->GetViewStart(&scollPos.x, &scollPos.y);

	wxPoint mouseLogicalPos = owner->ScreenToClient(wxGetMousePosition());

	bookmarks.push_back(
		std::make_pair(
			v->GetSourceVertex(), 
			Bookmark(
				zoomParams.GetInitialVirtualWidth(),
				zoomParams.GetInitialVirtualHeight(),
				zoomParams.GetZoomFactor(),
				zoomParams.GetScaleFactor(),
				zoomParams.GetPreviousScaleFactor(),
				zoomParams.GetFocalPoint(),
				xStep,
				yStep,
				owner->GetOwner()->IsMaximized(),
				owner->GetOwner()->GetSize(),
				owner->GetVirtualSize(),
				scollPos,
				mouseLogicalPos
			)
		)
	);
	RefreshRectangle(calculator->AddBookmark(v));
}

//-----------------------------------------------------------------------

void BookmarkManager::RemoveBookmark(VertexRenderingInfo * v) {
	assert(v && v->HasBookmark());
	bool break_ = false;

	for (BookmarksVec::iterator i = bookmarks.begin(); i != bookmarks.end(); ++i)
		if (i->first == v->GetSourceVertex()) {
			bookmarks.erase(i);
			break_ = true;
			if (bookmarkIndex > 0)
				--bookmarkIndex;
			break;
		}
	assert(break_);
	Rectangle rect(*v->GetBookmark()->GetRectangle());
	calculator->RemoveBookmark(v);
	RefreshRectangle(&rect);
}

//-----------------------------------------------------------------------

int BookmarkManager::RoundingUp(const double num) {
	double intpart;
	if (modf(num, &intpart) > 0.0)
		intpart += 1;
	
	return intpart;	
}

//-----------------------------------------------------------------------

void BookmarkManager::RefreshRectangle(const Rectangle* rect) {
	if (rect) {
		owner->Refresh();
		//double scaleFactor = owner->GetZoomManager().GetZoomParams().GetScaleFactor();
		//int viewX, viewY;
		//owner->CalcScrolledPosition(rect->GetX(), rect->GetY(), &viewX, &viewY);

		//owner->RefreshRect(
		//	wxRect(
		//		viewX * scaleFactor,
		//		viewY * scaleFactor,
		//		RoundingUp(rect->GetWidth() * scaleFactor),
		//		RoundingUp(rect->GetHeight()* scaleFactor)
		//	),
		//	false
		//);
	}
}

//-----------------------------------------------------------------------

void BookmarkManager::GotoBookmark(const Bookmark & b) {
	if (b.IsMaximized() && !owner->GetOwner()->IsMaximized())
		owner->GetOwner()->Maximize(true);

	else if (!b.IsMaximized()) {
		wxSize curSize = owner->GetOwner()->GetSize();
		wxSize oldSize = b.GetWindowSize();
		
		if (curSize.GetWidth() < oldSize.GetWidth() || curSize.GetHeight() < oldSize.GetHeight())
			owner->GetOwner()->SetSize(oldSize);	
	}

	ZoomParameters & zoomParams = owner->GetZoomManager().GetZoomParams();

	zoomParams.SetInitialVirtualWidth(b.GetInitialVirtualWidth());
	zoomParams.SetInitialVirtualHeight(b.GetInitialVirtualHeight());
	zoomParams.SetZoomFactor(b.GetZoomFactor());
	zoomParams.SetScaleFactor(b.GetScaleFactor());
	zoomParams.SetPreviousScaleFactor(b.GetPreviousScaleFactor());
	zoomParams.SetFocalPoint(b.GetFocalPoint());
	
	owner->Freeze();
	owner->SetVirtualSize(b.GetVirtualSize());
	owner->SetScrollRate(b.GetXStep(), b.GetYStep());
	owner->Scroll(b.GetScrollPosition().x, b.GetScrollPosition().y);
	owner->WarpPointer(b.GetMouseLogicalPos().x, b.GetMouseLogicalPos().y);
	owner->Thaw();									
}

}	//namespace iviews 