/**
 *	Statistics.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	December 2010
 */

#include <assert.h>

#include "Statistics.h"
#include "RenderingInfoAlgorithmsMacros.h"


#define INITIAL_NUMBER_OF_ROWS	100
#define INITIAL_NUMBER_OF_COLS	100

namespace iviews {

BEGIN_EVENT_TABLE(Statistics, wxFrame)
	EVT_CLOSE (Statistics::OnClose)
END_EVENT_TABLE()


//-----------------------------------------------------------------------

Statistics::Statistics(wxWindow * parent) :
	wxFrame(
		parent,
		wxID_ANY,
		wxT("i-views (Statistics)"),
		wxDefaultPosition,
        wxDefaultSize
	){
	grid = new wxGrid(
				this,
				wxID_ANY,
				wxPoint(0, 0),
				wxSize(400, 300)
			);

	grid->CreateGrid(0, 0);
    grid->AppendRows(INITIAL_NUMBER_OF_ROWS);
    grid->AppendCols(INITIAL_NUMBER_OF_COLS);
	grid->SetReadOnly(0, 0);
	grid->EnableEditing(false);

	topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add(grid, 1, wxEXPAND);

    SetAutoLayout(true);
    SetSizer(topSizer);
    topSizer->Fit(this);
    
	Centre();
}

//-----------------------------------------------------------------------

Statistics::~Statistics() {
	
}

//-----------------------------------------------------------------------

void Statistics::Init (void) {
	totalNumberOfLayers		= 0;
	totalNumberOfObjects	= 0;
	totalNumberOfEdges		= 0;	
	totalNumberOfFrontEdges	= 0;
	totalNumberOfBackEdges	= 0;
	totalNumberOfInnerEdges	= 0;
	
	layersStatistics.clear();

	grid->ClearGrid();

	int rows = grid->GetNumberRows();
	if (rows > INITIAL_NUMBER_OF_ROWS)
		grid->DeleteRows(INITIAL_NUMBER_OF_ROWS, rows - INITIAL_NUMBER_OF_ROWS);
	
	assert(grid->GetNumberRows() == INITIAL_NUMBER_OF_ROWS);
}

//-----------------------------------------------------------------------

void Statistics::ShowStatistics(LayersRenderingInfo * layers_) {
	assert(layers_);
	layers = layers_;

	Init();
	totalNumberOfLayers = layers->GetLayersRenderingInfo().size();
	FOR_ALL_LAYERS_CONST (layer, layers->GetLayersRenderingInfo())
		GetStatisticsForLayer((*layer));

	PrintStatistics();
	Show(true);
}

//-----------------------------------------------------------------------

void Statistics::PrintStatistics (void) {
	PrintLayersStatistics();

	grid->SetCellValue(0, 0, wxT("Total Number of Layers"));
	grid->SetCellValue(0, 1, UnsignedToString(totalNumberOfLayers));
	grid->SetCellValue(1, 0, wxT("Total Number of Objects"));
	grid->SetCellValue(1, 1, UnsignedToString(totalNumberOfObjects));
	grid->SetCellValue(2, 0, wxT("Total Number of Edges"));
	grid->SetCellValue(2, 1, UnsignedToString(totalNumberOfEdges));
	grid->SetCellValue(3, 0, wxT("Number of Front Edges"));
	grid->SetCellValue(3, 1, UnsignedToString(totalNumberOfFrontEdges));
	grid->SetCellValue(4, 0, wxT("Number of Back Edges"));
	grid->SetCellValue(4, 1, UnsignedToString(totalNumberOfBackEdges));
	grid->SetCellValue(5, 0, wxT("Number of Inner Edges"));
	grid->SetCellValue(5, 1, UnsignedToString(totalNumberOfInnerEdges));

	grid->AutoSizeColumn(0, true);
}

//-----------------------------------------------------------------------

void Statistics::PrintLayersStatistics (void) {
	LayerStatisticsList::iterator layerS;

	int initialRow = 10;
	for (layerS = layersStatistics.begin(); layerS != layersStatistics.end(); ++layerS) {
		PrintLayerStatistics(initialRow, *layerS);
		initialRow += 6;
	}
}

//-----------------------------------------------------------------------

void Statistics::PrintLayerStatistics (int startRow, const LayerStatistics & statistics) {
	if (grid->GetNumberRows() < startRow + 6)
		grid->AppendRows(INITIAL_NUMBER_OF_ROWS);

	grid->SetCellValue(startRow, 0, wxT("Number of Object in Layer: ") + UnsignedToString(statistics.layer));	//label
	grid->SetCellBackgroundColour(startRow, 0, wxColour(255, 0, 0));

	grid->SetCellValue(startRow++	, 1, UnsignedToString(statistics.numberOfObjects));		//value
	grid->SetCellValue(startRow		, 0, wxT("Number of Edges"));							//label
	grid->SetCellValue(startRow++	, 1, UnsignedToString(statistics.numberOfEdges));		//value
	grid->SetCellValue(startRow		, 0, wxT("Number of Front Edges"));						//label
	grid->SetCellValue(startRow++	, 1, UnsignedToString(statistics.numberOfFrontEdges));	//value
	grid->SetCellValue(startRow		, 0, wxT("Number of Back Edges"));						//label
	grid->SetCellValue(startRow++	, 1, UnsignedToString(statistics.numberOfBackEdges));	//value
	grid->SetCellValue(startRow		, 0, wxT("Number of Inner Edges"));						//label
	grid->SetCellValue(startRow++	, 1, UnsignedToString(statistics.numberOfInnerEdges));	//value

	totalNumberOfObjects	+= statistics.numberOfObjects;
	totalNumberOfEdges		+= statistics.numberOfEdges;
	totalNumberOfFrontEdges += statistics.numberOfFrontEdges;
	totalNumberOfBackEdges	+= statistics.numberOfBackEdges;
	totalNumberOfInnerEdges	+= statistics.numberOfInnerEdges;

}

//-----------------------------------------------------------------------

void Statistics::GetStatisticsForLayer (LayerRenderingInfo * layer) {
	assert(layer);
	layersStatistics.push_back(LayerStatistics());
	layersStatistics.back().layer					= layer->GetLayer();
	layersStatistics.back().numberOfObjects			= layer->GetVerticesRenderingInfo().size();

	FOR_ALL_VERTICES_CONST(vertex, layer->GetVerticesRenderingInfo())
		GetStatisticsForVertex(*vertex);
}

//-----------------------------------------------------------------------

void Statistics::GetStatisticsForVertex (VertexRenderingInfo * vertex) {
	assert(vertex && layersStatistics.size() != 0);
	typedef EdgeRenderingInfo::EdgeRenderingInfoPtrList EdgeRenderingInfoPtrList;

	LayerStatistics & statistics = layersStatistics.back();
	const EdgeRenderingInfoPtrList & edges = vertex->GetAllOutgoingEdgesRenderingInfo();

	statistics.numberOfEdges += edges.size();

	FOR_ALL_EDGES_CONST(edge, edges)
		if ((*edge)->IsInnerEdge())
			++statistics.numberOfInnerEdges;
		else
		if ((*edge)->IsFrontEdge())
			++statistics.numberOfFrontEdges;
		else
			++statistics.numberOfBackEdges;
}

//-----------------------------------------------------------------------

wxString Statistics::UnsignedToString (unsigned num) 
	{ return wxString::Format(wxT("%d"), num); }


//-----------------------------------------------------------------------

void Statistics::OnClose (wxCloseEvent &event) 
	{ Show(false); }

//-----------------------------------------------------------------------

Statistics::LayerStatistics::LayerStatistics (void) :
	layer				(0),
	numberOfObjects		(0),
	numberOfEdges		(0),
	numberOfBackEdges	(0),
	numberOfFrontEdges	(0),
	numberOfInnerEdges	(0)
{}



}	//namespace iviews