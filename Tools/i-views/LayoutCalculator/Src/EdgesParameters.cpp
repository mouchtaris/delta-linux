/**
 *	EdgesParameters.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#include "EdgesParameters.h"

namespace iviews {

EdgesParameters::EdgesParameters (void) :
	isAllEdgesHidden		(false),
	isInnerEdgesHidden		(false),
	isIncomingEdgesHidden	(false),
	isOutgoingEdgesHidden	(false)
{}

//-----------------------------------------------------------------------

bool EdgesParameters::HasHiddenAllEdges (void) const
	{ return isAllEdgesHidden; }

void EdgesParameters::HideAllEdges(bool hide) {
	isAllEdgesHidden = hide;
	HideInnerEdges(hide);
	HideIncomingEdges(hide);
	HideOutgoingEdges(hide);
}

//-----------------------------------------------------------------------

bool EdgesParameters::HasHiddenInnerEdges (void) const
	{ return isInnerEdgesHidden; }

void EdgesParameters::HideInnerEdges(bool hide)
	{ isInnerEdgesHidden = hide; }

//-----------------------------------------------------------------------

bool EdgesParameters::HasHiddenIncomingEdges (void) const
	{ return isIncomingEdgesHidden; }

void EdgesParameters::HideIncomingEdges(bool hide)
	{ isIncomingEdgesHidden = hide; }

//-----------------------------------------------------------------------

bool EdgesParameters::HasHiddenOutgoingEdges (void) const
	{ return isOutgoingEdgesHidden; }

void EdgesParameters::HideOutgoingEdges(bool hide)
	{ isOutgoingEdgesHidden = hide; }


}	//namespace iviews 