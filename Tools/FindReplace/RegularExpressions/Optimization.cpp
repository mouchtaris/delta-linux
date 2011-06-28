/*
 * Optimization.h 
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <string>
#include <assert.h>
#include <iostream>
#include "Optimization.h"


namespace frep {

#define OR(X)			dynamic_cast<OrExpr *>(X)
#define TAG(X)			dynamic_cast<TagExpr *>(X)
#define CONCAT(X)		dynamic_cast<ConcatExpr *>(X)
#define EPSILONEX(X)	dynamic_cast<EpsilonExpr *>(X)
#define ANY_CHAR(X)		dynamic_cast<AnyCharExpr *>(X)
#define ONE_OR_MORE(X)	dynamic_cast<OneOrMoreExpr *>(X)
#define ZERO_OR_MORE(X)	dynamic_cast<ZeroOrMoreExpr *>(X)



Optimization::Optimization(void){}

Optimization::~Optimization(void){}

//****************************************************
//		Nonmembers functions

//-----------------------------------------------------------------------

bool IsOrLeftChild(RegExpr *child){
	assert(OR(child->parent));

	if( OR(child->parent)->e1->id == child->id )
		return true;
	return false;
}

//-----------------------------------------------------------------------

bool IsConcatLeftChild(RegExpr *child){
	assert(CONCAT(child->parent));

	if( CONCAT(child->parent)->e1->id == child->id )
		return true;
	return false;
}

//-----------------------------------------------------------------------
//Mas endiaferei mono an o papous einai 
//concat h' or
bool Optimization::IsLeftChild(RegExpr* child) {
	assert( (OR(child->parent) || CONCAT(child->parent)) ); 

	if( OR(child->parent) ){
		return IsOrLeftChild(child);
	}
	return IsConcatLeftChild(child);
}



//****************************************************
//		private functions

//-----------------------------------------------------------------------

bool Optimization::PushChildren(RegExpr * e){
	switch( GetNodeType(e)){
		case NodeCONCAT: 
			{	k.push( CONCAT(e)->e2 );
				k.push( CONCAT(e)->e1 );
				break; 
			}
			
		case NodeOR:
			{	k.push( OR(e)->e2 );
				k.push( OR(e)->e1 );
				break;
			}
		case NodeTAG:			k.push( TAG(e)->e );			break; 
		case NodeONE_OR_MORE:	k.push( ONE_OR_MORE(e)->e );	break;
		case NodeZERO_OR_MORE:	k.push( ZERO_OR_MORE(e)->e );	break;
		default: return false; //eimaste se filo

	}//end of switch
	
	return true;
}

//-----------------------------------------------------------------------

Optimization::NodeType Optimization::GetNodeType (RegExpr * e){
	assert(e);

	if( OR(e) )
		return NodeOR;
	
	else if( CONCAT(e) )
		return NodeCONCAT;
	
	else if( TAG(e) )
		return NodeTAG;
	
	else if( ZERO_OR_MORE(e) )
		return NodeZERO_OR_MORE;

	else if( ONE_OR_MORE(e) )
		return NodeONE_OR_MORE;

	else if( dynamic_cast<DeltaIdentExpr *>(e) )
		return NodeDELTA_ID;
	
	else if( dynamic_cast<AnyCharExpr *>(e) )
		return NodeANY_CHAR;
	
	else if( dynamic_cast<WhiteSpaceExpr *>(e) )
		return NodeWHITE_SPACE;

	else if( dynamic_cast<StringExpr *>(e) )
		return NodeSTRING;
	
	else if( dynamic_cast<NumberExpr *>(e) )
		return NodeNUMBER;
	
	else if( dynamic_cast<QuotedStringExpr *>(e) )
		return NodeQUOTED_STRING;

	else if( dynamic_cast<IntegerExpr *>(e) )
		return NodeINTEGER;

	else if( EPSILONEX(e) )
		return NodeEPSILON;
	
	else 
		assert(0);
	return NodeNOTYPE;
}

//-----------------------------------------------------------------------

RegExpr * Optimization::DoOptimization(RegExpr *e){
	switch( GetNodeType(e) ){
		case NodeOR:
			{
				return OptimizationOr(e);
				break;
			}
		case NodeCONCAT:
			{
				return OptimizationConcat(e);
				break;
			}
		case NodeZERO_OR_MORE:
			{
				if( GetNodeType((ZERO_OR_MORE(e))->e) == NodeZERO_OR_MORE )
					return OptimizationNestedZeroOrMore(e);
				else if( GetNodeType((ZERO_OR_MORE(e))->e) == NodeEPSILON )
					return OptimizationEpsilon(e);
				break;
			}
		case NodeONE_OR_MORE:
			{
				if( GetNodeType((ONE_OR_MORE(e))->e) == NodeONE_OR_MORE )
					return OptimizationNestedOneOrMore(e);
				else if( GetNodeType((ONE_OR_MORE(e))->e) == NodeEPSILON )
					return OptimizationEpsilon(e);
				break;
			}

		default: ;//No optimization
	}
	return (RegExpr*)0;
}

//-----------------------------------------------------------------------

RegExpr * Optimization::OptimizationOr(RegExpr * e){
	assert( OR(e) );

	RegExpr * grandfaher	= e->parent;
	RegExpr * leftChild		= OR(e)->e1;
	RegExpr * rightChild	= OR(e)->e2;
	
	//kanoume elegxo na doume an ta paidia tou or einai ta eidia
	if( leftChild->UF() == rightChild->UF() ){

		if( grandfaher ){		//kanoume ton papou na dixnei sto neo paidi
			grandfaher = ConectGrandfatherToGrandchild<RegExpr>(
							grandfaher, 
							e, 
							leftChild
						);
		}
		leftChild->parent = grandfaher;
		return leftChild;
	}
	else
		return e;
}

//-----------------------------------------------------------------------

#define IS_CHILDERN_ZERO_OR_MORE()												\
	(ZERO_OR_MORE(leftChild) 												&&	\
	ZERO_OR_MORE(rightChild) 												&&	\
	(ZERO_OR_MORE(leftChild)->e->UF() == ZERO_OR_MORE(rightChild)->e->UF()))

#define IS_CHILDERN_ONE_OR_MORE()												\
	(ONE_OR_MORE(leftChild) 												&&	\
	ONE_OR_MORE(rightChild) 												&&	\
	(ONE_OR_MORE(leftChild)->e->UF() == ONE_OR_MORE(rightChild)->e->UF()))


RegExpr * Optimization::OptimizationConcat(RegExpr * e){
	assert( CONCAT(e) );

	RegExpr * leftChild		= CONCAT(e)->e1;
	RegExpr * rightChild	= CONCAT(e)->e2;
	RegExpr * grandfather	= e->parent;
	
	if( IS_CHILDERN_ZERO_OR_MORE()						||			
		IS_CHILDERN_ONE_OR_MORE()						||
		(EPSILONEX(leftChild) && EPSILONEX(rightChild))	|| 
		(!EPSILONEX(leftChild) && EPSILONEX(rightChild)) )
	{
		if( grandfather ){
			grandfather = ConectGrandfatherToGrandchild<RegExpr>(grandfather,
																 e,
																 leftChild);
		}
		leftChild->parent = grandfather;
		return leftChild;
	}
	else if( EPSILONEX(leftChild) && !EPSILONEX(rightChild)) {
		if( grandfather ){
			grandfather = ConectGrandfatherToGrandchild<RegExpr>(grandfather,
																 e,
																 rightChild);
		}
		rightChild->parent = grandfather;
		return rightChild;
	}

	else
		return e;
}

//-----------------------------------------------------------------------

RegExpr *	Optimization::OptimizationEpsilon(RegExpr *e){
	assert( ONE_OR_MORE(e) || ZERO_OR_MORE(e) );

	RegExpr * child			= ZERO_OR_MORE(e) ? ZERO_OR_MORE(e)->e : ONE_OR_MORE(e)->e;
	RegExpr * grandfather	= e->parent;
	
	if( EPSILONEX( child ) ){
		if( grandfather ){
			grandfather = ConectGrandfatherToGrandchild<RegExpr>(grandfather, 
																 e, 
																 child);
		}//end of if
		child->parent = grandfather;
		return child;
	}
	else
		return e;
}

//-----------------------------------------------------------------------

RegExpr * Optimization::OptimizationNestedOneOrMore(RegExpr * ex) {
	assert( ONE_OR_MORE_(ex) );
	
	RegExpr * child = ONE_OR_MORE_(ex)->e;

	if( ONE_OR_MORE_(child) ){
		ONE_OR_MORE_(child)->e->parent	= ex;
		ONE_OR_MORE_(ex)->e				= ONE_OR_MORE_(child)->e; 
	}//end of if

	return ex;
}

//-----------------------------------------------------------------------

RegExpr * Optimization::OptimizationNestedZeroOrMore(RegExpr * ex){
	assert( ZERO_OR_MORE(ex) );
	
	RegExpr * child = ZERO_OR_MORE(ex)->e;

	if( ZERO_OR_MORE(child) ){
		ZERO_OR_MORE(child)->e->parent	= ex;
		ZERO_OR_MORE(ex)->e				= ZERO_OR_MORE(child)->e; 
	}//end of if

	return ex;
}



//****************************************************
//		public functions

//-----------------------------------------------------------------------

int Optimization::PreOrderSearch( RegExpr * e){
	assert(e); 
	assert( k.empty() );

	int count		= 0;
	RegExpr * tmp	= (RegExpr*)0; 
	
	k.push(e);
	while( !k.empty() ){
		tmp = k.top();
		k.pop();
		count++;
		PushChildren(tmp);
	}//end of while
	return count;
}

//-----------------------------------------------------------------------

void Optimization::UnmarkAll (RegExpr * e){
	assert(e); 
	assert( k.empty() );

	int count		= 0;
	RegExpr * tmp	= (RegExpr*)0; 
	
	k.push(e);
	while( !k.empty() ){
		tmp = k.top();
		tmp->marked = false;
		k.pop();
		count++;
		PushChildren(tmp);
	}//end of while
	return;
}

//-----------------------------------------------------------------------

int Optimization::PostOrderSearch ( RegExpr * e) {
	assert(e);
	assert( k.empty() );
	
	int count		= 0;
	RegExpr * tmp	= (RegExpr*)0; 
	
	k.push(e);
	while( !k.empty() ){
		tmp = k.top();
		k.pop();

		if( tmp->marked ) { 
			count++;
		}
		else{
			tmp->marked = true;
			k.push(tmp);
			PushChildren(tmp);
		}

	}//end of while
	return count;
}

//-----------------------------------------------------------------------

RegExpr * Optimization::PostOrderOptimization ( RegExpr * e){
	assert(e);
	assert(k.empty());
	int count		= 0;
	RegExpr * tmp	= (RegExpr*)0; 
	
	k.push(e);
	while( !k.empty() ){
		tmp = k.top();
		k.pop();

		if( tmp->marked ) {
			tmp->id = ++count;
			if ( RegExpr * tmp2 = DoOptimization(tmp) )
				tmp = tmp2;
		}//end of if
		else{
			tmp->marked = true;
			k.push(tmp);
			PushChildren(tmp);
		}//end of else
	}//end of while

	return tmp;
}

} //namespace frep