/**
 *	GraphCoordinator.h
 *	
 *	Coordinate the processes for the graph and grapLayers' construction
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010
 */

#ifndef GRAPH_COORDINATOR_H
#define GRAPH_COORDINATOR_H

#include <list>
#include <string>
#include <utility>
#include "Graph.h"
#include "DebugInfo.h"
#include "GraphLayers.h"
#include "UndoRedoHelper.h"
#include "ConvertToGraph.h"
#include "LayerComputation.h"
#include "GraphExtracionFromAnExpression.h"

namespace iviews {

	namespace graphs {

		class GraphCoordinator {
		public:
			///////////////////////////////////
			//	typedef(s)

			///---- Constractor(s)
			GraphCoordinator ();
			~GraphCoordinator ();

			///////////////////////////////////////////////////
			//	overloaded operator(s)
			GraphLayers * 				operator() (const std::string & expression, const int depth);
			GraphLayers * 				ExcludeVertex (Vertex & v);
			GraphLayers * 				operator() (
											long depth,
											Vertex * startVertex, 
											Graph * graph, 
											VertexToDebugInfoMapping * mapping
										);


			GraphLayers	*				Undo (void);
			GraphLayers	*				Redo (void);
			bool						CanUndo (void);
			bool						CanRedo (void);

			void						Clean (void);
			void						DeleteGraphLayers (void);

			bool						HasError (void) const;
			const std::string &			GetErrorMessage (void) const;

			bool						GraphIsSimpleObje (void) const;
			const std::string &			GetDescriptionOfSimpleObject (void) const;

			long						GetDepth (void) const;
			Graph			*			GetGraph (void) const;
			Vertex			*			GetStartVertex (void) const;
			GraphLayers		*			GetGraphLayers (void) const;
			VertexToDebugInfoMapping *	GetVertexToDebugInfoMapping (void) const;


		private:
			///////////////////////////////////////////////////
			//	private typedef(s)
			typedef dinfo::DebugInfo				DebugInfo;
			typedef Vertex::VerticesPtrList			VerticesPtrList;
			typedef std::pair<Vertex *, DebugInfo>	VertexToDebugInfo;
			typedef std::list<VertexToDebugInfo>	VertexToDebugInfoList;


			///////////////////////////////////////////////////
			//	private iner Class

			class UndoRedoHelperForGraphCoordinator : public UndoRedoHelper<VerticesPtrList> {
			public:
				UndoRedoHelperForGraphCoordinator (void);
				void AddInUndoStack (Vertex *);
			};


			///////////////////////////////////////////////////
			//	private methods
			void SetError (const std::string & message);
			void SetSimpleObject (const std::string & description);
			void ClearVertexToDebugInfoMappingFromUnnessecaryVertices (void);


			///////////////////////////////////////////////////
			//	private members
			int			depth;
			bool		hasError;
			bool		isSimpleObject;
			std::string	errorMsg;
			std::string simpleObjectDescription;

			Vertex					 *	startVertex;
			Graph					 *	graph;
			GraphLayers				 *	graphLayers;
			VertexToDebugInfoMapping *	mapping;
			

			ConvertToGraph						converter;
			LayerComputation					layerComputation;
			GraphExtracionFromAnExpression		objGraphCreator;

			UndoRedoHelperForGraphCoordinator	undoRedo;
		};

	}	//namespace graphs
}	//namespace iviews

#endif	//GRAPH_COORDINATOR_H