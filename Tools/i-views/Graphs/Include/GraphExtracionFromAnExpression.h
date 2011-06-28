/**
 *	GraphExtracionFromAnExpression.h
 *	
 *	Extract the object Graph rom an expression
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010
 */

#ifndef GRAPH_EXTRACTION_FROM_AN_EXPRESSION_H
#define GRAPH_EXTRACTION_FROM_AN_EXPRESSION_H

#include <string>
#include "DeltaVMFuncReturnTypes.h"

namespace iviews {
	namespace graphs {
		class GraphExtracionFromAnExpression {

		public:
			///////////////////////////////////////////////////
			//	overloaded operator(s)

			//expression must not me an empty string and the depth must be > 0
			ObjectGraphData operator()(const std::string & expression, const int depth) const;
		};
	}	//namespace graphs
}	//namespace iviews

#endif	//GRAPH_EXTRACTION_FROM_AN_EXPRESSION_H