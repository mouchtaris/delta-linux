/**
 *	IViewsTextSource.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Octomber 2010
 */

#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif

#include <assert.h>
#include <iterator>
#include <algorithm>
#include <functional>

#include "ColourAddOnData.h"
#include "IViewsTextSource.h"


namespace iviews {
//-----------------------------------------------------------------------

static bool IsOdd(const int num) 
	{ return (num % 2) != 0; }

//-----------------------------------------------------------------------

static bool IsEven(const int num)
	{ return !IsOdd(num); }


//******************************************
//	public method API implementation


//-----------------------------------------------------------------------

IViewsTextSource::IViewsTextSource (
	wxScrolledWindow	* 	owner_,
	Vertex				* 	vertex_,
	LayersRenderingInfo * 	renderingInfo,
	FindAddOn			* 	addOn,
	ColourAddOnManager	* 	manager,
	const DebugInfo		& 	debugInfo,
	IViewsTextSourceSelectionManager * selectionManager_,
	const double			scaleFactor_
) : TextSource(),
	owner(owner_),
	vertex(vertex_),
	layersRenderingInfo(renderingInfo),
	findAddOn(addOn),
	colourAddOnManager(manager),
	entireText(),
	lineHolder(),
	markPos(),
	textPos(0),
	scaleFactor(scaleFactor_),
	selectionManager(selectionManager_)
{
	assert(addOn);
	currPos.row		= 0;
	currPos.col		= 0;
	selectedPos.row = 0;
	selectedPos.col = 0;

	AddText(path = debugInfo.GetAbsolutRef());

	const SlotDebugInfoList & slots = debugInfo.GetAllSlots();
	SlotDebugInfoList::const_iterator slot;

	for(slot = slots.begin(); slot != slots.end(); ++slot) {
		entireText.push_back('\n');
		AddText((*slot).GetKeyString());
		
		entireText.push_back('\n');
		AddText((*slot).GetContentString());
	}
}

//-----------------------------------------------------------------------

void IViewsTextSource::SetLayersRenderingInfo (LayersRenderingInfo * renInfo) {
	assert(renInfo);
	layersRenderingInfo = renInfo;
}

//-----------------------------------------------------------------------

bool IViewsTextSource::Open (void) 
	{ return true; }

//-----------------------------------------------------------------------

bool IViewsTextSource::Close (void)
	{ return true; }

//-----------------------------------------------------------------------

bool IViewsTextSource::IsEof (void) const {
	assert(textPos <= entireText.size());
	return textPos == entireText.size();
}

//-----------------------------------------------------------------------

char IViewsTextSource::GetCurr (void) {
	if (!IsEof()) {
		char c = entireText[textPos];

		if ( c == '\n' )
			currPos.row++;
		currPos.col = textPos++;		
		return c;
	}
	return EOF; 
}

//-----------------------------------------------------------------------

char IViewsTextSource::GetNext (void){
	if ( textPos < entireText.size() - 1 ){	
		char c = entireText[++textPos];
		if (entireText[textPos - 1] == '\n' )	//an autos pou umatsan \n tote au3anoume thn shra
			currPos.row++;
		currPos.col = textPos;		
		return c;
	}
	return EOF; 
}

//-----------------------------------------------------------------------

bool IViewsTextSource::Unput (void){
	if (textPos > 0){
		currPos.col = --textPos - 1;

		if (entireText[currPos.col] == '\n')
			currPos.row--;

		return true;
	}
	return false; 
}

//-----------------------------------------------------------------------

bool IViewsTextSource::Retract (unsigned marker){
	if (markPos.find(marker) != markPos.end()){
		textPos		= marker;
		currPos.row	= markPos[marker].row;
		currPos.col	= markPos[marker].col;
		Unmark(marker);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------

unsigned IViewsTextSource::Mark (void) {
	markPos[textPos].row = currPos.row;
	markPos[textPos].col = currPos.col;
	return textPos;
}

//-----------------------------------------------------------------------

bool IViewsTextSource::Unmark (unsigned marked)
	{ return markPos.erase(marked) == 1; }

//-----------------------------------------------------------------------

void IViewsTextSource::UnmarkAll (void)
	{ markPos.clear(); }


//-----------------------------------------------------------------------

void IViewsTextSource::SetPath (const std::string & path_)
	{}

//-----------------------------------------------------------------------

bool IViewsTextSource::IsFocused (void) 
	{ return selectionManager->IsFocused(path); }

//-----------------------------------------------------------------------

void IViewsTextSource::Focus (void)
	{  selectionManager->Focus(path); }

//-----------------------------------------------------------------------

const IViewsTextSource::Pos IViewsTextSource::GetPos (void) const 
	{ return currPos; }

//-----------------------------------------------------------------------
 
const IViewsTextSource::Pos IViewsTextSource::GetCurrentFilePos (void) 
	{ return selectedPos; }

//-----------------------------------------------------------------------
//eof an den uparxei auto to index
const char IViewsTextSource::GetSpecificChar (unsigned col) const
	{ return (col >= entireText.size())? EOF: entireText[col]; }

//-----------------------------------------------------------------------
const std::string IViewsTextSource::GetPath (void) const 
	{ return path; }

//-----------------------------------------------------------------------
const std::string IViewsTextSource::GetLineText (const int line) const{
	assert(line < lineHolder.size());
	return lineHolder[line];
}

//-----------------------------------------------------------------------
void IViewsTextSource::Bookmark (const unsigned row, const unsigned col)
	{}

//-----------------------------------------------------------------------
/*
Sto FindAddOn 8a tou dinoume ta data pou xriazetai gia na kanei select dil:
1) to graph::vertex pou 8a ya3ei na brei se poio VertexRenderingInfo antistixei
an den to brei tote malon 8a exei sbistei apo ton grapho ara xestikame.

2) se poio slot atnistixei dil sto slot 0, 1, 2 (-1 an einai to absolute reference)

3) enan bool isKey;

Emeis 3eroume oti sthn prwth grammh tou lineHolder exoume to absRef,
kai meta ana 2 grammes antistixizontai se ena slot.
Oi grammes prepei na einai panta monos ari8mos. (Mia grammhs gia to abs kai meta ana duo gia
ka8e slot opos upame kai poio panw).

An einai monh grammh exoume key, an einai zigh exoume content
*/
void IViewsTextSource::Select (
		const int startRow, 
		const int startCol, 
		const int endRow, 
		const int endCol 
	){
	assert(startRow == endRow);	
	Focus();
	int index		= 0;
	selectedPos.row = endRow;
	selectedPos.col = endCol + 1;
	FindAddOnData * addOnData;
	

	if (startRow == 0)
		addOnData = new FindAddOnData(vertex, false, index = -1);
	else 
		addOnData = new FindAddOnData(
							vertex, 
							IsOdd(startRow), 
							index = GetSlotIndexFromRow(startRow)
						);

	if (colourAddOnManager->AddOrRemoveConditionalAddOn(
			findAddOn,
			addOnData,
			layersRenderingInfo
		 )
	) {	
		SetScrollBarsToProperPosition(index);
		owner->Refresh();
	}
}

//-----------------------------------------------------------------------

std::string	IViewsTextSource::Replace (
		const int startRow, 
		const int startCol, 
		const int endRow, 
		const int endCol, 
		const std::string with
	) 
	{ assert("Does not support replace action"); return ""; }

//-----------------------------------------------------------------------

void IViewsTextSource::SetScaleFactor (const double scaleFactor_)
	{ scaleFactor = scaleFactor_; }


//***********************************************
// private methond API implementation

//-----------------------------------------------------------------------

int IViewsTextSource::GetSlotIndexFromRow (const int row) { 
	assert(row > 0);
	return IsEven(row) ? (row/2)-1 : row/2; 
}

SlotRenderingInfo * IViewsTextSource::GetNthSlotRenderingInfoFromList(
		SlotRenderingInfoPtrList &	slots,
		const int					distance
	){
	assert(distance >= 0 && distance < slots.size());

	SlotRenderingInfoPtrList::iterator i = slots.begin();
	std::advance(i, distance);

	return *i;
}


//-----------------------------------------------------------------------

void IViewsTextSource::AddText (const std::string & text) {
	std::copy(							
		text.begin(),					
		text.end(),						
		std::back_inserter(entireText)	
	);
	lineHolder.push_back(text);
}

//-----------------------------------------------------------------------

void IViewsTextSource::SetScrollBarsToProperPosition (const int slotIndex) {
	VertexRenderingInfo * v;
	if (layersRenderingInfo && (v = layersRenderingInfo->GetVertexRenderingInfo(vertex))) {
		geo::Point<coord_t> pos;
		
		if (slotIndex == -1 || !v->HasFullContents() || v->HasHiddenContents())	//we have text in abs Ref or we do not hav contents
			pos = v->GetPosition();
		else {
			SlotRenderingInfo * slot = GetNthSlotRenderingInfoFromList(
											v->GetAllSlostsRenderingInfo(),
											slotIndex
										);
			assert(slot);
			pos = slot->GetKeyRectangle()->GetPosition();
		}

		int x_step = 0, y_step = 0;
		owner->GetScrollPixelsPerUnit(&x_step, &y_step);
		x_step != 0 ? x_step = (pos.GetX() * scaleFactor) / x_step : x_step = pos.GetX() * scaleFactor;
		y_step != 0 ? y_step = (pos.GetY() * scaleFactor) / y_step : y_step = pos.GetY() * scaleFactor;

		owner->Scroll(x_step, y_step);
	}
}

}	//namespace iviews 