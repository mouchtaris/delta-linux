#include "LayoutParamsValidator.h"



namespace iviews {
	
//-----------------------------------------------------------------------
	
LayoutParamsValidator::LayoutParamsValidator (void) :
	hasLayersDistanceChanged				(false),
	hasLayersAlignmentChanged				(false),
	hasDrawLayersRectChanged				(false),
	hasDrawVertexWithFullContentsChanged	(false)
{}

//-----------------------------------------------------------------------

bool LayoutParamsValidator::IsLayoutParamsInvalid (
		const LayoutParams & oldParams, 
		const LayoutParams & newParams
	) {
	const LayerLayoutParams & oldLayerParams = oldParams.layerParams;
	const LayerLayoutParams & newLayerParams = newParams.layerParams;
		
	needsRecalculation						= false;
	hasLayersDistanceChanged				= oldLayerParams.layersDistance != newLayerParams.layersDistance;
	hasLayersAlignmentChanged				= oldLayerParams.layersAlignment != newLayerParams.layersAlignment;
	hasDrawLayersRectChanged				= oldLayerParams.drawLayersRect != newLayerParams.drawLayersRect;
	hasDrawVertexWithFullContentsChanged	= oldLayerParams.drawVertexWithFullContents != newLayerParams.drawVertexWithFullContents;

	if (oldParams.vertexParams != newParams.vertexParams)
		needsRecalculation = true;

	if (!needsRecalculation)
		needsRecalculation =	
			oldLayerParams.layersDistance				!= newLayerParams.layersDistance			||		
			oldLayerParams.verticesDistance				!= newLayerParams.verticesDistance			||
			oldLayerParams.innerEdgesDistance			!= newLayerParams.innerEdgesDistance		||
			oldLayerParams.distanceFromLayer			!= newLayerParams.distanceFromLayer			||
			oldLayerParams.distanceFromTopLeftCorner	!= newLayerParams.distanceFromTopLeftCorner; 

	return	needsRecalculation						||
			hasLayersDistanceChanged				||
			hasLayersAlignmentChanged				||
			hasDrawLayersRectChanged				||
			hasDrawVertexWithFullContentsChanged;

}

//-----------------------------------------------------------------------

bool LayoutParamsValidator::NeedsRecalculation (void) const 
	{ return needsRecalculation; }

//-----------------------------------------------------------------------

bool LayoutParamsValidator::HasLayersDistanceChanged (void) const
	{ return hasLayersDistanceChanged; }

//-----------------------------------------------------------------------

bool LayoutParamsValidator::HasLayersAlignmentChanged (void) const
	{ return hasLayersAlignmentChanged; }

//-----------------------------------------------------------------------

bool LayoutParamsValidator::HasDrawLayersRectChanged (void) const
	{ return hasDrawLayersRectChanged; }

//-----------------------------------------------------------------------

bool LayoutParamsValidator::HasDrawVertexWithFullContentsChanged (void) const
	{ return hasDrawVertexWithFullContentsChanged; }



}	//namespace iviews 