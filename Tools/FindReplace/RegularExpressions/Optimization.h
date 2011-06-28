/*
 * Optimization.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H


#include <stack>
#include "RegExpr.h"

namespace frep {

	#define OR_(X)				dynamic_cast<OrExpr *>(X)
	#define TAG_(X)				dynamic_cast<TagExpr *>(X)
	#define CONCAT_(X)			dynamic_cast<ConcatExpr *>(X)
	#define ONE_OR_MORE_(X)		dynamic_cast<OneOrMoreExpr *>(X)
	#define ZERO_OR_MORE_(X)	dynamic_cast<ZeroOrMoreExpr *>(X)


	class Optimization {
	private:
		typedef enum NodeType{
			NodeOR				= 0,
			NodeCONCAT, 
			NodeTAG,
			NodeONE_OR_MORE,
			NodeZERO_OR_MORE,
			NodeDELTA_ID,
			NodeANY_CHAR,
			NodeWHITE_SPACE,
			NodeQUOTED_STRING,
			NodeINTEGER,
			NodeNUMBER,
			NodeSTRING,
			NodeEPSILON,
			NodeNOTYPE
		}NodeType;

		RegExpr * route;
		std::stack<RegExpr*> k;



		//****************************
		//		private functions
		template <class T>
		RegExpr * ConectGrandfatherToGrandchild(T *grandfather, 
												T *father, 
												T* grandchild);

		//if false didnt has clildren else has clildren.
		bool		PushChildren(RegExpr *);
		bool		IsLeftChild(RegExpr* child);

		NodeType	GetNodeType (RegExpr *);
		RegExpr *	DoOptimization(RegExpr *);
		RegExpr *	OptimizationOr(RegExpr *);		
		RegExpr *	OptimizationConcat(RegExpr *);	
		RegExpr *	OptimizationEpsilon(RegExpr *);	
		RegExpr *	OptimizationNestedOneOrMore(RegExpr *); 
		RegExpr *	OptimizationNestedZeroOrMore(RegExpr *); 

	public:
		
		int			PreOrderSearch (RegExpr *);
		int			PostOrderSearch (RegExpr *);
		void		UnmarkAll (RegExpr *);
		RegExpr *	PostOrderOptimization (RegExpr *);


		Optimization(void);
		~Optimization(void);
	};


	template <class T>
	RegExpr * Optimization::ConectGrandfatherToGrandchild(	T *grandfather, 
															T *father, 
															T* grandchild)
	{
		assert(grandfather && father && grandchild);

		switch( Optimization::GetNodeType(grandfather) ){
			case NodeZERO_OR_MORE: 
				{	
					ZERO_OR_MORE_(grandfather)->e = grandchild; 
					break;
				}
			case NodeTAG:
				{
					TAG_(grandfather)->e = grandchild; 
					break;
				}
			case NodeOR: 
				{	if( IsLeftChild(father) )
						OR_(grandfather)->e1 = grandchild;
					else
						OR_(grandfather)->e2 = grandchild;
					break;
				 }
			case NodeCONCAT: 
				{
					if( IsLeftChild(father) )
						CONCAT_(grandfather)->e1 = grandchild;
					else
						CONCAT_(grandfather)->e2 = grandchild;
					break;
				}
			default: ;//Eimatse se filo
		}
		return dynamic_cast<RegExpr *>(grandfather);
	}

} //namespace frep {

#endif