/**
 *	Graph.h
 *	
 *	Basic Graph. We use adjacency list theory for the graph implementation.
 *	For each vertex, apart from the outgoing edges, we hold the incoming edges.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010
 */


#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <list>
#include <vector>
#include <assert.h>
#include <iostream>

#include "Vertex.h"
#include "CommonTypes.h"
#include "UndoRedoHelper.h"

namespace iviews {
	namespace graphs {
		class Graph {
		public:
			///////////////////////////////////////////////////
			//	public typedef (s)		
			typedef Vertex::VerticesPtrVec	VerticesPtrVec;
			typedef Vertex::VerticesPtrList	VerticesPtrList;
			
			///---- Constractor (s)
			Graph (const unsigned verticesNum);
			Graph () : edgesNum (0), verticesNum (0), nextVertexId (0) { }
			~Graph ();
			
			///////////////////////////////////////////////////
			//	public methods

			bool					Undo (void);
			bool					Redo (void);

			bool					IsEmpty (void) const;
			bool					VertexExists (const Vertex & v) const;
			bool					EdgeExists (const Vertex & source, const Vertex & target) const;

			bool					IncomingEdgeExists (const Vertex &to, const Vertex &from) const;
			bool					OutgoingEdgeExists (const Vertex &source, const Vertex &target) const;

			id_t 					MaxVertexId (void) const {	return nextVertexId; }	//Returns the largest used node id. 
			int						TotalNumberOfIncomingEdges (const Vertex &v) const;
			int						TotalNumberOfOutgoingEdges (const Vertex &v) const;
			int						TotalNumbersOfEdges (void) const { return edgesNum; }
			int						TotalNumberOfVertices (void) const { return verticesNum; }

			bool					AddEdge (const Vertex & source, const Vertex &target);
			bool					DeleteAllEdgesThatContainsVertex (const Vertex &v);
			bool					DeleteEdge (const Vertex &source, const Vertex &target);

			void					MarkVertexAsDeleted (Vertex &v);
			void					UnmarkVertexAsDeleted (Vertex &v);
			const Vertex &			NewVertex (void);
			const Vertex &			GetVertex (const id_t & id) const;
			const VerticesPtrVec  &	GetAllVertices (void) const { return allVertices; }
			const VerticesPtrList &	GetIncomingEdges (const Vertex & v) const;	
			const VerticesPtrList &	GetOutgoingEdges (const Vertex & v) const;
			
			///////////////////////////////////////////////////
			//	overloaded operator (s)
			friend std::ostream & operator<< (std::ostream &, const Graph &);

		private:
			///////////////////////////////////////////////////
			//	private typedef (s)
			//pair.first	== outgoingEdges;
			//pair.second	== incomingEdges; 
			typedef std::pair<VerticesPtrList, VerticesPtrList>	VerticesListPair;
			
			//pair.first	== source vertex
			//pair.second	== target vertex
			typedef std::pair<const Vertex *, const Vertex *>	Edge;

			typedef std::list<Edge>		EdgeList;
			typedef std::list<EdgeList>	EdgeListList;


			///////////////////////////////////////////////////
			//	private iner Class

			//-----------------------------------------------------------------------
			
			struct GraphTable : public std::vector<VerticesListPair> {
				bool						ContainsVertex (const Vertex & v) const;
				VerticesListPair		&	operator[] (const Vertex & v);
				const VerticesListPair	&	operator[] (const Vertex & v) const;
				VerticesListPair		&	operator[] (const Vertex * v);
				const VerticesListPair	&	operator[] (const Vertex * v) const;
			};

			//-----------------------------------------------------------------------

			class UndoRedoHelperForGraph : public UndoRedoHelper<EdgeList> {
			public: 
				UndoRedoHelperForGraph ();
				void AddInUndoStack (const Edge & edge);
			};

			//-----------------------------------------------------------------------
			
			class AddInUndoStackFunctor : public std::unary_function <Vertex *, void> {
			public: 
				//-----------------------------------------------------------------------

				AddInUndoStackFunctor (
					const Vertex			* source_, 
					UndoRedoHelperForGraph	& undoRedo_
				) : source (source_), undoRedo (undoRedo_) {}

				//-----------------------------------------------------------------------
				
				void operator () (Vertex * target) 
					{ undoRedo.AddInUndoStack (std::make_pair (source, target)); }
			private:
				const Vertex			* source;
				UndoRedoHelperForGraph	& undoRedo;
			};

			///////////////////////////////////////////////////
			//	private methods
			const VerticesPtrList&	GetEdges (const Vertex & v, bool isOutEdge) const;
			bool					EdgeExists (
										const Vertex & source, 
										const Vertex &target, 
										bool isOutEdge
									) const;

			int						GetNumberOfVertices (const Vertex & v, bool isOutEdge) const;
			void					ConditionalEraseVertexFromList (
										VerticesPtrList &	list, 
										const bool			isOutgoingList, 
										const Vertex *		ownerOfList, 
										const Vertex *		toDelete
									);

			bool					DeleteEdgeImplementation (
										const Vertex&	source, 
										const Vertex&	target, 
										const bool &	canAddInUndoStack
									);

			bool					DeleteAllEdgesThatContainsVertexImplementation (const Vertex &v);

			///////////////////////////////////////////////////
			//	private members
			int			edgesNum;
			int			verticesNum;
			unsigned	nextVertexId;
			
			GraphTable		table;	
			VerticesPtrVec		allVertices;
			UndoRedoHelperForGraph	undoRedo;

		};

		//-----------------------------------------------------------------------
		extern std::ostream & operator<< (std::ostream &, const Graph &);
	}	//namespace graphs
}	//namespace iviews

#endif	//GRAPH