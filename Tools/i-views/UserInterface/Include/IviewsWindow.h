//july 2010
#ifndef IVIEWS_WINDOW_H
#define IVIEWS_WINDOW_H


#include <string>


#include <wx/dc.h>
#include <wx/font.h>
#include <wx/frame.h>
#include <wx/event.h>
#include <wx/window.h>
#include <wx/string.h>
#include <wx/toolbar.h>
#include <wx/dcclient.h>

#include "Statistics.h"
#include "EdgeHiding.h"
#include "IviewsCanvas.h"
#include "LayoutParams.h"
#include "RenderingParams.h"
#include "EdgesParameters.h"

namespace iviews {

class IviewsWindow : public wxFrame {
public:
	IviewsWindow (
		wxWindow	* parent, 
		const wxFont			& font, 
		const LayoutParams		& lparams,  
		const RenderingParams	& rparams
	);
	~IviewsWindow ();



	void			 UpdateParams (			
						const wxFont			& font, 
						const LayoutParams		& lparams,  
						const RenderingParams	& rparams
					);

	bool			IsFocused (void) const;
	void 			OnSetFocus (wxFocusEvent &event);
	void 			OnKillFocus (wxFocusEvent &event);
	void 			OnQuit (wxCommandEvent& event);
	void 			OnCancel (wxKeyEvent& event);
	void 			OnMouseEnternInToolbar (wxCommandEvent& event);
	void 			OnZoom (wxCommandEvent &event);
	void 			OnLensTool (wxCommandEvent& event);
	void 			OnFitToWindo (wxCommandEvent& event);
	void 			OnFind (wxCommandEvent& event);
	void			OnHideShowEdges (wxCommandEvent& event);
	void 			OnUndoRedo (wxCommandEvent& event);
	void 			OnBookmarks (wxCommandEvent& event);
	void 			OnMouseWheel (wxMouseEvent & event);
	void 			OnSelectExpression (wxCommandEvent& event);
	void 			OnSaveGraph (wxCommandEvent& event);
	void 			OnLoadGraph (wxCommandEvent& event);
	void 			OnSaveGraphAsImage (wxCommandEvent& event);
	void 			OnLoadConfigurations (wxCommandEvent& event);
	void 			OnCancelAllDisplayAddOns (wxCommandEvent& event);
	void 			OnShowContents (wxCommandEvent& event);
	void 			OnHideContents (wxCommandEvent& event);
	void 			OnGotoSource (wxCommandEvent& event);
	void 			OnRemoveVertex (wxCommandEvent& event);
	void 			OnCopyVertexAbsRef (wxCommandEvent& event);
	void 			OnRestorePathColours (wxCommandEvent& event);
	void 			OnPathHighlight (wxCommandEvent& event);
	void 			OnViewSlotTextualContents (wxCommandEvent& event);
	void 			OnUndo (wxCommandEvent& event);
	void 			OnRedo (wxCommandEvent& event);
	void			OnStatistics (wxCommandEvent & event);
	void 			OnRightDown (wxMouseEvent & event);
	void			OnStartVerticesFromCommonPosition (wxCommandEvent & event);
	void			EnableStartVerticesFromCommonPosition (bool enable);
	void			EnableNewLoadGraphMenus (bool enable);
	void			ExportGraph (wxCommandEvent & event);
	void 			VertexMenu (
						const wxPoint & pos, 
						const bool hasContents, 
						const bool hasMadeInSlot,
						const EdgesParameters & vertexParams,
						const EdgesParameters & layerParams,
						const EdgesParameters & layersParams
					);
	void 			EnableMenus (bool enable = true);
	void 			EnableTools (bool enable = true);
	void 			SetSelectedText (const std::string & text) { selectedText = text; }
	


private:

					//if true was with oldTesx

	bool			ChengeMenuText (int id, const wxString & oldText, const wxString & newText);
	void			ChengeMenuTextTo (int id, const wxString & text);
	void			PopulateMenuBar (void);
	void			PopulateToolbar (void);
	wxMenu *		CreateVertexMenu (
						const bool hasContents, 
						const bool hasMadeInSlot,
						const EdgesParameters & vertexParams,
						const EdgesParameters & layerParams,
						const EdgesParameters & layersParams					
					); 
	wxMenu * 		CreateGraphMenu (void);
	wxMenu * 		CreateEditMenu (void);
	wxMenu * 		CreateFindSubMenu (void);
	wxMenu * 		CreateBookmarksSubMenu (void);
	wxMenu *		CreateExportSubMenu (void);
	wxMenu * 		CreateViewMenu (void);
	
	wxMenu *		CreateVertexEdgesSubMenu (const EdgesParameters & vertexParams);
	wxMenu *		CreateLayerEdgesSubMenu (const EdgesParameters & layerParams); 
	wxMenu *		CreateLayersEdgesSubMenu (const EdgesParameters & layersParams); 

	wxMenu * 		CreateEdgesSubMenu (
						const EdgesParameters & vertexParams,
						const EdgesParameters & layerParams,
						const EdgesParameters & layersParams
					);

	wxMenu * 		CreatePathHighlightingSubMenu (void);
	
	//true when hide
	bool			HideOrShowEdges (
						const int									id,
						const wxString&								hideString,
						const wxString&								showString,
						const EdgeHiding::EdgeHidingPreferences&	hidePref,
						const EdgeHiding::EdgeHidingPreferences&	showPref
					);


	bool			isFocused;
	bool			highlightInvolvedSlots;
	std::string		selectedText;
	wxPoint			lastMousePos;
	wxMenuBar	 *	menuBar;
	wxToolBar	 *	toolBar;
	wxMenu		 *  vertexOptions;
	IviewsCanvas *	canvas;
	Statistics*		statistics;




	// any class wishing to process wxWidgets events must use this macro
	DECLARE_EVENT_TABLE()

};

}	//namespace iviews 



#endif //IVIEWS_WINDOW_H
