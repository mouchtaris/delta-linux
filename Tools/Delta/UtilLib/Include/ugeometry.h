// ugeometry.h
// Geomety specific utilities.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UGEOMETRY_H
#define	UGEOMETRY_H

#include "utypes.h"
#include <math.h>

//---------------------------------------------------------------
// Quickly decides if (x,y) (x+w-1, y+h-1) is outside
// (wx, wy) (wx+ww-1, wy+wh-1); i.e. the windows do
// not overlap to each other. The win versions assume y
// increases towards bottom, the geometric assume y decreases
// towards bottom
//
#define urectoutsidewin(x, y, w, h, wx, wy, ww, wh)				\
	(	(x)			>	((wx)+(ww)-1)	|| 						\
		((x)+(w)-1)	<	(wx)			|| 						\
		(y)			>	((wy)+(wh)-1)	|| 						\
		((y)+(h)-1)	<	(wy)	)

#define urectoutsidegeometric(x, y, w, h, wx, wy, ww, wh)		\
	(	(x)			>	((wx)+(ww)-1)	|| 						\
		((x)+(w)-1)	<	(wx)			|| 						\
		(y)			<	((wy)-(wh)+1)	|| 						\
		((y)-(h)+1)	>	(wy)	)

#define	urectsoverlapping(x, y, w, h, wx, wy, ww, wh)			\
	!urectoutsidewin(x, y, w, h, wx, wy, ww, wh)

#define	urectsoverlappinggeometric(x, y, w, h, wx, wy, ww, wh)	\
	!urectoutsidegeometric(x, y, w, h, wx, wy, ww, wh)

// Same version with rectangle coordinates.

#define urectoutsidewinex(x1, y1, x2, y2, wx1, wy1, wx2, wy2)	\
	(	(x1)	>	(wx2)	||									\
		(x2)	<	(wx1)	||									\
		(y1)	>	(wy2)	||									\
		(y2)	<	(wy1)	)

#define	urectsoverlappingex(x1, y1, x2, y2, wx1, wy1, wx2, wy2) \
	!urectoutsidewinex(x1, y1, x2, y2, wx1, wy1, wx2, wy2)

#define urectoutsidegeometricex(x1, y1, x2, y2, wx1, wy1, wx2, wy2)	\
	(	(x1)	>	(wx2)	||									\
		(x2)	<	(wx1)	||									\
		(y1)	<	(wy2)	||									\
		(y2)	>	(wy1)	)

#define	urectsoverlappinggeometricex(x1, y1, x2, y2, wx1, wy1, wx2, wy2) \
	!urectoutsidegeometricex(x1, y1, x2, y2, wx1, wy1, wx2, wy2)

/////////////////////////////////////////////////////////////////

#define upointinsiderect(px, py, x, y, w, h)		\
	((px) >= (x) && (px) < ((x)+(w)) && (py) >= (y) && (py) < ((y)+(h)))

#define upointinsiderectex(px, py, x1, y1, x2, y2) \
	((px) >= (x1) && (px) <= (x2) && (py) >= (y1) && (py) <= (y2))

/////////////////////////////////////////////////////////////////

#define	uvalidrect(x, y, w, h)			((w) && (h) && (x) < (w) && (y) < (h))
#define	uvalidrectex(x1, y1, x2, y2)	((x1) < (x2) && (y1) < (y2))

/////////////////////////////////////////////////////////////////
// umin (requires same types), umin1 (different types, but returns first
// type), and umin2 (same as umin1, for second type).
//
template <class T>
T umin (T x, T y) {
	return x < y ? x : y;
}

template <class T1, class T2>
util_i32 udiff (T1 x, T2 y) {
	return ((util_i32)x) -((util_i32) y);
}

template <class T1, class T2>
T1 umin1 (T1 x, T2 y) {
	if (x < y)
		return x;
	else
		return (T1) y;
}

template <class T1, class T2>
T2 umin2 (T1 x, T2 y) {
	if (x < y)
		return (T2) x;
	else
		return y;
}

template <class T>
void uminupdate (const T& val, T& minVal) {
    
	if (val < minVal)
		minVal = val;
}

template <class T>
void umaxupdate (const T& val, T& maxVal) {
    
	if (val > maxVal)
		maxVal = val;
}

template <class T>
void uminmaxupdate (const T& val, T& minVal, T& maxVal) {
    
	uminupdate(val, minVal);
	umaxupdate(val, maxVal);
}

/////////////////////////////////////////////////////////////////
// Same as umin set of functions.
//
template <class T>
T umax (T x, T y) {
	return x > y ? x : y;
}

template <class T1, class T2>
T1 umax1 (T1 x, T2 y) {
	if (x > y)
		return x;
	else
		return (T1) y;
}

template <class T1, class T2>
T2 umax2 (T1 x, T2 y) {
	if (x > y)
		return (T2) x;
	else
		return y;
}

/////////////////////////////////////////////////////////////////
// A very simple rectangle template class.

template <class T> struct urectangleex;

template <class T> struct urectangle {

	T x, y, w, h;

	void operator=(const urectangle& r)						{ if (this != &r) new (this) urectangle(r); }
	void operator=(const urectangleex<T>& r)				{ new (this) urectangle(r); }
	bool operator==(const urectangle& r) const				{ return x==r.x && y==r.y && w==r.w && h==r.h; }
	bool operator==(const urectangleex<T>& r) const			{ return x==r.x1 && y==r.y1 && w==(r.x2-r.x1+1) && h==(r.y2-r.y1+1); }
	void mul (T xf, T yf)									{ x *= xf; y *= yf; w *= xf; h *= yf; }
	void add (T xf, T yf)									{ x += xf; y += yf; w += xf; h += yf; }
	bool isleftto (T xp) const								{ return x + w <= xp; }
	bool isrightto (T xp) const								{ return x > xp; }
	bool isupto (T yp) const								{ return y + h <= yp; }
	bool isdownto (T yp) const								{ return y > yp; }
	bool in (T xp, T yp) const								{ return upointinsiderect(xp, yp, x, y, w, h); } 
	bool isvalid (void) const								{ return uvalidrectex(x, y, (T) (x + w - 1), (T) (y + h - 1)); }
	bool isoverlapping (const urectangle& r) const			{ return urectsoverlapping(x,y,w,h,r.x,r.y,r.w,r.h); }
	bool isoverlappinggeometric (const urectangle& r) const	{ return urectsoverlappinggeometric(x,y,w,h,r.x,r.y,r.w,r.h); }
	void mergeunion (const urectangle& r, urectangle* at) const {
														at->x = umin(x, r.x);
														at->y = umin(y, r.y);
														at->w = umax(x + w, r.x + r.w) - at->x;
														at->h = umax(y + h, r.y + r.h) - at->y;
													}
	void mergeunion (const urectangle& r) {				T _x = x, _y = y;
														x = umin(x, r.x);
														y = umin(y, r.y);
														w = umax(_x + w, r.x + r.w) - x;
														h = umax(_y + h, r.y + r.h) - y;
													}

	urectangle (void) : x(0), y(0), w(0), h(0) {}
	urectangle (T _x, T _y, T _w, T _h) : x(_x), y(_y), w(_w), h(_h) {}
	urectangle (const urectangle& r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
	urectangle (const urectangleex<T>& r) : x(r.x1), y(r.y1), w(r.x2 - r.x1 + 1), h(r.y2 - r.y1 + 1) {}
};
	
template <class T> struct urectangleex {

	T x1, y1, x2, y2;

	void operator=(const urectangleex& r)						{ if (this != &r) new (this) urectangleex(r); }
	void operator=(const urectangle<T>& r)						{ new (this) urectangleex(r); }
	bool operator==(const urectangleex& r) const				{ return x1==r.x1 && y1==r.y1 && x2==r.x2 && y2==r.y2; }
	bool operator==(const urectangle<T>& r) const				{ return r.x==x1 && r.y==y1 && r.w==(x2-x1+1) && r.h==(y2-y1+1); }
	void mul (T xf, T yf)										{ x1 *= xf; y1 *= yf; x2 *= xf; y2 *= yf; }
	void add (T xf, T yf)										{ x1 += xf; y1 += yf; x2 += xf; y2 += yf; }
	bool isleftto (T x) const									{ return x2 < x; }
	bool isrightto (T x) const									{ return x1 > x; }
	bool isupto (T y) const										{ return y2 < y; }
	bool isdownto (T y) const									{ return y1 > y; }
	bool in (T x, T y) const									{ return upointinsiderectex(x, y, x1, y1, x2, y2); }
	bool isvalid (void) const									{ return uvalidrectex(x1, y1, x2, y2); }
	bool isoverlapping (const urectangleex& r) const			{ return urectsoverlappingex(x1, y1, x2, y2, r.x1, r.y1, r.x2, r.y2); }
	bool isoverlappinggeometric (const urectangleex& r) const	{ return urectsoverlappinggeometricex(x1, y1, x2, y2, r.x1, r.y1, r.x2, r.y2); }

	urectangleex (void) : x1(0), y1(0), x2(0), y2(0) {}
	urectangleex (T _x1, T _y1, T _x2, T _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
	urectangleex (const urectangle<T>& r) : x1(r.x), y1(r.y), x2(r.x + r.w - 1), y2(r.y + r.h - 1) {}
	urectangleex (const urectangleex& r) : x1(r.x1), y1(r.y1), x2(r.x2), y2(r.y2) {}
};
	
/////////////////////////////////////////////////////////////////

template <typename T> class upoint {
	private:
	T x, y;

	public:
	T		getx (void) const 
				{ return x; }
	T		gety (void) const 
				{ return y; }

	template <typename C>
	void	setx (C _x) 
				{ x = (T)  _x; }

	template <typename C>
	void	sety (C _y)			
				{ y = (T) _y; }

	template <typename C>
	void	move (C dx, C dy)	
				{ setx(((C) x) + dx); sety(((C) y) + dy); }
	
	void operator=(const upoint& p)
				{ new (this) upoint(p); }

	template <typename C>
	upoint (C _x , C _y) : x((T) _x), y((T) _y) {}

	upoint (const upoint& p) : x(p.x), y(p.y){}
	upoint (void) : x(0), y(0) {}
};

/////////////////////////////////////////////////////////////////

inline bool uispower2 (util_ui32 num) {
	for (util_ui32 i = 1; i <= num; i <<= 1)
		if ((num & ~i) == 0)
			return true;
	return false;
}

inline util_ui32 ugetmaxpower2 (util_ui32 num) {	// strips off everything but the largest 2 power in num
	util_ui32 result = 1;
	while (num >>= 1)
		result <<= 1;
	return result;
}

/////////////////////////////////////////////////////////////////

inline double ufractionalpart (double val)
	{ return modf(val, &val); }

inline double uintegerpart (double val)
	{ return val - ufractionalpart(val); }

/////////////////////////////////////////////////////////////////

template <class T1, class T2> 
void uswap (T1& x, T2& y) {
	T1 temp = x;
	x = (T1) y;
	y = (T2) temp;
}

/////////////////////////////////////////////////////////////////

template <class T>
T usqr (T x) { 
	return x * x; 
}

/////////////////////////////////////////////////////////////////

template <class T>
T uabs (T x) { 
	return x < 0 ? -x : x; 
}

template <class T>
T usign (T x) { 
	return x < ((T) 0) ? ((T) -1) : ((T) +1); 
}

/////////////////////////////////////////////////////////////////
// Returns true if first rectangle is included within the second.
//
template <class T>
inline bool urectinclusion (
					T x1, T y1, T w1, T h1,
					T x2, T y2, T w2, T h2
				) {

	return	x1 >= x2			&&
			y1 >= y2			&&
			x1 + w1 <= x2 + w2	&&
			y1 + h1 <= y2 + h2;
}

template <class T>
inline bool urectinclusion (
					const urectangle<T>& r1, 
					const urectangle<T>& r2
				) {
	return urectinclusion(
				r1.x, r1.y, r1.w, r1.h,
				r2.x, r2.y, r2.w, r2.h
			);
}


/////////////////////////////////////////////////////////////////
// Clipping of (x,y) (x+w-1, y+h-1) against (wx, wy) (wx+ww-1, wy+wh-1)
// producing (cx, cy) (cx+cw-1, cy+ch-1). may need to type cast to a single
// type.
//
template <class T>
void ucliprect(
		T	x,
		T	y,
		T	w,
		T	h,

		T	wx,
		T	wy,
		T	ww,
		T	wh,

		T*	cx,
		T*	cy,
		T*	cw,
		T*	ch
	) {

	*cw = umin(wx + ww, x + w) - (*cx = umax(wx, x));
	*ch = umin(wy + wh, y + h) - (*cy = umax(wy, y));
}

//*****************************

template <class T> void ucliprect (
		urectangle<T>&	r,
		T				wx,
		T				wy,
		T				ww,
		T				wh
	) {
	r.w = umin(wx + ww, r.x + r.w) - (r.x = umax(wx, r.x));
	r.h = umin(wy + wh, r.y + r.h) - (r.y = umax(wy, r.y));
}

//*****************************

template <class T> void ucliprect (
		urectangle<T>&			r,
		const urectangle<T>&	area
	) {
	r.w = umin(area.x + area.w, r.x + r.w) - (r.x = umax(area.x, r.x));
	r.h = umin(area.y + area.h, r.y + r.h) - (r.y = umax(area.y, r.y));
}

//*****************************

template <class T> void ucliprect (
		urectangleex<T>&	r,
		T					wx,
		T					wy,
		T					ww,
		T					wh
	) {
	r.x1 = umax(wx, r.x1);
	r.x2 = umin(wx + ww - 1, r.x2);
	r.y1 = umax(wy, r.y1);
	r.y2 = umin(wy + wh - 1, r.y2);
}

//*****************************

template <class T> void ucliprect (
		urectangleex<T>&		r,
		const urectangleex<T>&	area
	) {
	r.x1 = umax(area.x1, r.x1);
	r.x2 = umin(area.x2, r.x2);
	r.y1 = umax(area.y1, r.y1);
	r.y2 = umin(area.y2, r.y2);
}

//*****************************

template <typename T>	
const T& utrim (T* val, const T& left, const T& right) {
	if (*val < left)
		return *val = left;
	if (*val > right)
		return *val = right;
	return *val;
}

/////////////////////////////////////////////////////////////////
// This function decides if a point is close to a rectangle, based 
// on a positive distance threshold for each axis. With 0 threshold,
// the function returns true if point lays inside a rectangle.
//
inline bool	upointclosetorect (
		util_ui32	px, 
		util_ui32	py, 

		util_ui32	rx, 
		util_ui32	ry, 
		util_ui32	rw,
		util_ui32	rh,

		util_ui32	threshold
	) {

	bool closeX =	px < rx ?		px + threshold >= rx :
					px >= rx+rw ?	rx + rw + threshold > px : true;

	if (!closeX) 
		return false;

	bool closeY =	py < ry ?		py + threshold >= ry :
					py >= ry+rh ?	ry + rh + threshold > py : true;

	return closeY;
}

/////////////////////////////////////////////////////////////////

// Rotates (x,y) around (rx,ry), put (0,0) for the origin, according to angle.
UTILLIB_FUNC void urotate (float* x, float* y, float angleInRadians, float rx, float ry);

// Scales (x,y) according to factor > 0, with respect to (sx,sy),  put (0,0) for the origin.
UTILLIB_FUNC void uscale (float* x, float* y, float factor, float sx, float sy);

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
