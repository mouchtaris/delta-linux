#ifndef	STATISTICS_H
#define	STATISTICS_H

#include <list>

#include <wx/grid.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/window.h>

#include "GraphLayers.h"
#include "EdgeRenderingInfo.h"
#include "LayerRenderingInfo.h"
#include "LayersRenderingInfo.h"
#include "VertexRenderingInfo.h"


namespace iviews {
	
class Statistics : public wxFrame {
public:
	
	~Statistics ();
	Statistics (wxWindow * parent);

	
	void			ShowStatistics (LayersRenderingInfo * layers);

private:
	struct LayerStatistics {
		typedef graphs::GraphLayers::Layer Layer;

		Layer		layer;
		unsigned 	numberOfObjects;
		unsigned 	numberOfEdges;
		unsigned 	numberOfBackEdges;
		unsigned 	numberOfFrontEdges;
		unsigned 	numberOfInnerEdges;

		LayerStatistics (void);
	};
	typedef std::list<LayerStatistics> LayerStatisticsList;

	void				OnClose (wxCloseEvent &event);

	void				Init (void);
	void				PrintStatistics (void);
	void				PrintLayersStatistics (void);
	void				PrintLayerStatistics (int initialRow, const LayerStatistics & statistics);
	void				GetStatisticsForLayer (LayerRenderingInfo * layer);
	void				GetStatisticsForVertex (VertexRenderingInfo * vertex);
	wxString			UnsignedToString (unsigned num);

	unsigned			totalNumberOfLayers;
	unsigned			totalNumberOfObjects;
	unsigned			totalNumberOfEdges;	//for each vertex count only outgoing vertices (debug)
	unsigned			totalNumberOfFrontEdges;
	unsigned			totalNumberOfBackEdges;
	unsigned			totalNumberOfInnerEdges;
	
	wxGrid*				grid;
	wxBoxSizer*			topSizer;
	LayersRenderingInfo* layers;
	LayerStatisticsList	layersStatistics;

	DECLARE_EVENT_TABLE()
};



}	//namespace iviews


#endif	//STATISTICS_H