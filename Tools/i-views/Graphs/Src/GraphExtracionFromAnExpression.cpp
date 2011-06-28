/**
 *	GraphExtracionFromAnExpression.cpp
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	March 2010
 */

#include "GraphExtracionFromAnExpression.h"


#include <assert.h>

#include "Call.h"
#include "Common.h"

namespace iviews {

namespace graphs {


ObjectGraphData GraphExtracionFromAnExpression::operator()( const std::string &expression, 
															const int depth) const 
{	
	assert (expression != "" && depth >= 0);
	return ide::Call<ObjectGraphData (const String& expr, int depth, bool directed)>
									 ("GraphExtracionFromAnExpression", "DeltaVM", "EvalObjectGraph")
									 (util::std2str(expression), depth, false);
}



}	//namespace graphs
}	//namespace iviews