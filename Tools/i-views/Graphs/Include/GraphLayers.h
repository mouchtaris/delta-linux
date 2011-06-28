/**
 *	GraphExtracionFromAnExpression.h
 *	
  *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010
 */

#ifndef GRAPH_LAYERS_H
#define GRAPH_LAYERS_H

#include <vector>
#include "Graph.h"

namespace iviews {

	namespace graphs {
		class GraphLayers {
		public:
			///////////////////////////////////////////////////
			//	public typedef (s)
			typedef unsigned int Layer;
			typedef Vertex::VerticesPtrList VerticesPtrList;

			///---- Constractor (s)
			GraphLayers (const Graph & basicGraph);
			GraphLayers (const Layer & maxNumOfLayers, const Graph & basicGraph);


			///////////////////////////////////////////////////
			//	public methods
			
			bool					LayerExists (const Layer & l) const;							
			bool					IsEmpty (void) const;										
			bool					SwapVerticesInLayer (
										const Vertex & v1, 
										const Vertex & v2, 
										const Layer & l
									);	
			bool					AddVertexInLayer (const Vertex &, const Layer & l);
			bool					DeleteLastLayer (void);	
			
			bool					VertexExist (const Vertex & v) const;
			bool					VertexExistsInLayer (const Vertex & v, const Layer & l);	
			
			Layer					NewLayer (void);
			unsigned				TotalNumberOfLayers (void) const;				
			unsigned				TotalNumberOfVertices (void) const;
			unsigned				TotalNumberOfVerticesInLayer (const Layer & l) const;
			
			//v1 and v2 to exists in layer l.
			long					GetDistanceOfVerticesInLayer (
										const Vertex & v1,
										const Vertex & v2, 
										const Layer & l
									) const;
			
			Layer					GetLayerThatContainVertex (const Vertex & v) const;
			Layer					GetLastLayer (void) const;
			Layer					GetFirstLayer (void) const;
			bool					HasNextLayer (const Layer & l) const;
			Layer					GetNextLayer (const Layer & l) const;

			const Graph	&			GetGraph (void) const;
			const VerticesPtrList &	GetAllVertices (void) const;
			const VerticesPtrList &	GetAllVerticesInLayer (const Layer & l) const;		

			///////////////////////////////////////////////////
			//	overloaded operator (s)
			friend std::ostream & operator<< (std::ostream & stream, const GraphLayers & gl);
			friend std::ostream & operator<< (std::ostream & stream, const GraphLayers * gl);
		private:
			VerticesPtrList::iterator		GetVertexFromLayer (const Vertex & v, const Layer & l);	
			VerticesPtrList::const_iterator	GetVertexFromLayer (const Vertex & v, const Layer & l) const; 	

			///////////////////////////////////////////////////
			//	typedef (s)
			typedef std::vector<Vertex::VerticesPtrList> LayersTable;

			Layer			nextLayer;		//next layer that will be created
			//unsigned		maxNumOfLayers;	//max number of layers in the graph
			unsigned		verticesNum;	//total number of verticies that graphs contains
			const Graph	&	basicGraph;
			LayersTable		table;
			VerticesPtrList	allVertices;
		};

		//-----------------------------------------------------------------------
		extern std::ostream & operator<< (std::ostream & stream, const GraphLayers & gl);
		extern std::ostream & operator<< (std::ostream & stream, const GraphLayers * gl);

	}	//namespace graphs

}	//namespace iviews
#endif