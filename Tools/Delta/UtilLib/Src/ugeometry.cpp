// ugeometry.cpp
// Geomety specific utilities.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#include "ugeometry.h"
#include <math.h>

// Clockwise rotation.
//
UTILLIB_FUNC void urotate (float* x, float* y, float angleInRadians, float rx, float ry) {
	*x -= rx;
	*y -= ry;
	float cos_theta = cos(angleInRadians); 
	float sin_theta = sin(angleInRadians); 
	float nx = *x * cos_theta - *y * sin_theta  + rx;
	float ny = *y * cos_theta + *x * sin_theta  + ry;
	*x = nx;
	*y = ny;
}

UTILLIB_FUNC void uscale (float* x, float* y, float factor, float sx, float sy) {
	*x = (*x - sx) * factor + sx;
	*y = (*y - sy) * factor + sy;
}