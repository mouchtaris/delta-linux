/**
 *	IviewsCanvas.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <math.h>

#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/dcbuffer.h>
#include <wx/font.h>
#include <wx/event.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/bitmap.h>
#include <wx/cursor.h>
#include <wx/utils.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>


#include "Colour.h"
#include "SlotFinder.h"
#include "GraphLayers.h"
#include "VertexFinder.h"
#include "IviewsCanvas.h"
#include "IviewsWindow.h"
#include "TypesConverter.h"
#include "GotoSourcePoint.h"
#include "GraphToXml.h"
#include "XmlToGraph.h"
#include "LayoutParamsValidator.h"
#include "ViewSlotTextualContents.h"
#include "XMLConfigurationLoader.h"

namespace iviews {

//-----------------------------------------------------------------------

#define SLOT_TOOLTIP()				\
	slotTooltip(					\
		this,						\
		lastVisitedVertex,			\
		geo::Point<coord_t>(x, y)	\
	)

//-----------------------------------------------------------------------

#define COONTENT_TOOLTIP()								\
	contentsTooltip.MouseMove(							\
		lastVisitedVertex,								\
		graphCoordinator.GetVertexToDebugInfoMapping(), \
		geo::Point<coord_t>(x, y),						\
		zoomManager.GetZoomParams().GetZoomFactor()		\
	)

//--------calculation---------------------------------------------------------------

#define PREPARE_FOR_DRAW(DO_CALCULATION, GRAPH_LAYERS, COLOUR_ADD_ON_MANAGER)						\
	if (true) {																						\
		wxBusyInfo wait(wxT("Please wait, graph is being calculated"), this->GetParent());			\
		if (DO_CALCULATION)																			\
			renderingInfo = layoutCalculator.Calculate(												\
								GRAPH_LAYERS,														\
								graphCoordinator.GetVertexToDebugInfoMapping()						\
							);																		\
		assert(renderingInfo);																		\
		findSourceCollectionProducer.SetGraphLayers(GRAPH_LAYERS);									\
		findSourceCollectionProducer.SetVertexToDebugInfoMapping(									\
			graphCoordinator.GetVertexToDebugInfoMapping()											\
		);																							\
		findSourceCollectionProducer.SetLayersRenderingInfo(renderingInfo);							\
		findDialog->SetNewSearch();																	\
		COLOUR_ADD_ON_MANAGER;																		\
		Rectangle bb = renderingInfo->CalcBoundingBox(layoutParams.layerParams.layersDistance);		\
		SetVirtualSize(bb.GetWidth(), bb.GetHeight());												\
		InitialiseManagers();																		\
		lensView.SetPrimitiveList(layoutCalculator.GetPrimitivesList());							\
		owner->EnableMenus();																		\
		owner->EnableTools();																		\
		if (layoutParams.layerParams.drawVertexWithFullContents)									\
			owner->EnableStartVerticesFromCommonPosition(true);										\
		else																						\
			owner->EnableStartVerticesFromCommonPosition(false);									\
		canDraw = true;																				\
	} else	

//-----------------------------------------------------------------------

BEGIN_EVENT_TABLE(IviewsCanvas, wxScrolledWindow)
	EVT_CHAR				(IviewsCanvas::OnCancel)
	EVT_PAINT				(IviewsCanvas::OnPaint)  
    EVT_MOTION				(IviewsCanvas::OnMouseMove)
	EVT_LEFT_UP				(IviewsCanvas::OnMouseLeftUp)
	EVT_LEFT_DOWN			(IviewsCanvas::OnMouseLeftDown)
	EVT_RIGHT_DOWN			(IviewsCanvas::OnMouseRightDown)
	EVT_MOUSEWHEEL			(IviewsCanvas::OnMouseWheel)
	EVT_ERASE_BACKGROUND	(IviewsCanvas::OnErase)
END_EVENT_TABLE()


//-----------------------------------------------------------------------

IviewsCanvas::IviewsCanvas (
	IviewsWindow			* parent, 
	const wxFont			& font_, 
	const LayoutParams		& lparams,  
	const RenderingParams	& rparams
) :	wxScrolledWindow( 
		(wxWindow*)parent, 
		wxID_ANY, 
		wxDefaultPosition, 
		wxDefaultSize,
        wxHSCROLL | wxVSCROLL | wxRETAINED
	),
	owner						(parent),
	font						(font_),
	wxGraphicsLayoutPrimitives	(this, font),
	layoutParams				(lparams),
	renderingParams				(rparams),
	layoutCalculator			(&layoutParams, &renderingParams),
	renderingApi				(),
	renderer					(&renderingApi),
	lensView					(parent, font_),
	imageSaver					(font_),
	canDraw						(false),
	relativeSlotsHighlighter	(),
	currentVertexHighlighter	(),
	pathHighlighting			(),
	copyToClipboard				(),
	slotTooltip					(500),
	contentsTooltip				(this, &layoutParams, &renderingParams, 500, 70),
	renderingInfo				(static_cast<LayersRenderingInfo *>(0)),
	lastVisitedVertex			(static_cast<VertexRenderingInfo *>(0)),
	findAddOn					(),
	findApplicationCommunicator	(),
	findSourceCollectionProducer(this, &findAddOn, &colourAddOnManager),
	bookmarkManager				(this, &layoutCalculator),
	handTool					(this),
	zoomManager					(this, layoutParams.layerParams.layersAlignment),
	rectOflastVisitedVertex		((Rectangle *)0),
	isExplicitRefresh			(false)
{
	ApplyParams();
	findDialog = new IViewsFindDialog(
						this, 
						&findSourceCollectionProducer,
						&findApplicationCommunicator
					);
}

//-----------------------------------------------------------------------

IviewsCanvas::~IviewsCanvas() { Clean(); }


void IviewsCanvas::StartVerticesFromCommonPosition (bool b) {
	
	if (!renderingInfo && !layoutParams.layerParams.drawVertexWithFullContents)
		return;

	layoutCalculator.SetCommonBeginingEdges(renderingInfo, b);
	PREPARE_FOR_DRAW(false, graphCoordinator.GetGraphLayers(), colourAddOnManager);
	Refresh();
}

//-----------------------------------------------------------------------

void IviewsCanvas::StopAllTooltips (void){
	slotTooltip.Stop();
	contentsTooltip.Stop();
}

//-----------------------------------------------------------------------

void IviewsCanvas::DrawGraph (const wxString & expr, const long depth) { 
	Clean();
	graphs::GraphLayers * graphLayers = graphCoordinator(str2std(expr), depth);

	if ( graphCoordinator.HasError() )
		wxMessageBox(util::std2str(graphCoordinator.GetErrorMessage()), _("Error"), wxICON_ERROR);
	else if(graphCoordinator.GraphIsSimpleObje())
		wxMessageBox(util::std2str(graphCoordinator.GetDescriptionOfSimpleObject()));
	else {
		layoutCalculator.Clear();
		PREPARE_FOR_DRAW(true, graphLayers, colourAddOnManager);
		Refresh();	//To afinw
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::UpdateParams (			
		const wxFont			& font_, 
		const LayoutParams		& lparams,  
		const RenderingParams	& rparams
	){
	wxFont oldFont						= font;
	LayoutParams oldLayoutParams		= layoutParams;
	RenderingParams oldRenderingParams	= renderingParams;
	font								= font_;
	layoutParams						= lparams;
	renderingParams 					= rparams;
	ApplyParams();	

	if (!renderingInfo)
		return;

	LayoutParamsValidator validator;
	if (validator.IsLayoutParamsInvalid(oldLayoutParams, lparams)) {
		if (!validator.NeedsRecalculation()) {	//if not then skip the check of above
			if (validator.HasDrawLayersRectChanged())
				layoutCalculator.DrawLayersRectangle(renderingInfo, lparams.layerParams.drawLayersRect);
			
			if (validator.HasDrawVertexWithFullContentsChanged())
				layoutCalculator.ShowVerticesWithFullContents(renderingInfo, lparams.layerParams.drawVertexWithFullContents);			
			
			if (validator.HasLayersAlignmentChanged()) {
				layoutCalculator.SetLayersAlignmentTo(renderingInfo, lparams.layerParams.layersAlignment);
				zoomManager.SetLayerAlignment(layoutParams.layerParams.layersAlignment);
				zoomManager.CleanUp();
				zoomManager.Initialise();
			}
		}
	}

	if (!validator.NeedsRecalculation() && (oldRenderingParams != rparams)) {
		colourAddOnManager.RestoreColours(renderingInfo);
		layoutCalculator.UpdateColours(renderingInfo);
	}

	if (validator.NeedsRecalculation() || oldFont != font) {
				
		CleanUpManagers(false);
		CleanRenderingData();
		graphs::GraphLayers * graphLayers = graphCoordinator.GetGraphLayers();
		assert(graphLayers);
		PREPARE_FOR_DRAW(true, graphLayers, colourAddOnManager);
		if (renderingInfo)
			bookmarkManager.UpdateBookmarks(renderingInfo);
	}
	Refresh();	//To afinw
}

//-----------------------------------------------------------------------

ZoomManager & IviewsCanvas::GetZoomManager (void)
	{ return zoomManager; }

//-----------------------------------------------------------------------

IviewsWindow * IviewsCanvas::GetOwner (void) const
	{ return owner; } 

//-----------------------------------------------------------------------

Rectangle IviewsCanvas::GetBoundingBox (void) const {
	if (renderingInfo) 
		return renderingInfo->CalcBoundingBox(layoutParams.layerParams.layersDistance);	

	return Rectangle();
}

//-----------------------------------------------------------------------

LayersRenderingInfo * IviewsCanvas::GetLayersRenderingInfo (void) const
	{ return renderingInfo; }

//-----------------------------------------------------------------------

VertexRenderingInfo * IviewsCanvas::GetLastVisitedVertex (void) const
	{ return lastVisitedVertex; }


/// ///////////////////// Event functions  /////////////////////////

//-----------------------------------------------------------------------

void IviewsCanvas::OnErase (wxEraseEvent& event) 
	{}

//-----------------------------------------------------------------------

void IviewsCanvas::OnCancel (wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE && renderingInfo)
		if (colourAddOnManager.RemoveFirst(&findAddOn, renderingInfo))
			Refresh();	//Edw malon 8elei rect
}

//-----------------------------------------------------------------------

void IviewsCanvas::Refresh (bool erase_bg, const wxRect* rect) {
	isExplicitRefresh = true;
	wxScrolledWindow::Refresh(false, rect);
}

void IviewsCanvas::RefreshRect (const wxRect& rect, bool eraseBackground) {
	isExplicitRefresh = true;
	wxScrolledWindow::RefreshRect(rect, eraseBackground);
}

//-----------------------------------------------------------------------

void IviewsCanvas::OnPaint (wxPaintEvent &WXUNUSED(event)) {
    wxPaintDC dc(this);
	dc.Clear();
	MyPrepareDC(dc);
	
	if (canDraw) {
		renderingApi.SetWxDC(&dc);
		renderer(*layoutCalculator.GetPrimitivesList());
		
		if (!handTool.IsActive())
			DisplayLens();
	}

	GetOwner()->SetStatusText(zoomManager.GetZoomParams().ToString());
	
	isExplicitRefresh = false;
}

/// /////////////////////////// Mouse methods ///////////////////////////////

//-----------------------------------------------------------------------

void IviewsCanvas::OnMouseMove (wxMouseEvent &event) {
	if (!owner->IsFocused())
		return;

	coord_t x, y;
	wxPoint devicePos = event.GetPosition();
	GetLogicalPos(&x, &y, devicePos);

	if (renderingInfo) {
		lastVisitedVertex = VertexFinder()(renderingInfo, x, y);
		if (lastVisitedVertex) 
			findSourceCollectionProducer.SetLastVisitedVertex(lastVisitedVertex->GetSourceVertex());
		OnMouseMoveHighlightCurrentVertex(x, y);
	}
		
	if (!lensView.IsEnabled()) {
		SLOT_TOOLTIP();
		COONTENT_TOOLTIP();
	}

	MoveHandTool(devicePos, event.LeftIsDown());		//always after tooltips
	zoomManager.MouseMove(devicePos);
	MoveLens(x, y, devicePos, true);
}

//-----------------------------------------------------------------------

void IviewsCanvas::OnMouseWheel (wxMouseEvent & event) {
	if (event.ControlDown())
		if (event.ShiftDown())
			ZoomLens(event.GetWheelRotation() >= 0, event.GetPosition());
		else
			event.GetWheelRotation() >= 0 ? ZoomIn() : ZoomOut();
	else if (contentsTooltip.IsShown())
		contentsTooltip.OnMouseWheel(event);
	else
		HandleOnMouseWheel(event);
}

//-----------------------------------------------------------------------

void IviewsCanvas::OnMouseLeftUp (wxMouseEvent & event) {
	if (handTool.IsActive())
		handTool.MouseLeftUp();
	
	if (lensView.IsEnabled())
		ShowLens(event.GetPosition());
}

//-----------------------------------------------------------------------

void IviewsCanvas::OnMouseLeftDown (wxMouseEvent & event) { 
	StopAllTooltips();
	HideLens();
	handTool.MouseLeftDown(event.GetPosition()); 
}

//-----------------------------------------------------------------------

void IviewsCanvas::OnMouseRightDown (wxMouseEvent & event) {
	if (renderingInfo && lastVisitedVertex) {
		StopAllTooltips();
		owner->VertexMenu(
			event.GetPosition(), 
			lastVisitedVertex->HasFullContents() && !lastVisitedVertex->HasHiddenContents(),
			SlotFinder()(lastVisitedVertex, "{madein}") != static_cast<SlotRenderingInfo *>(0),
			lastVisitedVertex->GetEdgesParameters(),
			renderingInfo->GetLayerRenderingInfo(lastVisitedVertex->GetSourceLayer())->GetEdgesParameters(),
			renderingInfo->GetEdgesParameters()
		);
	}
}

/// //////////////////////////// Undo - Redo //////////////////////////////////


//-----------------------------------------------------------------------

void IviewsCanvas::Undo (void) {
	graphs::GraphLayers * graphLayers = ((graphs::GraphLayers *)0);
	
	if (graphLayers = graphCoordinator.Undo() ){
		layoutCalculator.Clear();
		PREPARE_FOR_DRAW(true, graphLayers, colourAddOnManager.Apply(renderingInfo));		
		Refresh();	
	}
	DisableMenuAndTools();
}

//-----------------------------------------------------------------------

void IviewsCanvas::Redo (void) {
	
	if (graphCoordinator.CanRedo()){
		layoutCalculator.Clear();
		graphs::GraphLayers * graphLayers = ((graphs::GraphLayers *)0);

		if (graphLayers = graphCoordinator.Redo() )
			{ PREPARE_FOR_DRAW(true, graphLayers, colourAddOnManager.Exclude(renderingInfo)); }
		Refresh();	
	}
	DisableMenuAndTools();
}


/// /////////////////////////////// Load - Save //////////////////////////////


//-----------------------------------------------------------------------

void IviewsCanvas::SaveGraph (const wxString & path) {
	GraphToXml graphToXML;

	if (!graphCoordinator.HasError() && graphCoordinator.GetGraph())
		graphToXML(	
			str2std(path),
			graphCoordinator.GetDepth(),
			*graphCoordinator.GetGraph(),
			*graphCoordinator.GetStartVertex(),
			*graphCoordinator.GetVertexToDebugInfoMapping()
		);
}

//-----------------------------------------------------------------------

void IviewsCanvas::LoadGraph (const wxString & path) {
	XmlToGraph graphLoader;
	if (renderingInfo)
		Clean();

	if (graphLoader(str2std(path))) {

		graphs::GraphLayers * graphLayers = graphCoordinator(
												graphLoader.GetDepth(),
												graphLoader.GetStartVertex(),
												graphLoader.GetGraph(),
												graphLoader.GetVertexToDebugInfoMapping()
											);
		layoutCalculator.Clear();		
		PREPARE_FOR_DRAW(true, graphLayers, colourAddOnManager);
		Refresh();	
		GetOwner()->SetStatusText(wxT("File opened: ") + path);
	}
	else  {
		assert(graphLoader.HasError());
		wxMessageBox(
			std2str(graphLoader.GetErrorMessage()), 
			_("Error"), 
			wxICON_ERROR
		);
		GetOwner()->SetStatusText(wxT("Failed to open: ") + path);
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::LoadConfigurations (const wxString & path) 
	{ XMLConfigurationLoader()(path, layoutParams, renderingParams); }

//-----------------------------------------------------------------------

void IviewsCanvas::SaveGraphAsImage (void) {	
	if (renderingInfo)
		imageSaver.Save(
			renderingInfo->CalcBoundingBox(layoutParams.layerParams.layersDistance),
			*layoutCalculator.GetPrimitivesList()
		);
}

//-----------------------------------------------------------------------

void IviewsCanvas::ApplyZoomOperation (void (ZoomManager::*op)(void)) {
	if (GetLayersRenderingInfo()) {				
		Freeze();								
		StopAllTooltips();
		(zoomManager.*op)();	
		findSourceCollectionProducer.SetScaleFactor(zoomManager.GetZoomParams().GetScaleFactor());		
		Thaw();									
		Refresh();	
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::ZoomIn (void) 
	{ ApplyZoomOperation(&ZoomManager::ZoomIn); }

void IviewsCanvas::ZoomFit (void) 
	{ ApplyZoomOperation(&ZoomManager::ZoomFit); }

void IviewsCanvas::ZoomOut (void) 
	{ ApplyZoomOperation(&ZoomManager::ZoomOut); }

void IviewsCanvas::ZoomRestore (void) 
	{ ApplyZoomOperation(&ZoomManager::ZoomRestore); }

//-----------------------------------------------------------------------

void IviewsCanvas::Find (void) { 
	findSourceCollectionProducer.SetScaleFactor(zoomManager.GetZoomParams().GetScaleFactor());
	findDialog->OpenDialog(""); 
}

//-----------------------------------------------------------------------

void IviewsCanvas::FindPrev (void) 
	{ findDialog->OnFindPrev(wxCommandEvent()); }

//-----------------------------------------------------------------------

void IviewsCanvas::FindNext (void) 
	{ findDialog->OnFindNext(wxCommandEvent()); }

/// //////////// Bookmarks ///////////////

//-----------------------------------------------------------------------

void IviewsCanvas::ToggleBookmark (void) {
	
	if (lastVisitedVertex) {
		owner->SetStatusText(wxT("Toggle Bookmark"));
		HideLens();
		bookmarkManager.ToggleBookmark(lastVisitedVertex);
		lensView.SetPrimitiveList(layoutCalculator.GetPrimitivesList());
		lensView.Refresh();
	}
	else
		owner->SetStatusText(wxT("Can't bookmark: no vertex selected"));
}

//-----------------------------------------------------------------------

void IviewsCanvas::DisableAllBookmarks (void) 
	{ bookmarkManager.DisableAllBookmarks(); }

//-----------------------------------------------------------------------

void IviewsCanvas::EnableBookmarks (void) 
	{ bookmarkManager.EnableBookmarks(); }

//-----------------------------------------------------------------------

void IviewsCanvas::GotoPreviousBookmark (void) 
	{ bookmarkManager.PreviousBookmark(); }

//-----------------------------------------------------------------------

void IviewsCanvas::GotoNextBookmark (void) 
	{ bookmarkManager.NextBookmark(); }

//-----------------------------------------------------------------------

void IviewsCanvas::ClearBookmarks (void) { 
	if (renderingInfo)
		bookmarkManager.ClearBookmarks(renderingInfo); 
}


/// //////////// Features ///////////////

//-----------------------------------------------------------------------

void IviewsCanvas::ExportGraphTo (GraphExporter::ExportedFileFormats format) {
	if (format == GraphExporter::DOT_FORMAT) {
		assert(
			graphCoordinator.GetGraph()						&& 
			graphCoordinator.GetStartVertex()				&& 
			graphCoordinator.GetVertexToDebugInfoMapping()
		);

		exporter.ExportToDot(
			*graphCoordinator.GetGraph(),
			*graphCoordinator.GetStartVertex(),
			*graphCoordinator.GetVertexToDebugInfoMapping()
		);
	}
	else if (format == GraphExporter::SVG_FORMAT)
		if (renderingInfo) {
			Rectangle bb = renderingInfo->CalcBoundingBox(layoutParams.layerParams.layersDistance);
			exporter.ExportToSVG(
				bb.GetWidth(),
				bb.GetHeight(),
				*layoutCalculator.GetPrimitivesList()
			);	
		}
	else
		assert (0);
}

//-----------------------------------------------------------------------

void IviewsCanvas::HideOrShowVertexEdges (const EdgeHiding::EdgeHidingPreferences & prefrence) {
	if (lastVisitedVertex) {
		edgeHiding.HideOrShowEdgesForVertex(lastVisitedVertex, prefrence);
		Refresh();
	}
}
void IviewsCanvas::HideOrShowLayerEdges (const EdgeHiding::EdgeHidingPreferences & prefrence) {
	if (renderingInfo && lastVisitedVertex) {
		
		edgeHiding.HideOrShowEdgesForLayre(
			renderingInfo->GetLayerRenderingInfo(lastVisitedVertex->GetSourceLayer()),
			prefrence
		);
		Refresh();
	}
}
void IviewsCanvas::HideOrShowLayersEdges (const EdgeHiding::EdgeHidingPreferences & prefrence) {
	if (renderingInfo) {
		edgeHiding.HideOrShowEdgesForLayres(renderingInfo, prefrence);
		Refresh();
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::EnableLensView (void) 
	{ lensView.Show(); }

//-----------------------------------------------------------------------

void IviewsCanvas::ShowLastVisitedVertexContents (void) {
	if (lastVisitedVertex) {
		if (layoutCalculator.ShowContentsOfVertex(lastVisitedVertex, renderingInfo) ) { 
			colourAddOnManager.Exclude(renderingInfo);
			Rectangle bb = renderingInfo->CalcBoundingBox(layoutParams.layerParams.layersDistance);
			SetVirtualSize(bb.GetWidth(), bb.GetHeight());	
			zoomManager.AdjustScrollSteps();
			Refresh();
		}
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::HideLastVisitedVertexContents (void) {
	if (lastVisitedVertex)
		if (layoutCalculator.HideContentsOfVertex(lastVisitedVertex, renderingInfo)) {
			colourAddOnManager.Exclude(renderingInfo);
			Refresh();	
		}
}

//-----------------------------------------------------------------------

void IviewsCanvas::CancelAllDisplayAddOns (void) { 
	if (renderingInfo) {
		colourAddOnManager.RestoreColours(renderingInfo);
		Refresh();	
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::HighlightAllRelativeSlots (void) {
	relativeSlotsHighlighter;

	if (renderingInfo && lastVisitedVertex) 
		if (colourAddOnManager.AddOrRemoveConditionalAddOn(
				&relativeSlotsHighlighter, 
				new ColourAddOnData(lastVisitedVertex->GetSourceVertex()), 
				renderingInfo
			) )
		Refresh();	
}

//-----------------------------------------------------------------------

void IviewsCanvas::RestoreRelativeSlotsColours (void) {
	if (renderingInfo)
		if (colourAddOnManager.RemoveAll(&relativeSlotsHighlighter, renderingInfo))
			Refresh();	
}

//-----------------------------------------------------------------------

void IviewsCanvas::RestorePathColours (void){ 	
	if (renderingInfo) {
		colourAddOnManager.RemoveFirst(&currentVertexHighlighter, renderingInfo);
		colourAddOnManager.RemoveAll(&pathHighlighting, renderingInfo);
		Refresh();	
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::RemoveLastVisitedVertex (void) {

	if (lastVisitedVertex) {
		graphs::Vertex * vertex = lastVisitedVertex->GetSourceVertex(); //the vertex we will delate from graph
		
		CleanUpManagers(true);
		CleanRenderingData();
		graphs::GraphLayers * graphLayers = graphCoordinator.ExcludeVertex(*vertex);
	
		if (graphLayers) { 
			PREPARE_FOR_DRAW(true, graphLayers, colourAddOnManager.Exclude(renderingInfo)); 
			bookmarkManager.UpdateBookmarks(renderingInfo);
		}
		else {	//deleted start vertex
			canDraw = false;
			SetVirtualSize(0, 0);		
			Scroll(0, 0);
			bookmarkManager.CleanUp();
		}
		Refresh();	
	}
	DisableMenuAndTools();
}

//-----------------------------------------------------------------------

void IviewsCanvas::CopyAbsRefToClipboard (void) {
	if (renderingInfo && lastVisitedVertex)
		copyToClipboard(lastVisitedVertex, graphCoordinator.GetVertexToDebugInfoMapping());
}

//-----------------------------------------------------------------------

bool IviewsCanvas::GotoSourceFile (void) {
	if (renderingInfo && lastVisitedVertex) 
		return GotoSourcePoint()(lastVisitedVertex);
	return false;
}

//-----------------------------------------------------------------------

void IviewsCanvas::ViewSlot (const wxPoint & devicePos) {
	coord_t x, y;
	GetLogicalPos(&x, &y, devicePos);

	if (renderingInfo && lastVisitedVertex) 
		ViewSlotTextualContents()(this, lastVisitedVertex, x, y);
}

//-----------------------------------------------------------------------

void IviewsCanvas::HighlightPath (
		HighlightingDirection direction,
		bool highlightInvolvedSlots
	) {
	if (renderingInfo && lastVisitedVertex) {
		colourAddOnManager.RemoveFirst(&currentVertexHighlighter, renderingInfo);
		colourAddOnManager.AddOrRemoveConditionalAddOn(
			&pathHighlighting, 
			new PathHighlightingAddOnData(
				direction,
				lastVisitedVertex->GetSourceVertex(),
				highlightInvolvedSlots
			), 
			renderingInfo
		);
		Refresh();	
	}
}



//////////////////////////////////////////////
//	Private Method Api implementation

//-----------------------------------------------------------------------

void IviewsCanvas::DisableMenuAndTools (void) {
	if (!renderingInfo) {
		owner->EnableTools(false);
		owner->EnableMenus(false);
		owner->EnableNewLoadGraphMenus(true);
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::RefreshRectangle (Rectangle * rect) {
	assert(rect);

	double scaleFactor = zoomManager.GetZoomParams().GetScaleFactor();
	//int viewX, viewY;
	//CalcScrolledPosition(rect->GetX(), rect->GetY(), &viewX, &viewY);
	wxClientDC dc(this);
	MyPrepareDC(dc);
	int lx = dc.LogicalToDeviceX(rect->GetX());
	int ly = dc.LogicalToDeviceY(rect->GetY());
	int lw = dc.LogicalToDeviceXRel(rect->GetWidth());
	int lh = dc.LogicalToDeviceYRel(rect->GetHeight());
	
	RefreshRect(
		wxRect(
			lx,
			ly,
			lw,
			lh
		),
		false
	);
	//Update();
}

//-----------------------------------------------------------------------

bool IviewsCanvas::IsUsingLens (void) const
	{ return !handTool.IsActive() && lensView.IsEnabled() && lensView.IsInitialised(); } 

/////////////////////////////////////////////////////////////////////////

void IviewsCanvas::ZoomLens (bool zoomIn, const wxPoint& devicePos) {

	if (IsUsingLens()) {
		if (zoomIn)
			lensView.ZoomIn();
		else
			lensView.ZoomOut();
		coord_t x, y;
		GetLogicalPos(&x, &y, devicePos);
		MoveLens(x, y,  devicePos, true);
	}
}

/////////////////////////////////////////////////////////////////////////

void IviewsCanvas::ShowLens (const wxPoint& devicePos) {
	//assert(IsUsingLens());
	coord_t x, y;
	GetLogicalPos(&x, &y, devicePos);
	MoveLens(x, y, devicePos, false);
}

/////////////////////////////////////////////////////////////////////////

void IviewsCanvas::HideLens (void) {
	if (IsUsingLens()) {
		wxClientDC dc(this);
		MyPrepareDC(dc);
		lensView.DisplayLensRectangle(dc);
	}
}

/////////////////////////////////////////////////////////////////////////

void IviewsCanvas::DisplayLens (void) {

	if (IsUsingLens()) {

		wxClientDC dc(this);
		MyPrepareDC(dc);

		wxPoint oldLogical = lensView.GetOldLogicalMousePos();
		wxPoint oldDevice  = lensView.GetOldDeviceMousePos();
		coord_t x, y;
		GetLogicalPos(&x, &y, oldDevice);

		if (!isExplicitRefresh)	// Will not redraw anyway
			lensView.DisplayLensRectangle(dc);		// To hide old rect.

		if (x != oldLogical.x || y != oldLogical.y)	// Move to new position (don't hide old rect).
			MoveLens(x, y, oldDevice, false);
		else
			lensView.DisplayLensRectangle(dc);		// Just display at current position.
	}
}

/////////////////////////////////////////////////////////////////////////

void IviewsCanvas::MoveLens (
		int				logicalMouseX, 
		int				logicalMouseY, 
		const wxPoint&	deviceMousePos, 
		bool			clearOldRect
	) {

	if (!handTool.IsActive() && lensView.IsEnabled()) {

		StopAllTooltips();
		wxClientDC dc(this);
		MyPrepareDC(dc);

		lensView.MoveLens(
			this, 
			dc, 
			wxPoint(logicalMouseX, logicalMouseY),
			deviceMousePos,
			zoomManager.GetZoomParams().GetScaleFactor(),
			clearOldRect
		);
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::MoveHandTool (const wxPoint& mousePos, bool leftMouseButtonIsDown) {
	if (handTool.IsActive()) {
		StopAllTooltips();
		handTool.MouseMove(mousePos, leftMouseButtonIsDown);
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::OnMouseMoveHighlightCurrentVertex (coord_t lx, coord_t ly) {

	if (lensView.IsEnabled() || handTool.IsActive() || !renderingInfo)
		return;

	graphs::Vertex * v = lastVisitedVertex ? lastVisitedVertex->GetSourceVertex() : (graphs::Vertex *)0;

	HighlightCurrentVertexAddOnData * data = new HighlightCurrentVertexAddOnData(v, lx, ly);

	if (colourAddOnManager.AddOrRemoveConditionalAddOn(
			&currentVertexHighlighter, 
			data, 
			renderingInfo
		) ) {

			Rectangle * curr = (Rectangle *)0;
			if (lastVisitedVertex) {
				curr = lastVisitedVertex->GetVertexMainRectangle();
				RefreshRectangle(curr);
			}
			
			if (rectOflastVisitedVertex && 
				(!lastVisitedVertex || (curr && *rectOflastVisitedVertex != curr)) 
				) {
				RefreshRectangle(rectOflastVisitedVertex);
			}
			rectOflastVisitedVertex = curr;
	}
}

//-----------------------------------------------------------------------

void IviewsCanvas::GetLogicalPos (coord_t* x, coord_t* y, const wxPoint & pt) {
	wxClientDC dc(this);
	MyPrepareDC(dc);
	*x = dc.DeviceToLogicalX(pt.x);	
	*y = dc.DeviceToLogicalY(pt.y);
}

//-----------------------------------------------------------------------

void IviewsCanvas::MyPrepareDC (wxDC & dc) {
	DoPrepareDC(dc);																		
	dc.SetAxisOrientation(true, false);														
	dc.SetUserScale(
		zoomManager.GetZoomParams().GetScaleFactor(), 
		zoomManager.GetZoomParams().GetScaleFactor()
	);
	dc.SetMapMode(wxMM_TEXT);	/*Each logical unit is 1 pixel.*/							
	dc.SetFont(font);																		
}

//-----------------------------------------------------------------------

void IviewsCanvas::ApplyParams (void) {
	if (!layoutParams.primitives)																			
		layoutParams.primitives = &wxGraphicsLayoutPrimitives;												
	
	lensView.SetLensFont(font);
	layoutCalculator.SetLayoutParams(&layoutParams);
	layoutCalculator.SetRenderingParams(&renderingParams);
	contentsTooltip.SetLayoutParams(&layoutParams);															
	contentsTooltip.SetRenderingParams(&renderingParams);														
	findSourceCollectionProducer.SetSelectionColour(renderingParams.colours.selectionColour);
	currentVertexHighlighter.SetSlotColour(renderingParams.colours.highlightCurrentSlotColour);				
	currentVertexHighlighter.SetVertexColour(renderingParams.colours.highlightCurrentVertexColour);			
	relativeSlotsHighlighter.SetSlotsColour(renderingParams.colours.highlightRelativeSlotsColour);				
	pathHighlighting.SetIncomingEdgesColour(renderingParams.colours.pathHighlightingIncomingEdgesColour);	
	pathHighlighting.SetOutgoingEdgesColour(renderingParams.colours.pathHighlightingOutgoingEdgesColour);	
}

//-----------------------------------------------------------------------

void IviewsCanvas::Clean (void) {
	CleanUpManagers(true);
	graphCoordinator.Clean();
	CleanRenderingData();
}

//-----------------------------------------------------------------------
void IviewsCanvas::CleanRenderingData (void) {
	layoutCalculator.Clear();
	if (renderingInfo) {
		delete renderingInfo;
		renderingInfo = static_cast<LayersRenderingInfo *> (0);
	}
	canDraw					= false;
	rectOflastVisitedVertex	= static_cast<Rectangle *>(0);
	lastVisitedVertex		= static_cast<VertexRenderingInfo *>(0);
}

//-----------------------------------------------------------------------

void IviewsCanvas::InitialiseManagers (void) {
	zoomManager.Initialise();
	bookmarkManager.Initialise();
}

void IviewsCanvas::CleanUpManagers (bool cleanBookmarkManage) {
		
	zoomManager.CleanUp();
	if (renderingInfo)
		colourAddOnManager.RestoreColours(renderingInfo);

	if (cleanBookmarkManage)
		bookmarkManager.CleanUp();	
}

}	//namespace iviews 