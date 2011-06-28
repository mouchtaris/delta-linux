#include "SlotRenderingInfo.h"


namespace iviews {


//-----------------------------------------------------------------------

SlotRenderingInfo::SlotRenderingInfo( 
	const id_t			& id_,
	graphs::Vertex		* source_, 
	dinfo::SlotDebugInfo* slot_,
	Text				* keyText_,		
	Rectangle			* keyRect_,
	Rectangle			* keyExpandButtonRect_,	
	Text				* conntentText_,	
	Rectangle			* contentRect_,
	Rectangle			* contentExpandButtonRect_
) :	id						(id_),
	isHidden				(false),
	source					(source_),
	slot					(slot_),
	keyText					(keyText_),
	contentText				(conntentText_),
	keyRect					(keyRect_),
	keyExpandButtonRect		(keyExpandButtonRect_),
	contentRect				(contentRect_),
	contentExpandButtonRect	(contentExpandButtonRect_)
{
	assert(source);
	assert(slot);
}

//-----------------------------------------------------------------------

SlotRenderingInfo::~SlotRenderingInfo() {
	if (keyText != static_cast<Text *>(0)) {
		delete keyText;
		keyText = static_cast<Text *>(0);
	}

	if (contentText != static_cast<Text *>(0)) {
		delete contentText;
		contentText = static_cast<Text *>(0);
	}

	if (keyRect != static_cast<Rectangle *>(0)) {
		delete keyRect;
		keyRect = static_cast<Rectangle *>(0);
	}

	if (keyExpandButtonRect != static_cast<Rectangle *>(0)) {
		delete keyExpandButtonRect;
		keyExpandButtonRect = static_cast<Rectangle *>(0);
	}

	if (contentRect != static_cast<Rectangle *>(0)) {
		delete contentRect;
		contentRect = static_cast<Rectangle *>(0);
	}

	if (contentExpandButtonRect!= static_cast<Rectangle *>(0)) {
		delete contentExpandButtonRect;
		contentExpandButtonRect = static_cast<Rectangle *>(0);
	}

}

//-----------------------------------------------------------------------

bool SlotRenderingInfo::IsHidden (void) const
	{ return isHidden; }

//-----------------------------------------------------------------------

void SlotRenderingInfo::Hide (bool hide) {
	if (isHidden == hide)
		return;
	isHidden = hide;
	keyText->Show(!hide);
	contentText->Show(!hide);
	keyRect->Show(!hide);
	contentRect->Show(!hide);
	if (keyExpandButtonRect)
		keyExpandButtonRect->Show(!hide);

	if (contentExpandButtonRect)
		contentExpandButtonRect->Show(!hide);
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::SetKeyText(Text * text) { 
	assert(text);	
	keyText = text; 
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::SetContentText(Text * text) { 
	assert(text != static_cast<Text *> (0));
	contentText = text; 
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::SetKeyRectangle(Rectangle * rect) { 
	assert(rect);
	keyRect = rect; 
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::SetContentExpandButtonRectangle(Rectangle * rect){ 
	assert(rect);
	contentExpandButtonRect = rect; 
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::SetKeyExpandButtonRectangle(Rectangle * rect){ 
	assert(rect);
	contentExpandButtonRect = rect; 
}


//-----------------------------------------------------------------------

void SlotRenderingInfo::SetContentRectangle(Rectangle * rect) { 
	assert(rect);
	contentRect = rect; 
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::AddKeyEdgeRenderingInfo(EdgeRenderingInfo * eri) {
	assert(eri);
	keyEdges.push_back(eri);
}

//-----------------------------------------------------------------------

void SlotRenderingInfo::AddContentEdgeRenderingInfo(EdgeRenderingInfo * eri) {
	assert(eri);
	contentEdges.push_back(eri);
}

}	//namespace iviews