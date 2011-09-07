/**
 *	BookmarkManager.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Sempteber 2010
 */

#ifndef BOOKMARK_MANAGER_H
#define BOOKMARK_MANAGER_H

#include <vector>
#include <utility>

#include "Point.h"
#include "Vertex.h"
#include "Bookmark.h"
#include "Dimension.h"
#include "TypesConverter.h"
#include "LayoutCalculator.h"
#include "VertexRenderingInfo.h"
#include "LayersRenderingInfo.h"

namespace iviews {

class IviewsCanvas;

class BookmarkManager {
public:
	typedef graphs::Vertex					Vertex;
	typedef graphs::Vertex::VerticesPtrList	VerticesPtrList;


	BookmarkManager(
		IviewsCanvas*		owner, 
		LayoutCalculator*	calculator				
	);						

	void				Initialise (void);
	void				CleanUp (void);

	bool 				HasBookmarks (void);							
	void 				EnableBookmarks (void);							
	void 				DisableAllBookmarks( void);						
	void 				ClearBookmarks (LayersRenderingInfo * layers);	

	void 				ToggleBookmark (VertexRenderingInfo * v);
	void 				UpdateBookmarks (LayersRenderingInfo * layers);	

	
	
	bool 				NextBookmark (void);
	bool 				PreviousBookmark (void);


private:
	typedef geo::Point<int>								
						IntPoint;
	typedef geo::Dimension<int>							
						IntDimension;
	typedef std::vector<
			std::pair<
				Vertex *, 
				Bookmark
			>
		>				BookmarksVec;


	void				GotoBookmark (const Bookmark & b);
	void				AddBookmark (VertexRenderingInfo * v);
	void				RemoveBookmark (VertexRenderingInfo * v);
	void				RefreshRectangle (const Rectangle* rect);
	int					RoundingUp (const double num);


	bool				lockBookmarks;
	size_t				bookmarkIndex;
	BookmarksVec		bookmarks;
	IviewsCanvas	 *	owner;
	LayoutCalculator *	calculator;
	
};


}	//namespace iviews 

#endif	//BOOKMARK_MANAGER_H