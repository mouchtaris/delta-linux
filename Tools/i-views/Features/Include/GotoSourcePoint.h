/**
 *	GotoSourcePoint.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

#ifndef	GOTO_SOURCE_POINT_H
#define GOTO_SOURCE_POINT_H


#include <vector>
#include <string>

#include "RenderingTypes.h"
#include "VertexRenderingInfo.h"


namespace iviews {
class GotoSourcePoint {

public:
	GotoSourcePoint(void);
	bool operator()(VertexRenderingInfo * v);

public:
	typedef std::vector<std::string> StringVector;

	bool ParseMadeinString(const std::string & madeinString);
	
	void Tokenize(
			const std::string&	str,
			StringVector &		tokens,
			const std::string&	delimiters = " "
		);
};
}	//

#endif	//GOTO_SOURCE_POINT_H