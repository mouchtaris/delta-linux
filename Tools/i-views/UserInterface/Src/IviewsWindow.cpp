/**
 *	IviewsWindow.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <wx/menu.h>
#include <wx/numdlg.h>
#include <wx/msgdlg.h>
#include <wx/gdicmn.h>
#include <wx/textdlg.h>
#include <wx/xml/xml.h>
#include <wx/filedlg.h>

#include "LayoutParams.h"
#include "IviewsWindow.h"
#include "GraphExporter.h"
#include "ReferenceDepth.h"
#include "TypesConverter.h"
#include "RenderingParams.h"
#include "PathHighlighting.h"


#include "new.xpm"
#include "load.xpm"
#include "save.xpm"
#include "image.xpm"
#include "zoom_in.xpm"
#include "zoom_out.xpm"
#include "book_next.xpm"
#include "book_prev.xpm"
#include "book_clear.xpm"
#include "lens_view.xpm"
#include "book_toggle.xpm"
#include "fit_to_window.xpm"

#define HIDE_INNER_EDGES_STRING			wxT("Hide Inner Edges")
#define HIDE_INCOMING_EDGES_STRING		wxT("Hide Incoming Edges")
#define HIDE_OUTGOING_EDGES_STRING		wxT("Hide Outgoing Edges")
#define HIDE_ALL_EDGES_STRING			wxT("Hide All Edges")

#define SHOW_INNER_EDGES_STRING			wxT("Show Inner Edges")
#define SHOW_INCOMING_EDGES_STRING		wxT("Show Incoming Edges")
#define SHOW_OUTGOING_EDGES_STRING		wxT("Show Outgoing Edges")
#define SHOW_ALL_EDGES_STRING			wxT("Show All Edges")


#define DRAW_EDGES_FROM_OBJECTS_STRING	wxT("Draw outgoing edges from Objects")
#define DRAW_EDGES_FROM_SLOTS_STRING	wxT("Draw outgoing edges from Slot")

#define	IVIES_FRAME_STYLE				\
        wxDEFAULT_FRAME_STYLE		|	\
		wxNO_FULL_REPAINT_ON_RESIZE |	\
		wxCLIP_CHILDREN				



// IDs for the controls and the menu commands
enum {
    // menu items
    FILE_QUIT	= wxID_EXIT,
	TOOLBAR_ID = 0,

	MENU_BAR_ELEMTS_BEGIN,
	MENU_ELEMENT_NEW_EXPRESSION,
	MENU_ELEMENT_LOAD_GRAPH,
	MENU_ELEMENT_SAVE_GRAPH,
	MENU_ELEMENT_SAVE_GRAPH_AS_IMAGE,
	MENU_ELEMENT_EXPORT_SUB_MENU,
	MENU_ELEMENT_ZOOM_IN,
    MENU_ELEMENT_ZOOM_OUT,
    MENU_ELEMENT_ZOOM_RESTORE,
	MENU_ELEMENT_LENS_TOOL,
	MENU_ELEMENT_SUBMENU_FIND,
	MENU_ELEMENT_FIT_TO_WINDOW,
	MENU_ELEMENT_SUBMENU_BOOKMARKS,
	MENU_ELEMENT_UNDO,
	MENU_ELEMENT_REDO,
	MENU_ELEMENT_STATISTICS,
	MENU_ELEMENT_DRAW_EDGES_FROM_OBJECTS,
	MENU_ELEMENT_CANCEL_ALL_DISPLAY_ADDONS,
	MENU_BAR_ELEMTS_END,
	
	FIND,
	FIND_NEXT,
	FIND_PREV,
	
	TOGGLE_BOOKMARK, 
	DISABLE_ALL_BOOKMARKS, 
	ENABLE_BOOKMARK,
	PREVIOUS_BOOKMARK,
	NEXT_BOOKMARK,
	CLEAR_BOOKMARKS,

	EXPORT_AS_DOT,
	EXPORT_AS_SVG,

	SUBMENU_EDGES,
	HIDE_VERTEX_INNER_EDGES,
	HIDE_VERTEX_INCOMING_EDGES,
	HIDE_VERTEX_OUTGOING_EDGES,
	HIDE_VERTEX_ALL_EDGES,
	SHOW_VERTEX_INNER_EDGES,
	SHOW_VERTEX_INCOMING_EDGES,
	SHOW_VERTEX_OUTGOING_EDGES,
	SHOW_VERTEX_ALL_EDGES,
	HIDE_LAYER_INNER_EDGES,
	HIDE_LAYER_INCOMING_EDGES,
	HIDE_LAYER_OUTGOING_EDGES,
	HIDE_LAYER_ALL_EDGES,
	SHOW_LAYER_INNER_EDGES,
	SHOW_LAYER_INCOMING_EDGES,
	SHOW_LAYER_OUTGOING_EDGES,
	SHOW_LAYER_ALL_EDGES,
	HIDE_LAYERS_INNER_EDGES,
	HIDE_LAYERS_INCOMING_EDGES,
	HIDE_LAYERS_OUTGOING_EDGES,
	HIDE_LAYERS_ALL_EDGES,
	SHOW_LAYERS_INNER_EDGES,
	SHOW_LAYERS_INCOMING_EDGES,
	SHOW_LAYERS_OUTGOING_EDGES,
	SHOW_LAYERS_ALL_EDGES,

	VERTEX_SHOW_CONTENTS,
	VERTEX_HIDE_CONTENTS,
	VERTEX_COPY_TEXT,
	VERTEX_REMOVE,
	VERTEX_RESTORE_COLOURS,
	VERTEX_GOTO_SOURCE,
	VERTEX_HIGHLIGHT_BOTH,
	VERTEX_HIGHLIGHT_INCOMING,
	VERTEX_HIGHLIGHT_OUTGOING,
	VERTEX_HIGHLIGHT_INVOLVED_SLOTS,
	VERTEX_SLOT_TEXTUAL_CONTENTS,
	VERTEX_HIGHLIGHT_RELATIVE_SLOTS,
	VERTEX_RESTORE_RELATIVE_SLOTS_COLOURS,
	VERTEX_SUBMENU_PATH_HIGHLIGHTING
};



namespace iviews{

BEGIN_EVENT_TABLE(IviewsWindow, wxFrame)
    EVT_MENU 		(FILE_QUIT,								IviewsWindow::OnQuit)
    EVT_MENU 		(EXPORT_AS_DOT,							IviewsWindow::ExportGraph)
	EVT_MENU 		(EXPORT_AS_SVG,							IviewsWindow::ExportGraph)
	EVT_MENU 		(MENU_ELEMENT_NEW_EXPRESSION,			IviewsWindow::OnSelectExpression)
	EVT_MENU 		(MENU_ELEMENT_SAVE_GRAPH,				IviewsWindow::OnSaveGraph)
	EVT_MENU 		(MENU_ELEMENT_LOAD_GRAPH,				IviewsWindow::OnLoadGraph)	
	EVT_MENU 		(MENU_ELEMENT_SAVE_GRAPH_AS_IMAGE,		IviewsWindow::OnSaveGraphAsImage)	
	EVT_MENU 		(MENU_ELEMENT_FIT_TO_WINDOW,			IviewsWindow::OnFitToWindo)	
	EVT_MENU 		(MENU_ELEMENT_ZOOM_IN,					IviewsWindow::OnZoom)
	EVT_MENU 		(MENU_ELEMENT_ZOOM_OUT,					IviewsWindow::OnZoom)
	EVT_MENU 		(MENU_ELEMENT_ZOOM_RESTORE,				IviewsWindow::OnZoom)
	EVT_MENU 		(VERTEX_GOTO_SOURCE,					IviewsWindow::OnGotoSource)
	EVT_MENU 		(TOGGLE_BOOKMARK,						IviewsWindow::OnBookmarks)
	EVT_MENU 		(DISABLE_ALL_BOOKMARKS,					IviewsWindow::OnBookmarks)
	EVT_MENU 		(ENABLE_BOOKMARK,						IviewsWindow::OnBookmarks)
	EVT_MENU 		(PREVIOUS_BOOKMARK,						IviewsWindow::OnBookmarks)
	EVT_MENU 		(NEXT_BOOKMARK,							IviewsWindow::OnBookmarks)
	EVT_MENU 		(CLEAR_BOOKMARKS,						IviewsWindow::OnBookmarks)
	EVT_MENU 		(MENU_ELEMENT_LENS_TOOL,				IviewsWindow::OnLensTool)
	EVT_MENU 		(MENU_ELEMENT_STATISTICS,				IviewsWindow::OnStatistics)
	EVT_MENU 		(FIND,									IviewsWindow::OnFind)
	EVT_MENU 		(FIND_NEXT,								IviewsWindow::OnFind)
	EVT_MENU 		(FIND_PREV,								IviewsWindow::OnFind)
	EVT_MENU 		(MENU_ELEMENT_UNDO,						IviewsWindow::OnUndoRedo)
	EVT_MENU 		(MENU_ELEMENT_REDO,						IviewsWindow::OnUndoRedo)
	EVT_MENU 		(MENU_ELEMENT_DRAW_EDGES_FROM_OBJECTS,	IviewsWindow::OnStartVerticesFromCommonPosition)
	EVT_MENU 		(MENU_ELEMENT_CANCEL_ALL_DISPLAY_ADDONS,IviewsWindow::OnCancelAllDisplayAddOns)
	EVT_MENU 		(VERTEX_SLOT_TEXTUAL_CONTENTS,			IviewsWindow::OnViewSlotTextualContents)
	EVT_MENU 		(VERTEX_COPY_TEXT,						IviewsWindow::OnCopyVertexAbsRef)
	EVT_MENU 		(VERTEX_REMOVE,							IviewsWindow::OnRemoveVertex)
	EVT_MENU 		(VERTEX_RESTORE_COLOURS,				IviewsWindow::OnRestorePathColours)
	EVT_MENU 		(VERTEX_HIGHLIGHT_BOTH,					IviewsWindow::OnPathHighlight)
	EVT_MENU 		(VERTEX_HIGHLIGHT_INCOMING,				IviewsWindow::OnPathHighlight)
	EVT_MENU 		(VERTEX_HIGHLIGHT_OUTGOING,				IviewsWindow::OnPathHighlight)
	EVT_MENU 		(VERTEX_HIGHLIGHT_INVOLVED_SLOTS,		IviewsWindow::OnPathHighlight)
	EVT_MENU 		(VERTEX_HIGHLIGHT_RELATIVE_SLOTS,		IviewsWindow::OnPathHighlight)
	EVT_MENU 		(VERTEX_RESTORE_RELATIVE_SLOTS_COLOURS,	IviewsWindow::OnPathHighlight)
	EVT_MENU 		(VERTEX_SHOW_CONTENTS,					IviewsWindow::OnShowContents)
	EVT_MENU 		(VERTEX_HIDE_CONTENTS,					IviewsWindow::OnHideContents)
	EVT_TOOL_ENTER	(TOOLBAR_ID,							IviewsWindow::OnMouseEnternInToolbar)
	EVT_SET_FOCUS											(IviewsWindow::OnSetFocus)
	EVT_KILL_FOCUS											(IviewsWindow::OnKillFocus)
	EVT_CHAR												(IviewsWindow::OnCancel)
	EVT_MOUSEWHEEL											(IviewsWindow::OnMouseWheel)
	EVT_RIGHT_DOWN											(IviewsWindow::OnRightDown)
	EVT_MENU_RANGE (HIDE_VERTEX_INNER_EDGES, SHOW_LAYERS_ALL_EDGES, IviewsWindow::OnHideShowEdges)

END_EVENT_TABLE()

void IviewsWindow::OnRightDown (wxMouseEvent & evt) {
	canvas->OnMouseRightDown(evt);
}



//-----------------------------------------------------------------------
// frame constructor
IviewsWindow::IviewsWindow (
	wxWindow				* parent, 
	const wxFont			& font, 
	const LayoutParams		& lparams,  
	const RenderingParams	& rparams
) :	wxFrame(
		parent, 
		wxID_ANY, 
		wxT("i-views (Main View)"), 
		wxPoint(50, 50), 
		wxSize(550, 340),
        IVIES_FRAME_STYLE				
	),
	isFocused				(true),
	selectedText			(""),
	highlightInvolvedSlots	(false),
	menuBar					((wxMenuBar*)0),
	toolBar					((wxToolBar*)0),
	vertexOptions			((wxMenu*)0)
{
	canvas = new IviewsCanvas(this, font, lparams, rparams);

	statistics = new Statistics(this);
	PopulateMenuBar();
	PopulateToolbar();
    CreateStatusBar(1);
    SetStatusText(wxT("Ready"));

	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add(canvas, 1, wxEXPAND);
    SetAutoLayout(true);
    SetSizer(sizer);

	Centre();
}

//-----------------------------------------------------------------------

IviewsWindow::~IviewsWindow(){}

//-----------------------------------------------------------------------

void IviewsWindow::OnStatistics (wxCommandEvent & event) {
	if (canvas->GetLayersRenderingInfo())
		statistics->ShowStatistics(canvas->GetLayersRenderingInfo());
}

//-----------------------------------------------------------------------

void IviewsWindow::ExportGraph (wxCommandEvent & event) {
	if (event.GetId() == EXPORT_AS_DOT)
		canvas->ExportGraphTo(GraphExporter::DOT_FORMAT);
	else if (event.GetId() == EXPORT_AS_SVG)
		canvas->ExportGraphTo(GraphExporter::SVG_FORMAT);
	else 
		assert(0);
}

//-----------------------------------------------------------------------

bool IviewsWindow::IsFocused (void) const
	{ return isFocused; }

void IviewsWindow::OnSetFocus (wxFocusEvent &event)
	{ isFocused = true; }

void IviewsWindow::OnKillFocus (wxFocusEvent &event)
	{ isFocused = false; }

//-----------------------------------------------------------------------

void IviewsWindow::UpdateParams (			
		const wxFont			& font, 
		const LayoutParams		& lparams,  
		const RenderingParams	& rparams
	) {
	canvas->UpdateParams(font, lparams, rparams);
}

//-----------------------------------------------------------------------

void IviewsWindow::OnQuit (wxCommandEvent& WXUNUSED(event)) {	
	if (!IsBeingDeleted()) {
		Destroy();
		Close(true);	
	}
}

//-----------------------------------------------------------------------

void IviewsWindow::OnMouseEnternInToolbar (wxCommandEvent& event) 
	{ canvas->StopAllTooltips(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnCancel (wxKeyEvent& event) 
	{ canvas->OnCancel(event); }

//-----------------------------------------------------------------------

void IviewsWindow::OnZoom (wxCommandEvent& event) {
	switch (event.GetId()) {
        case MENU_ELEMENT_ZOOM_IN:		canvas->ZoomIn();		break;
		case MENU_ELEMENT_ZOOM_OUT:		canvas->ZoomOut();		break;
		case MENU_ELEMENT_ZOOM_RESTORE:	canvas->ZoomRestore();	break;
        default: assert(0);
    }
}

//-----------------------------------------------------------------------

void IviewsWindow::OnLensTool (wxCommandEvent& event) 
	{ canvas->EnableLensView(); }

//-----------------------------------------------------------------------
void IviewsWindow::OnFitToWindo (wxCommandEvent& event) 
	{ canvas->ZoomFit(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnFind (wxCommandEvent& event) { 
	switch(event.GetId()) {
		case FIND		: canvas->Find();		break;
		case FIND_NEXT	: canvas->FindNext();	break;
		case FIND_PREV	: canvas->FindPrev();	break;
		default:
			assert(0);
	};
}

//-----------------------------------------------------------------------

void IviewsWindow::OnHideShowEdges (wxCommandEvent& event) {
	switch(event.GetId()) {
		case HIDE_VERTEX_INNER_EDGES:		canvas->HideOrShowVertexEdges(EdgeHiding::HideInnerEdges);		break;
		case HIDE_VERTEX_INCOMING_EDGES:	canvas->HideOrShowVertexEdges(EdgeHiding::HideIcomingEdges);	break;
		case HIDE_VERTEX_OUTGOING_EDGES:	canvas->HideOrShowVertexEdges(EdgeHiding::HideOutgoingEdges);	break;
		case HIDE_VERTEX_ALL_EDGES:			canvas->HideOrShowVertexEdges(EdgeHiding::HideAllEdges);		break;
		case SHOW_VERTEX_INNER_EDGES:		canvas->HideOrShowVertexEdges(EdgeHiding::ShowInnerEdges);		break;
		case SHOW_VERTEX_INCOMING_EDGES:	canvas->HideOrShowVertexEdges(EdgeHiding::ShowIncomingEdges);	break;
		case SHOW_VERTEX_OUTGOING_EDGES:	canvas->HideOrShowVertexEdges(EdgeHiding::ShowOutgoingEdges);	break;
		case SHOW_VERTEX_ALL_EDGES:			canvas->HideOrShowVertexEdges(EdgeHiding::ShowAllEdges);		break;

		case HIDE_LAYER_INNER_EDGES:		canvas->HideOrShowLayerEdges(EdgeHiding::HideInnerEdges);		break;
		case HIDE_LAYER_INCOMING_EDGES:		canvas->HideOrShowLayerEdges(EdgeHiding::HideIcomingEdges);		break;
		case HIDE_LAYER_OUTGOING_EDGES:		canvas->HideOrShowLayerEdges(EdgeHiding::HideOutgoingEdges);	break;
		case HIDE_LAYER_ALL_EDGES:			canvas->HideOrShowLayerEdges(EdgeHiding::HideAllEdges);			break;
		case SHOW_LAYER_INNER_EDGES:		canvas->HideOrShowLayerEdges(EdgeHiding::ShowInnerEdges);		break;
		case SHOW_LAYER_INCOMING_EDGES:		canvas->HideOrShowLayerEdges(EdgeHiding::ShowIncomingEdges);	break;
		case SHOW_LAYER_OUTGOING_EDGES:		canvas->HideOrShowLayerEdges(EdgeHiding::ShowOutgoingEdges);	break;
		case SHOW_LAYER_ALL_EDGES:			canvas->HideOrShowLayerEdges(EdgeHiding::ShowAllEdges);			break;

		case HIDE_LAYERS_INNER_EDGES:		canvas->HideOrShowLayersEdges(EdgeHiding::HideInnerEdges);		break;
		case HIDE_LAYERS_INCOMING_EDGES:	canvas->HideOrShowLayersEdges(EdgeHiding::HideIcomingEdges);	break;
		case HIDE_LAYERS_OUTGOING_EDGES:	canvas->HideOrShowLayersEdges(EdgeHiding::HideOutgoingEdges);	break;
		case HIDE_LAYERS_ALL_EDGES:			canvas->HideOrShowLayersEdges(EdgeHiding::HideAllEdges);		break;
		case SHOW_LAYERS_INNER_EDGES:		canvas->HideOrShowLayersEdges(EdgeHiding::ShowInnerEdges);		break;
		case SHOW_LAYERS_INCOMING_EDGES:	canvas->HideOrShowLayersEdges(EdgeHiding::ShowIncomingEdges);	break;
		case SHOW_LAYERS_OUTGOING_EDGES:	canvas->HideOrShowLayersEdges(EdgeHiding::ShowOutgoingEdges);	break;
		case SHOW_LAYERS_ALL_EDGES:			canvas->HideOrShowLayersEdges(EdgeHiding::ShowAllEdges);		break;

		default:
			assert(0);
	};

}

//-----------------------------------------------------------------------

void IviewsWindow::OnUndoRedo (wxCommandEvent& event) { 
	switch(event.GetId()) {
		case MENU_ELEMENT_UNDO : canvas->Undo();	break;
		case MENU_ELEMENT_REDO : canvas->Redo();	break;
		default:
			assert(0);
	};
}

//-----------------------------------------------------------------------

void IviewsWindow::OnBookmarks (wxCommandEvent& event) {
	switch(event.GetId()) {
		case TOGGLE_BOOKMARK		: canvas->ToggleBookmark();			break;
		case DISABLE_ALL_BOOKMARKS	: canvas->DisableAllBookmarks();	break;
		case ENABLE_BOOKMARK		: canvas->EnableBookmarks();		break;
		case PREVIOUS_BOOKMARK		: canvas->GotoPreviousBookmark();	break;
		case NEXT_BOOKMARK			: canvas->GotoNextBookmark();		break;
		case CLEAR_BOOKMARKS		: canvas->ClearBookmarks();			break;
		default:
			assert(0);
	};
}

//-----------------------------------------------------------------------

void IviewsWindow::OnMouseWheel (wxMouseEvent & event) 
	{ canvas->OnMouseWheel(event); }

//-----------------------------------------------------------------------

void IviewsWindow::OnShowContents (wxCommandEvent& event)
	{ canvas->ShowLastVisitedVertexContents(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnHideContents (wxCommandEvent& event)
	{ canvas->HideLastVisitedVertexContents(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnSaveGraph (wxCommandEvent& event) {

	wxFileDialog * saveDialog = new wxFileDialog(
										this, 
										wxT("Save File As _?"), 
										wxEmptyString, 
										wxEmptyString,
										wxT("XML file (*.xml)|*.xml"),
										wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition
								);
 
	if (saveDialog->ShowModal() == wxID_OK) {
		wxString currentDocPath = saveDialog->GetPath();
		canvas->SaveGraph(currentDocPath);
		SetStatusText(wxT("File saved: ") + currentDocPath);
	}


}

//-----------------------------------------------------------------------

void IviewsWindow::OnLoadGraph (wxCommandEvent& event) {
	wxFileDialog * openDialog = new wxFileDialog(
										this, 
										wxT("Choose a XML File to Open"), 
										wxEmptyString, 
										wxEmptyString,
										wxT("XML file (*.xml)|*.xml"),
										wxFD_OPEN, wxDefaultPosition
								);

	if (openDialog->ShowModal() == wxID_OK) {
		wxString currentDocPath = openDialog->GetPath();
		canvas->LoadGraph(currentDocPath);
	}
}

//-----------------------------------------------------------------------

void IviewsWindow::OnSaveGraphAsImage (wxCommandEvent& event) 
	{ canvas->SaveGraphAsImage(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnLoadConfigurations (wxCommandEvent& event){
	wxString path = wxFileSelector(
						wxT("Choose a XML File to Open"), 
						wxT(""), wxT(""), wxT(""), 
						wxT("XML files (*.xml)|*.xml|All files (*.) |*."), 0, this
					);
	canvas->LoadConfigurations(path);
}

//-----------------------------------------------------------------------

void IviewsWindow::OnSelectExpression (wxCommandEvent& event) {
	wxString expr = wxGetTextFromUser(	
						wxT("Put the expression that you want to display"), 
						wxT("Input text"), 
						std2str(selectedText),
						this
					);

	if (!expr.IsEmpty()) {
		selectedText = str2std(expr);
		long depth = GetReferenceDepth(1, 1, 20, this);

		if (depth >= 0 && depth <= 20)
			canvas->DrawGraph(expr, depth);
		else
			wxMessageBox(wxT("Depth must be between 0(unlimited) and 20"), wxT("Error"), wxICON_ERROR);
	}
	
}

//-----------------------------------------------------------------------

void IviewsWindow::OnRemoveVertex (wxCommandEvent& event) 
	{ canvas->RemoveLastVisitedVertex(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnGotoSource (wxCommandEvent& event){ 
	if (canvas->GotoSourceFile())
		this->GetParent()->SetFocus();
}

//-----------------------------------------------------------------------

void IviewsWindow::OnCancelAllDisplayAddOns (wxCommandEvent& event)
	{ canvas->CancelAllDisplayAddOns(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnViewSlotTextualContents (wxCommandEvent& event) 
	{ canvas->ViewSlot(lastMousePos); }

//-----------------------------------------------------------------------

void IviewsWindow::OnCopyVertexAbsRef (wxCommandEvent& event) 
	{ canvas->CopyAbsRefToClipboard(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnRestorePathColours (wxCommandEvent& event) 
	{ canvas->RestorePathColours(); }

//-----------------------------------------------------------------------

void IviewsWindow::OnPathHighlight (wxCommandEvent& event) {
	switch(event.GetId()){
		case VERTEX_HIGHLIGHT_BOTH:
			canvas->HighlightPath(BOTH, highlightInvolvedSlots);
			break;
		case VERTEX_HIGHLIGHT_INCOMING:
			canvas->HighlightPath(INCOMING, highlightInvolvedSlots);
			break;
		case VERTEX_HIGHLIGHT_OUTGOING:
			canvas->HighlightPath(OUTGOING, highlightInvolvedSlots);
			break;
		case VERTEX_HIGHLIGHT_INVOLVED_SLOTS:
			highlightInvolvedSlots = !highlightInvolvedSlots;
			break;

		case VERTEX_HIGHLIGHT_RELATIVE_SLOTS:
			canvas->HighlightAllRelativeSlots();
			break;

		case VERTEX_RESTORE_RELATIVE_SLOTS_COLOURS:
			canvas->RestoreRelativeSlotsColours();
			break;
		default:
			assert(0);
			break;
	};
}

//-----------------------------------------------------------------------

void IviewsWindow::VertexMenu (
		const wxPoint&			pos, 
		const bool				hasContents, 
		const bool				hasMadeInSlot,
		const EdgesParameters & vertexParams,
		const EdgesParameters & layerParams,
		const EdgesParameters & layersParams
	) {
	
	wxMenu		* vertexOptions = CreateVertexMenu(
									hasContents, 
									hasMadeInSlot,
									vertexParams,
									layerParams,
									layersParams
								);
	wxWindow	* vertexWindow	= new wxWindow(this, wxID_ANY, pos);
	
	lastMousePos = pos;
	vertexWindow->PopupMenu(vertexOptions);
	vertexWindow->SetName(wxT(""));
}

//-----------------------------------------------------------------------

void IviewsWindow::OnStartVerticesFromCommonPosition (wxCommandEvent & event) {
	canvas->StartVerticesFromCommonPosition(
				ChengeMenuText (
					MENU_ELEMENT_DRAW_EDGES_FROM_OBJECTS, 
					DRAW_EDGES_FROM_OBJECTS_STRING, 
					DRAW_EDGES_FROM_SLOTS_STRING
				)		
	);
}

//-----------------------------------------------------------------------

void IviewsWindow::EnableNewLoadGraphMenus (bool enable) {
	if (menuBar) {
		menuBar->Enable(MENU_ELEMENT_LOAD_GRAPH,		enable);
		menuBar->Enable(MENU_ELEMENT_NEW_EXPRESSION,	enable);
	}

	if (toolBar) {
		toolBar->EnableTool(MENU_ELEMENT_LOAD_GRAPH,	enable);
		toolBar->EnableTool(MENU_ELEMENT_NEW_EXPRESSION,enable);
	}
}

//-----------------------------------------------------------------------

void IviewsWindow::EnableStartVerticesFromCommonPosition (bool enable) {
	if (menuBar)
		menuBar->Enable(MENU_ELEMENT_DRAW_EDGES_FROM_OBJECTS, enable);
}

//-----------------------------------------------------------------------

void IviewsWindow::EnableMenus (bool enable) {
	if (menuBar)
		for (int menuBarElement = MENU_BAR_ELEMTS_BEGIN + 1; 
			 menuBarElement < MENU_BAR_ELEMTS_END; 
			 ++menuBarElement)
			 menuBar->Enable(menuBarElement, enable);

}

//-----------------------------------------------------------------------

void IviewsWindow::EnableTools (bool enable) {
	if (toolBar){
		toolBar->EnableTool(MENU_ELEMENT_NEW_EXPRESSION,		enable);
		toolBar->EnableTool(MENU_ELEMENT_LOAD_GRAPH,			enable);
		toolBar->EnableTool(MENU_ELEMENT_SAVE_GRAPH,			enable);
		toolBar->EnableTool(TOGGLE_BOOKMARK,					enable);
		toolBar->EnableTool(PREVIOUS_BOOKMARK,					enable);
		toolBar->EnableTool(NEXT_BOOKMARK,						enable);
		toolBar->EnableTool(CLEAR_BOOKMARKS,					enable);
		toolBar->EnableTool(MENU_ELEMENT_ZOOM_IN,				enable);
		toolBar->EnableTool(MENU_ELEMENT_ZOOM_OUT,				enable);
		toolBar->EnableTool(MENU_ELEMENT_LENS_TOOL,				enable);
		toolBar->EnableTool(MENU_ELEMENT_SAVE_GRAPH_AS_IMAGE,	enable);
		toolBar->EnableTool(MENU_ELEMENT_FIT_TO_WINDOW,			enable);
	}
}

//***************************************************
//	Private method API implemenatation


//-----------------------------------------------------------------------

#define ADD_TOOL(TOOL_ID, LABEL, XPM, HELP1, HELP2)	\
	toolBar->AddTool(								\
		TOOL_ID,									\
		wxT(LABEL),									\
		wxBitmap(XPM),								\
		wxNullBitmap, 								\
		wxITEM_NORMAL,								\
		wxT(HELP1),									\
		wxT(HELP2)									\
	)


void IviewsWindow::PopulateToolbar (void) {	
	toolBar = CreateToolBar(wxTB_FLAT | wxTB_HORIZONTAL, TOOLBAR_ID);// | wxTB_TEXT);
	toolBar->SetToolBitmapSize(wxSize(16, 16));	//16 is the size of xpms
    
	ADD_TOOL(MENU_ELEMENT_NEW_EXPRESSION	, "", new_xpm			, "Draw a new graph for an expression", "");
	ADD_TOOL(MENU_ELEMENT_LOAD_GRAPH		, "", load_xpm			, "Load a graph from a xml file", "");
	ADD_TOOL(MENU_ELEMENT_SAVE_GRAPH		, "", save_xpm			, "Save the graph in an xml file", "");
	toolBar->AddSeparator();
	ADD_TOOL(TOGGLE_BOOKMARK				, "", book_toggle_xpm	, "Toggle a bookmark on the current Vertex", "");
	ADD_TOOL(PREVIOUS_BOOKMARK				, "", book_prev_xpm		, "Move to the previous bookmark", "");
	ADD_TOOL(NEXT_BOOKMARK					, "", book_next_xpm		, "Move to the next bookmark", "");
	ADD_TOOL(CLEAR_BOOKMARKS				, "", book_clear_xpm	, "Clear all Bookmarks", "");
	toolBar->AddSeparator();
	ADD_TOOL(MENU_ELEMENT_ZOOM_IN			, "", zoom_in_xpm		, "Zoom in", "");
	ADD_TOOL(MENU_ELEMENT_ZOOM_OUT			, "", zoom_out_xpm		, "Zoom out", "");
	toolBar->AddSeparator();
	ADD_TOOL(MENU_ELEMENT_LENS_TOOL			, "", lens_view_xpm		, "Enable Lens View", "");
	ADD_TOOL(MENU_ELEMENT_SAVE_GRAPH_AS_IMAGE, "", image_xpm		, "Save Graph as an Image", "");
	ADD_TOOL(MENU_ELEMENT_FIT_TO_WINDOW		, "", fit_to_window_xpm	, "Fit to Window", "");
	
	EnableTools(false);
	EnableNewLoadGraphMenus(true);
	toolBar->SetToolSeparation(10);
	toolBar->Realize();
}

//-----------------------------------------------------------------------

bool IviewsWindow::ChengeMenuText (int id, const wxString & oldText, const wxString & newText) {
	wxMenuItem * menu = menuBar->FindItem(id);	
	assert(menu);

	if (menu->GetText() == oldText) {
		menu->SetText(newText);
		return true;
	}
	else {
		menu->SetText(oldText);
		return false;
	}
}

//-----------------------------------------------------------------------

void IviewsWindow::ChengeMenuTextTo (int id, const wxString & text) {
	wxMenuItem * menu = menuBar->FindItem(id);	
	assert(menu);
	menu->SetText(text);
}


//-----------------------------------------------------------------------

void IviewsWindow::PopulateMenuBar (void) {
    menuBar = new wxMenuBar;
    menuBar->Append(CreateGraphMenu(), wxT("&Graph"));
	menuBar->Append(CreateEditMenu(), wxT("&Edit"));
    menuBar->Append(CreateViewMenu(), wxT("&View"));
	
	EnableMenus(false);
	EnableNewLoadGraphMenus(true);
	SetMenuBar(menuBar);	
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateGraphMenu (void) {
    wxMenu * graphMenu = new wxMenu;
    graphMenu->Append(MENU_ELEMENT_NEW_EXPRESSION,			wxT("New &Expression\tCtrl-E"));
	graphMenu->Append(MENU_ELEMENT_SAVE_GRAPH,				wxT("&Save Graph\tCtrl-S"));
	graphMenu->Append(MENU_ELEMENT_LOAD_GRAPH,				wxT("&Load Graph\tCtrl-L"));
	graphMenu->Append(MENU_ELEMENT_SAVE_GRAPH_AS_IMAGE,		wxT("&Save Graph as an Image"));
	graphMenu->Append(MENU_ELEMENT_EXPORT_SUB_MENU,			wxT("Export as"), CreateExportSubMenu());
	graphMenu->Append(MENU_ELEMENT_UNDO,					wxT("Undo Remove\tCtrl-Z") );
    graphMenu->Append(MENU_ELEMENT_REDO,					wxT("Redo Remove\tCtrl-Y") );
	graphMenu->Append(MENU_ELEMENT_STATISTICS,				wxT("Statistics") );
	graphMenu->Append(MENU_ELEMENT_DRAW_EDGES_FROM_OBJECTS,	DRAW_EDGES_FROM_OBJECTS_STRING);
	return graphMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateEditMenu (void) {
	wxMenu * editMenu = new wxMenu;
	editMenu->Append(MENU_ELEMENT_SUBMENU_FIND,			wxT("Find"),	  CreateFindSubMenu());
	editMenu->Append(MENU_ELEMENT_SUBMENU_BOOKMARKS,	wxT("Bookmarks"), CreateBookmarksSubMenu());
	editMenu->Append(MENU_ELEMENT_LENS_TOOL,			wxT("Lens View"));
	editMenu->Append(MENU_ELEMENT_FIT_TO_WINDOW,		wxT("Fit to Window"));

	return editMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateFindSubMenu (void) {
	wxMenu *findMenu = new wxMenu;
	findMenu->Append(FIND,				wxT("&Find \tCtrl-F"));
	findMenu->Append(FIND_NEXT,			wxT("&Find Next\tF3"));
	findMenu->Append(FIND_PREV,			wxT("&Find Prev\tShift-F3"));
	return findMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateExportSubMenu (void) {
	wxMenu *exportMenu = new wxMenu;
	exportMenu->Append(EXPORT_AS_DOT,	wxT("Dot Format"));
	exportMenu->Append(EXPORT_AS_SVG,	wxT("SVG Format"));
	return exportMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateBookmarksSubMenu (void) {
	wxMenu *bookMarkMenu = new wxMenu;
	bookMarkMenu->Append(TOGGLE_BOOKMARK,		wxT("Toggle Bookmark\tCtrl-F2"));
	bookMarkMenu->Append(DISABLE_ALL_BOOKMARKS, wxT("Disable Bookmarks"));
	bookMarkMenu->Append(ENABLE_BOOKMARK,		wxT("Enable Bookmarks"));
	bookMarkMenu->Append(PREVIOUS_BOOKMARK,		wxT("Previous Bookmark\tShift-F2"));
	bookMarkMenu->Append(NEXT_BOOKMARK,			wxT("Next Bookmark\tF2"));
	bookMarkMenu->Append(CLEAR_BOOKMARKS,		wxT("Clear Bookmarks"));
	return bookMarkMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateViewMenu (void) {
	wxMenu * viewMenu = new wxMenu;
    viewMenu->Append(MENU_ELEMENT_ZOOM_IN,					wxT("Zoom In\tCtrl-+"));
    viewMenu->Append(MENU_ELEMENT_ZOOM_OUT,					wxT("Zomm Out\tCtrl--"));
    viewMenu->Append(MENU_ELEMENT_ZOOM_RESTORE,				wxT("Zoom &Normal\tCtrl-N"));
	viewMenu->Append(MENU_ELEMENT_CANCEL_ALL_DISPLAY_ADDONS,wxT("Clear all Display Filters"));

	return viewMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateEdgesSubMenu (
		const EdgesParameters & vertexParams,
		const EdgesParameters & layerParams,
		const EdgesParameters & layersParams
	) {

	wxMenu *edgesMenu = new wxMenu;
	edgesMenu->Append(
		wxID_ANY, 
		wxT("For Vertex"), 
		CreateVertexEdgesSubMenu(vertexParams)
	);

	edgesMenu->Append(
		wxID_ANY, 
		wxT("For Layer"), 
		CreateLayerEdgesSubMenu(layerParams)	
	);

	edgesMenu->Append(
		wxID_ANY, 
		wxT("For Layers"), 
		CreateLayersEdgesSubMenu(layersParams)	
	);
	return edgesMenu;	
}



inline void SetEdgesSubMenuEdgesParameters (
		wxMenu *				menu,
		const EdgesParameters & params,
		int						innerEdgesHideId,
		int						innerEdgesShowId,
		const wxString & 		hideInnerEdgesString,
		const wxString & 		showInnerEdgesString,
		int						incomingEdgesHideId,
		int						incomingEdgesShowId,
		const wxString & 		hideIncomingEdgesString,
		const wxString & 		showIncomingEdgesString,
		int						outgoingEdgesHideId,
		int						outgoingEdgesShowId,
		const wxString & 		hideOutgoingEdgesString,
		const wxString & 		showOutgoingEdgesString,
		int						allEdgesHideId,
		int						allEdgesShowId,
		const wxString & 		hideAllEdgesString,
		const wxString & 		showAllEdgesString
	){
	if (params.HasHiddenInnerEdges())
		menu->Append(innerEdgesShowId, showInnerEdgesString);
	else
		menu->Append(innerEdgesHideId, hideInnerEdgesString);

	if (params.HasHiddenIncomingEdges())
		menu->Append(incomingEdgesShowId, showIncomingEdgesString);
	else
		menu->Append(incomingEdgesHideId, hideIncomingEdgesString);

	if (params.HasHiddenOutgoingEdges())
		menu->Append(outgoingEdgesShowId, showOutgoingEdgesString);
	else
		menu->Append(outgoingEdgesHideId, hideOutgoingEdgesString);

	if (params.HasHiddenAllEdges())
		menu->Append(allEdgesShowId, showAllEdgesString);
	else
		menu->Append(allEdgesHideId, hideAllEdgesString);
}

wxMenu * IviewsWindow::CreateVertexEdgesSubMenu (const EdgesParameters & vertexParams) {
	wxMenu * vertexEdgesMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	SetEdgesSubMenuEdgesParameters (
		vertexEdgesMenu,
		vertexParams,
		HIDE_VERTEX_INNER_EDGES,
		SHOW_VERTEX_INNER_EDGES,
		HIDE_INNER_EDGES_STRING,
		SHOW_INNER_EDGES_STRING,
		HIDE_VERTEX_INCOMING_EDGES,
		SHOW_VERTEX_INCOMING_EDGES,
		HIDE_INCOMING_EDGES_STRING,
		SHOW_INCOMING_EDGES_STRING,		
		HIDE_VERTEX_OUTGOING_EDGES,
		SHOW_VERTEX_OUTGOING_EDGES,
		HIDE_OUTGOING_EDGES_STRING,
		SHOW_OUTGOING_EDGES_STRING,
		HIDE_VERTEX_ALL_EDGES,
		SHOW_VERTEX_ALL_EDGES,
		HIDE_ALL_EDGES_STRING,
		SHOW_ALL_EDGES_STRING
	);
	return vertexEdgesMenu;
}

wxMenu * IviewsWindow::CreateLayerEdgesSubMenu (const EdgesParameters & layerParams) {
	wxMenu * layerEdgesMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	SetEdgesSubMenuEdgesParameters (
		layerEdgesMenu,
		layerParams,
		HIDE_LAYER_INNER_EDGES,
		SHOW_LAYER_INNER_EDGES,
		HIDE_INNER_EDGES_STRING,
		SHOW_INNER_EDGES_STRING,
		HIDE_LAYER_INCOMING_EDGES,
		SHOW_LAYER_INCOMING_EDGES,
		HIDE_INCOMING_EDGES_STRING,
		SHOW_INCOMING_EDGES_STRING,		
		HIDE_LAYER_OUTGOING_EDGES,
		SHOW_LAYER_OUTGOING_EDGES,
		HIDE_OUTGOING_EDGES_STRING,
		SHOW_OUTGOING_EDGES_STRING,
		HIDE_LAYER_ALL_EDGES,
		SHOW_LAYER_ALL_EDGES,
		HIDE_ALL_EDGES_STRING,
		SHOW_ALL_EDGES_STRING
	);
	return layerEdgesMenu;
}

wxMenu * IviewsWindow::CreateLayersEdgesSubMenu (const EdgesParameters & layersParams) {
	wxMenu * layersEdgesMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	SetEdgesSubMenuEdgesParameters (
		layersEdgesMenu,
		layersParams,
		HIDE_LAYERS_INNER_EDGES,
		SHOW_LAYERS_INNER_EDGES,
		HIDE_INNER_EDGES_STRING,
		SHOW_INNER_EDGES_STRING,
		HIDE_LAYERS_INCOMING_EDGES,
		SHOW_LAYERS_INCOMING_EDGES,
		HIDE_INCOMING_EDGES_STRING,
		SHOW_INCOMING_EDGES_STRING,		
		HIDE_LAYERS_OUTGOING_EDGES,
		SHOW_LAYERS_OUTGOING_EDGES,
		HIDE_OUTGOING_EDGES_STRING,
		SHOW_OUTGOING_EDGES_STRING,
		HIDE_LAYERS_ALL_EDGES,
		SHOW_LAYERS_ALL_EDGES,
		HIDE_ALL_EDGES_STRING,
		SHOW_ALL_EDGES_STRING
	);
	return layersEdgesMenu;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreatePathHighlightingSubMenu (void) {
	wxMenu * pathHighlighting = new wxMenu(wxT(""), wxMENU_TEAROFF);
	pathHighlighting->Append(VERTEX_HIGHLIGHT_INCOMING, wxT("Incoming References"));
	pathHighlighting->Append(VERTEX_HIGHLIGHT_OUTGOING, wxT("Outgoing References"));
	pathHighlighting->Append(VERTEX_HIGHLIGHT_BOTH,		wxT("All Refrences"));
	pathHighlighting->AppendCheckItem(VERTEX_HIGHLIGHT_INVOLVED_SLOTS, wxT("Highlight Slots"))->Check(highlightInvolvedSlots);
	pathHighlighting->Append(VERTEX_RESTORE_COLOURS,	wxT("Unhighlight all"));
	
	return pathHighlighting;
}

//-----------------------------------------------------------------------

wxMenu * IviewsWindow::CreateVertexMenu (
		const bool hasContents, 
		const bool hasMadeInSlot,
		const EdgesParameters & vertexParams,
		const EdgesParameters & layerParams,
		const EdgesParameters & layersParams
	) {
	//if (vertexOptions)
	//	return vertexOptions;
	vertexOptions = new wxMenu(wxT(""), wxMENU_TEAROFF);

	vertexOptions->Append(VERTEX_COPY_TEXT, wxT("Copy Absolute Object Reference"));
	vertexOptions->Append(VERTEX_REMOVE, wxT("Remove Vertex (Can Undo)"));
	vertexOptions->Append(VERTEX_SLOT_TEXTUAL_CONTENTS, wxT("View Contents of Focus Slot"));
	
	vertexOptions->Append(VERTEX_GOTO_SOURCE, wxT("Goto Source (Object Creator)"));
	if (!hasMadeInSlot)
		vertexOptions->Check(VERTEX_GOTO_SOURCE, false);

	if (hasContents)
		vertexOptions->Append(VERTEX_HIDE_CONTENTS, wxT("Hide Contents"));
	else	
		vertexOptions->Append(VERTEX_SHOW_CONTENTS, wxT("Show Contents"));
	

	vertexOptions->Append(
		SUBMENU_EDGES, 
		wxT("Configure Edges"), 
		CreateEdgesSubMenu(vertexParams, layerParams, layersParams)
	);
	
	vertexOptions->Append(
		VERTEX_SUBMENU_PATH_HIGHLIGHTING, 
		wxT("&Highlight Paths"), 
		CreatePathHighlightingSubMenu()
	);

	vertexOptions->Append(VERTEX_HIGHLIGHT_RELATIVE_SLOTS, wxT("Highlight all Referrer Slots"));

	return vertexOptions;
}




}	//namespace iviews