/**
 *	ConvertToGraph.h
 *	
 *	Converte an ObjectGraph to Graph
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010
 */

#ifndef CONVERT_TO_GRAPH_H
#define CONVERT_TO_GRAPH_H

#include <map>
#include <string>

#include "Graph.h"
#include "Vertex.h"
#include "DebugInfo.h"
#include "DeltaVMFuncReturnTypes.h"
#include "VertexToDebugInfoMapping.h"


#include <list>
#include <utility>
#include "UndoRedoHelper.h"
namespace iviews {
	namespace graphs {

		class ConvertToGraph {
		public:
			///---- Constractor(s)
			ConvertToGraph (void);


			///////////////////////////////////////////////////
			//	public methods
			
			//caller is responsible to delete the instance
			VertexToDebugInfoMapping * GetDebugInfo (void) const;

			Vertex *	GetStartVertex (void) const;
			bool		HasError (void) const;
			bool		IsSimpleObject (void) const;
			std::string GetDescriptionOfSimpleObject (void) const;
			std::string	GetErrorMessage (void) const;



			///////////////////////////////////////////////////
			//	overloaded operator(s)

			//caller is responsible to delete the instance
			Graph * operator() (const ObjectGraphData &ogd);	


		private:
			///////////////////////////////////////////////////
			//	private typedef(s)
			typedef std::map<std::string, Vertex *> ObjectRefToVertexMapping;


			///////////////////////////////////////////////////
			//	private methods
			void		Init (void);
			Vertex *	GetNewOrExistingVertex (	
							const std::string & refrence, 
							const ObjectGraphMap & objGraphMap
						);
			
			Graph *		ProduceGraphFromCompositeExpression (const ObjectGraphData & ogd);
			
			void		ConditionalCreateAndConectChildrenWithParent (	
							const Vertex & parent, 
							const ObjectNodeList & contents,
							const ObjectGraphMap & objGraphMap
						);

			///////////////////////////////////////////////////
			//	private members
			unsigned	nextSlotId;

			bool		hasError;
			bool		isSimpleObj;
			std::string errorMessage;
			std::string descriptionOfSimpleObj;

			Graph	* graph;
			Vertex	* startVertex;
			
			ObjectGraphMap				objGraphMap;
			ObjectRefToVertexMapping	objToVertMapping;
			VertexToDebugInfoMapping *	vertToDebugInfoMapping;

		};

	}	//namespace graphs

}	//namespace iviews
#endif //CONVERT_TO_GRAPH_H