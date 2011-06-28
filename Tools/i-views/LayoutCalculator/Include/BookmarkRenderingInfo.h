#ifndef	BOOKMARK_RENDERING_INFO_H
#define BOOKMARK_RENDERING_INFO_H

#include <utility>
#include "Text.h"
#include "Rectangle.h"
#include "CommonTypes.h"
#include "RenderingTypes.h"

namespace iviews {
	
class BookmarkRenderingInfo {
public:
	typedef geo::Point<coord_t> CoordTPoint;

	~BookmarkRenderingInfo ();
	BookmarkRenderingInfo (
		const id_t	& id,
		Text		* text,
		Rectangle	* rect
	);

	//Update all contetns' pos.
	void				SetPosition (const coord_t & x, const coord_t & y);
	void				SetPosition (const CoordTPoint & newPos);

	const CoordTPoint &	GetPosition (void) const;

	length_t			GetWidth (void) const;
	length_t			GetHeight (void) const;
	coord_t				GetX (void) const;
	coord_t				GetY (void) const;

	Text *				GetText (void) const;
	Rectangle *			GetRectangle (void) const;
	std::pair<id_t, id_t>	
						GetPrimitivesIds (void) const;

private:
	id_t		  id;	
	Text		* text;
	Rectangle	* rect;
};

}	//namespace iviews

#endif	//BOOKMARK_RENDERING_INFO_H