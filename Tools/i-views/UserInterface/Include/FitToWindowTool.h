#ifndef FIT_TO_WINDOW_TOOL_H
#define	FIT_TO_WINDOW_TOOL_H


#include "Dimension.h"
#include "IviewsCanvas.h"
#include "RenderingTypes.h"
#include "LayersRenderingInfo.h"

namespace iviews {
	class IviewsCanvas;

	class FitToWindowTool {
	public:
		typedef geo::Dimension<length_t> Dimension;

		bool operator()(
			IviewsCanvas * owner, 
			const Rectangle & layersBoundingBox,
			const Dimension & distanceFromTopLeftCorner
		);

	private:
		bool CanFitToWindo(const Rectangle & bb, const Dimension & distanceFromTopLeftCorner);
		bool IsBiggerThanWindow(const Rectangle & bb, const Dimension & distanceFromTopLeftCorner);
		void FitInBiggerWindow(const Rectangle & bb, const Dimension & distanceFromTopLeftCorner);
		void FitInSmallerWindow(const Rectangle & bb, const Dimension & distanceFromTopLeftCorner);

		IviewsCanvas * owner;
	};

}	//namespace iviews

#endif	//FIT_TO_WINDOW_TOOL_H