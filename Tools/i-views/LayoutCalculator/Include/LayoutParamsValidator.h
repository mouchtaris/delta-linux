#ifndef LAYOUT_PARAMS_VALIDATOR_H
#define	LAYOUT_PARAMS_VALIDATOR_H


#include "LayoutParams.h"


namespace iviews {


class LayoutParamsValidator {
public:
	~LayoutParamsValidator(){}
	LayoutParamsValidator (void);

	bool IsLayoutParamsInvalid (const LayoutParams & oldParams, const LayoutParams & newParams);

	bool NeedsRecalculation (void) const;
	bool HasLayersDistanceChanged (void) const;
	bool HasLayersAlignmentChanged (void) const;
	bool HasDrawLayersRectChanged (void) const;
	bool HasDrawVertexWithFullContentsChanged (void) const;

private:
	bool needsRecalculation;
	bool hasLayersDistanceChanged;
	bool hasLayersAlignmentChanged;
	bool hasDrawLayersRectChanged;
	bool hasDrawVertexWithFullContentsChanged;

};


}	//namespace iviews 


#endif //LAYOUT_PARAMS_VALIDATOR_H