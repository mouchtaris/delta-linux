/**
 *	ZoomManager.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#ifndef ZOOM_MANAGER_H
#define ZOOM_MANAGER_H


#include <wx/scrolwin.h>
#include "LayoutParams.h"
#include "ZoomParameters.h"

namespace iviews {
		
	////////////////////////////////////////////////////////////////////////

	class ZoomManager {

		public:
		typedef LayerLayoutParams::LayerAlignment	LayerAlignment;

		ZoomManager (wxScrolledWindow* canvas, LayerAlignment alignment);
		~ZoomManager() {}
		
		void			Initialise (void);
		void			CleanUp (void);

		void 			ZoomIn (void);
		void 			ZoomOut (void);
		void			ZoomFit (void);
		void 			ZoomRestore (void);
		void			MouseMove (const wxPoint & currentPos);
		LayerAlignment	GetLayerAlignment (void);
		void			SetLayerAlignment (LayerAlignment newLayerAlignment);
		void			AdjustScrollSteps (void);
		
		ZoomParameters& GetZoomParams (void) 
							{ return zoomParams; }

		private:
		wxScrolledWindow*
						GetCanvas (void) 
							{ return zoomParams.GetCanvas(); }


		void			AdjustScrollSteps (int newWidth, int newHeight);
		void			ApplyCurrentZoom (const wxPoint& focus);

		bool			isInitialised;
		LayerAlignment	layerAlignment;
		ZoomParameters	zoomParams;
	};

	////////////////////////////////////////////////////////////////////////

}	//namespace iviews 

#endif //ZOOM_MANAGER_H
