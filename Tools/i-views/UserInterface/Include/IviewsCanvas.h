//july 2010
#ifndef IVIEWS_CANVAS_H
#define IVIEWS_CANVAS_H

#include <string>

#include <wx/dc.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>

#include "HandTool.h"
#include "Rectangle.h"
#include "Primitive.h"
#include "FindAddOn.h"
#include "ImageSaver.h"
#include "EdgeHiding.h"
#include "ZoomManager.h"
#include "SlotTooltip.h"
#include "LayoutParams.h"
#include "GraphExporter.h"
#include "LensViewWindow.h"
#include "WxRenderingAPI.h"
#include "RenderingParams.h"
#include "BookmarkManager.h"
#include "ContentsTooltip.h"
#include "LayoutCalculator.h"
#include "GraphCoordinator.h"
#include "IViewsFindDialog.h"
#include "PathHighlighting.h"
#include "ColourAddOnManager.h"
#include "LayredGraphRenderer.h"
#include "HighlightRelativeSlots.h"
#include "HighlightCurrentVertex.h"
#include "WxGraphicsLayoutPrimitives.h"
#include "CopyAbsoluteReferenceOption.h"
#include "IViewsApplicationCommunicator.h"
#include "IViewsTextSourceCollectionProducer.h"

namespace iviews {
	class IviewsWindow;

	class IviewsCanvas : public wxScrolledWindow {
	public:
		typedef Primitive::PrimitivePtrList		PrimitivePtrList;
			


		~IviewsCanvas ();
		IviewsCanvas (
			IviewsWindow			* parent, 
			const wxFont			& font, 
			const LayoutParams		& lparams,  
			const RenderingParams	& rparams
		);

		void					StartVerticesFromCommonPosition (bool b);
		void					StopAllTooltips (void);
		void					DrawGraph (const wxString & expr, const long depth);		
		void 					UpdateParams (			
									const wxFont			& font, 
									const LayoutParams		& lparams,  
									const RenderingParams	& rparams
								);
		ZoomManager &			GetZoomManager (void);
		IviewsWindow		*	GetOwner (void) const;
		Rectangle				GetBoundingBox (void) const;
		LayersRenderingInfo *	GetLayersRenderingInfo (void) const;
		VertexRenderingInfo *	GetLastVisitedVertex (void) const;


		////////// Event functions  //////////
		void OnErase (wxEraseEvent& event);
		void OnCancel (wxKeyEvent& event);
		void OnPaint (wxPaintEvent &event);
		/////////////////////////////////////


		/////////////////// Mouse ///////////////////
		void OnMouseMove (wxMouseEvent &event);
		void OnMouseWheel (wxMouseEvent & event);
		void OnMouseLeftUp (wxMouseEvent & event);
		void OnMouseLeftDown (wxMouseEvent & event);
		void OnMouseRightDown (wxMouseEvent & event);
		/////////////////////////////////////////////


		/////////////// Undo - Redo ///////////////
		void Undo (void);
		void Redo (void);
		///////////////////////////////////


		/////////////// Load - Save ///////////////
		void SaveGraph (const wxString & path);
		void LoadGraph (const wxString & path);
		void LoadConfigurations (const wxString & path);
		void SaveGraphAsImage (void);
		///////////////////////////////////////////


		/////////////// Zoom ///////////////
		void ApplyZoomOperation (void (ZoomManager::*op)(void));
		void ZoomIn (void);
		void ZoomOut (void);
		void ZoomRestore (void);
		void ZoomFit (void);

		////////////////////////////////////
		
	
		/////////////// Find ///////////////
		void Find (void);
		void FindNext (void);
		void FindPrev (void);
		///////////////////////////////////


		/////////////// Bookmarks ///////////////
		void ToggleBookmark (void);
		void DisableAllBookmarks (void);
		void EnableBookmarks (void);
		void GotoPreviousBookmark (void);
		void GotoNextBookmark (void);
		void ClearBookmarks (void);
		/////////////////////////////////////////
	

		/////////////// Features ///////////////
		void HideOrShowVertexEdges (const EdgeHiding::EdgeHidingPreferences & prefrence);
		void HideOrShowLayerEdges (const EdgeHiding::EdgeHidingPreferences & prefrence);
		void HideOrShowLayersEdges (const EdgeHiding::EdgeHidingPreferences & prefrence);

		void ExportGraphTo (GraphExporter::ExportedFileFormats format);
		void EnableLensView (void);
		void ShowLastVisitedVertexContents (void);
		void HideLastVisitedVertexContents (void);
		void CancelAllDisplayAddOns (void);
		void HighlightAllRelativeSlots (void);
		void RestoreRelativeSlotsColours (void);
		void RestorePathColours (void);
		void RemoveLastVisitedVertex (void);
		void CopyAbsRefToClipboard (void);
		bool GotoSourceFile (void);
		void ViewSlot (const wxPoint & devicePos);
		void HighlightPath (
				HighlightingDirection direction,
				bool highlightInvolvedSlots
			);
		/////////////////////////////////////////

		virtual void Refresh (bool erase_bg = true, const wxRect* rect = NULL);
		virtual void RefreshRect (const wxRect& rect, bool eraseBackground = true);	
	private:
		////////////////////////////////
		//	Private method API
		void DisableMenuAndTools (void);
		void RefreshRectangle (Rectangle * rect);
		void InitialiseManagers (void);
		void CleanUpManagers (bool cleanBookmarkManage);
		void ApplyParams (void);
		void MyPrepareDC (wxDC & dc);
		void GetLogicalPos (coord_t* x, coord_t* y, const wxPoint & pt);

		void DisplayLens (void);
		void MoveLens (int logicalMouseX, int logicalMouseY, const wxPoint& deviceMousePos, bool display);
		void ZoomLens (bool zoomIn, const wxPoint& devicePos);
		bool IsUsingLens (void) const;
		void HideLens (void);
		void ShowLens (const wxPoint& devicePos);

		void MoveHandTool (const wxPoint& mousePos, bool leftMouseButtonIsDown);
		void OnMouseMoveHighlightCurrentVertex (coord_t lx, coord_t ly);
		void Clean (void);
		void CleanRenderingData (void);
		

		////////////////////////////////
		//	Private members
		//bool								commonBeginningEdges;
		ImageSaver							imageSaver;
		bool								canDraw;
		graphs::GraphCoordinator			graphCoordinator;
		LayredGraphRenderer					renderer;
		LayoutCalculator					layoutCalculator;
		GraphExporter						exporter;
		IviewsWindow *						owner;
		LayersRenderingInfo *				renderingInfo;
		VertexRenderingInfo *				lastVisitedVertex;
		Rectangle *							rectOflastVisitedVertex;
			
		//// params
		wxFont								font;
		WxRenderingAPI						renderingApi;
		LayoutParams						layoutParams;
		RenderingParams						renderingParams;
		WxGraphicsLayoutPrimitives			wxGraphicsLayoutPrimitives;

		//// tools
		LensViewWindow						lensView;
		HandTool							handTool;

		//// managers
		ZoomManager							zoomManager;
		BookmarkManager						bookmarkManager;
		ColourAddOnManager					colourAddOnManager;

		///// tooltips
		SlotTooltip							slotTooltip;
		ContentsTooltip						contentsTooltip;

		//find
		FindAddOn							findAddOn;
		IViewsFindDialog *					findDialog;
		IViewsApplicationCommunicator		findApplicationCommunicator;
		IViewsTextSourceCollectionProducer	findSourceCollectionProducer;

		//// Features
		EdgeHiding							edgeHiding;
		PathHighlighting					pathHighlighting;
		HighlightRelativeSlots				relativeSlotsHighlighter;
		HighlightCurrentVertex				currentVertexHighlighter;
		CopyAbsoluteReferenceOption			copyToClipboard;
		bool								isExplicitRefresh;
		
		
		DECLARE_EVENT_TABLE ()
	};

}	//namespace iviews






#endif //IVIEWS_CANVAS_H