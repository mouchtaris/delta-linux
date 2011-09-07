/**
 *	Bookmark.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Sempteber 2010
 */

#ifndef BOOKMARK_H
#define BOOKMARK_H


#include <wx/gdicmn.h>

namespace iviews {
	

class Bookmark {
public:

	~Bookmark(){}
	Bookmark(void);
	Bookmark(
		const int		initialVirtualWidth_,
		const int		initialVirtualHeight_,
		const int		zoomFactor_,
		const double	scaleFactor_,
		const double	previousScaleFactor_,
		const wxPoint&	focalPoint_,
		const int		xStep_,
		const int		yStep_,
		const bool		isMaximized_,
		const wxSize&	windowSize_,
		const wxSize&	virtualSize_,
		const wxPoint&	scrolPos_,
		const wxPoint&	mouseLogicalPos_
	);

	int		GetInitialVirtualWidth (void) const;
	void	SetInitialVirtualWidth (const int width);

	int		GetInitialVirtualHeight (void) const;
	void	SetInitialVirtualHeight (const int height);

	int		GetZoomFactor (void) const;
	void	SetZoomFactor (const int factor);

	double	GetScaleFactor (void) const;
	void	SetScaleFactor (const double factor);

	double	GetPreviousScaleFactor (void) const;
	void	SetPreviousScaleFactor (const double factor);

	wxPoint	GetFocalPoint (void) const;
	void	setFocalPoint (const wxPoint & pt);

	int		GetXStep (void) const;
	void	SetXStep (const int step);

	int		GetYStep (void) const;
	void	SetYStep (const int step); 

	bool	IsMaximized (void) const;
	void	SetMaximized (const bool b);

	wxSize	GetWindowSize (void) const;
	void	SetWindowSize (const wxSize & size);
	
	wxSize	GetVirtualSize (void) const;
	void	SetVirtualSize (const wxSize & size);

	wxPoint	GetScrollPosition (void) const;
	void	SetScrollPosition (const wxPoint & pt);

	wxPoint	GetMouseLogicalPos (void) const;
	void	SetMouseLogicalPos (const wxPoint & pt);

	operator bool (void) const;
	static const Bookmark & 
			NullBookmark(void);

private:
	bool	nullOblect;
	
	////	About zoom
	int		initialVirtualWidth;
	int		initialVirtualHeight;
	int		zoomFactor;
	double	scaleFactor;
	double	previousScaleFactor;
	wxPoint	focalPoint;

	////	About window
	int		xStep;
	int		yStep;
	bool	isMaximized;
	wxSize	windowSize;
	wxSize	virtualSize;
	wxPoint	scrolPos;
	wxPoint	mouseLogicalPos;
	


};
}	//namespace iviews

#endif	//BOOKMARK_H