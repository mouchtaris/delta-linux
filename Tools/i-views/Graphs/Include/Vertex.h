/**
 *	Vertex.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	March 2010
 */

#ifndef VERTEX_H
#define VERTEX_H

#include <set>
#include <list>
#include <vector>
#include <string>
#include <utility>
#include <assert.h>
#include <functional>
#include "Functional.h"
#include "CommonTypes.h"

namespace iviews {
	namespace graphs {
		class Vertex {
		public:
			///////////////////////////////////////////////////
			//	Functors

			//-----------------------------------------------------------------------
			
			struct VertexComparatorFunctor : public std::binary_function<const Vertex *, const Vertex *, bool>{
				bool operator() (const Vertex * v1, const Vertex * v2) const
					{	return v1->Id() < v2->Id();	}
			};
			
			//-----------------------------------------------------------------------
			
			struct VertexEqualityFunctor : public std::unary_function<const Vertex *, bool> {
				const Vertex & ver;
				VertexEqualityFunctor(const Vertex & v) : ver(v)
					{}

				bool operator() (const Vertex * v) const {
					assert (v != static_cast<Vertex *>(0));
					return ver == *v;
				}
			};

			///////////////////////////////////////////////////
			//	public typedef(s)

			typedef std::list<Vertex *>		VerticesPtrList;
			typedef std::vector<Vertex *>	VerticesPtrVec;
			
			typedef std::set<	graphs::Vertex *, 
								VertexComparatorFunctor
							> VerticesSet;



			///---- Constractor(s)
			Vertex(id_t _id) : id (_id), nullObject(false) {}


			//////////////////////////////////////////////////
			//	public methods
			unsigned	Id() const { return id; }


			///////////////////////////////////////////////////
			//	overloaded operator(s)

			bool		operator == (const Vertex *) const;
			bool		operator == (const Vertex &v) const { return id == v.id; }
			
			bool		operator != (const Vertex *) const;
			bool		operator != (const Vertex &v) const { return id != v.id; }

			friend bool operator == (const Vertex *, const Vertex &);
			friend bool operator != (const Vertex *, const Vertex &);

			operator bool (void) const { return !nullObject; }

			static const Vertex & BadVertex();// { return staatic bad; }
		private:
			friend class Graph;
			Vertex() : nullObject(true) {}
			
			///////////////////////////////////////////////////
			//	private members
			id_t id;
			bool nullObject;
		};


		extern bool operator == (const Vertex *, const Vertex &);
		extern bool operator != (const Vertex *, const Vertex &);

	}	//namespace graphs
}	//namespace iviews
#endif //VERTEX_H