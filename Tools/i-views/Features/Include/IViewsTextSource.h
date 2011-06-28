#ifndef IVIEWS_TEXT_SOURCE_H
#define IVIEWS_TEXT_SOURCE_H

#include <map>
#include <vector>
#include <string>
#include <wx/scrolwin.h>

#include "Point.h"
#include "Vertex.h"
#include "DebugInfo.h"
#include "FindAddOn.h"
#include "TextSource.h"
#include "SlotDebugInfo.h"
#include "SlotRenderingInfo.h"
#include "ColourAddOnManager.h"
#include "LayersRenderingInfo.h"
#include "IViewsTextSourceSelectionManager.h"

namespace iviews {

class IViewsTextSource : public frep::TextSource{
public:
	typedef graphs::Vertex		Vertex;
	typedef dinfo::DebugInfo	DebugInfo;
	
	IViewsTextSource(
		wxScrolledWindow	*	owner_,
		Vertex				*	vertex_,
		LayersRenderingInfo *	renderingInfo,
		FindAddOn			*	addOn,
		ColourAddOnManager	*	manager,
		const DebugInfo		&	debugInfo,
		IViewsTextSourceSelectionManager *,
		const double			scaleFactor_ = 1.0
	);

	void						SetLayersRenderingInfo(LayersRenderingInfo *);

	virtual bool				Open (void);
	virtual bool				Close (void);
	virtual bool				IsEof(void) const;
	virtual char				GetCurr (void);
	virtual char				GetNext (void);
	virtual bool				Unput(void);
	virtual bool				Retract(unsigned marker);
	virtual unsigned			Mark (void);
	virtual bool				Unmark (unsigned marked);
	virtual void				UnmarkAll (void);
	virtual void				SetPath(const std::string & path_);
	virtual bool				IsFocused (void);
	virtual void				Focus (void);

	
	virtual const Pos			GetPos (void) const;
	virtual const Pos			GetCurrentFilePos (void);
	
	//eof an den uparxei auto to index
	virtual const char			GetSpecificChar(unsigned col) const;
	virtual const std::string	GetPath(void) const;
	virtual const std::string	GetLineText (const int line) const;
	
	virtual void				Bookmark(const unsigned row, const unsigned col);
	virtual void				Select(
									const int startRow, 
									const int startCol, 
									const int endRow, 
									const int endCol 
								);

	virtual std::string			Replace(
									const int startRow, 
									const int startCol, 
									const int endRow, 
									const int endCol, 
									const std::string with
								);

	void						SetScaleFactor(const double scaleFactor);
private:
	typedef std::vector<char>	CharVec;
	typedef std::vector<std::string>	
								StringVec;
	typedef std::map<unsigned, Pos>		
								LineToPos;	

	typedef SlotRenderingInfo::SlotRenderingInfoPtrList
								SlotRenderingInfoPtrList;
	typedef dinfo::SlotDebugInfo::SlotDebugInfoList	
								SlotDebugInfoList;

	//typedef SlotRenderingInfo::SlotRenderingInfoPtrList SlotRenderingInfoPtrList;

	int							GetSlotIndexFromRow (const int row);
	SlotRenderingInfo *			GetNthSlotRenderingInfoFromList (
									SlotRenderingInfoPtrList &	slots,
									const int					distance
								);
	void						AddText (const std::string & text);
	void						SetScrollBarsToProperPosition (const int slotIndex);
	

	wxScrolledWindow	* 		owner;
	Vertex				* 		vertex;
	LayersRenderingInfo * 		layersRenderingInfo;
	FindAddOn			* 		findAddOn;
	ColourAddOnManager	* 		colourAddOnManager;

	Pos							currPos;	//current coordinates
	Pos							selectedPos;//Where coordinates if we select an area of vertex
	CharVec						entireText;	//entire vertex's text;
	StringVec					lineHolder;	//vector's index map to line, and string is the corresponding text for the line
	LineToPos					markPos;	//Marck coordinates if we call Marck
	unsigned					textPos;	//current pos in entireText
	double						scaleFactor;

	IViewsTextSourceSelectionManager * selectionManager;
	
};

}	//namespace iviews

#endif	//IVIEWS_TEXT_SOURCE_H