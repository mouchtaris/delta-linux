/**
 *	ZoomParameters.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifndef ZOOM_PARAMETERS_H
#define ZOOM_PARAMETERS_H

#include <wx/scrolwin.h>

namespace iviews {
	
	////////////////////////////////////////////////////////////////////////
	
	class ZoomParameters {

		private:
		int					initialVirtualWidth;		// 100 %
		int					initialVirtualHeight;		// 100 %
		int					zoomFactor;
		double				scaleFactor, previousScaleFactor;
		wxPoint				focalPoint;
		wxScrolledWindow*	canvas;
		bool				isInitialised;

		void		ComputeScaleFactor (void);

		/////////////////////////////////////////////////////////////////////////
		public:

		void		Initialise (void);
		void		CleanUp (void);

		bool		CanZoomIn (void) const;
		bool		CanZoomOut (void) const;
		void 		ZoomIn (void);
		void 		ZoomOut (void);
		void		ZoomFit (void);
		void 		ZoomRestore (void);
		void		CalculateViewParameters (
						int* viewX,
						int* viewY,
						int* virtualW,
						int* virtualH
					);
		
		int			GetZoomFactor (void) const;
		void		SetZoomFactor (const int factor);

		double		GetScaleFactor (void) const;
		void		SetScaleFactor (const double factor);

		double		GetPreviousScaleFactor (void) const;
		void		SetPreviousScaleFactor (const double factor);
		
		int			GetInitialVirtualWidth (void) const;
		void		SetInitialVirtualWidth (const int width);
		
		int			GetInitialVirtualHeight (void) const;
		void		SetInitialVirtualHeight (const int height);
		wxString	ToString (void) const;

		const wxPoint&
					GetFocalPoint (void) const 
						{ return focalPoint; }
		void		SetFocalPoint (const wxPoint& pt) 
						{ focalPoint = pt; }

		wxScrolledWindow*	
					GetCanvas (void)
						{ return canvas; }

		ZoomParameters (wxScrolledWindow* _canvas);
		~ZoomParameters (){}
	};
		
	////////////////////////////////////////////////////////////////////////

}	//namespace iviews

#endif	//ZOOM_PARAMETERS_H

