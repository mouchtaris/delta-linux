/**
 *	DeltaASTFwdDecls.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_DELTAASTFWDDECLS_H
#define DELTA_DELTAASTFWDDECLS_H

#include "Common.h"
#include "DeltaASTDataTypes.h"

namespace delta {

////////////////////////////////////////////////////////////////////////
// Generic ASTNode type Fwd decls
//
class DeltaASTNode;

template<unsigned _TypeId, class TASTNode, class TValue>
class ContainerASTNode;

template<unsigned _TypeId, unsigned _N, class TASTNode, class TValue>
class NaryASTNode;

template<unsigned _TypeId, class TASTNodeLeft, class TASTNodeRight, class TValue>
class BinaryASTNode;

template<unsigned _TypeId, class TASTNode, class TValue>
class UnaryASTNode;

template<unsigned _TypeId, class TValue>
class LeafASTNode;

////////////////////////////////////////////////////////////////////////

#define	StmtsASTNodeType			1
#define	ExpressionListASTNodeType	2
#define	ArgListASTNodeType			3
#define	UnaryKwdASTNodeType			4
#define	LeafKwdASTNodeType			5
#define	WhileASTNodeType			6
#define	ForASTNodeType				7
#define	ForeachASTNodeType			8
#define	IfASTNodeType				9
#define	ElseASTNodeType				10
#define	TryASTNodeType				11
#define	TrapASTNodeType				12
#define	FunctionASTNodeType			13
#define	FunctionNameASTNodeType		14
#define	StringifiedIdASTNodeType	15
#define	StdLibfuncASTNodeType		16
#define	LibfuncASTNodeType			17
#define	AttributeASTNodeType		18
#define	VariableASTNodeType			19
#define ConstASTNodeType			20
#define ArgASTNodeType				21
#define NewAttributeASTNodeType		22
#define TableElemASTNodeType		23
#define TableElemsASTNodeType		24
#define TableIndexListASTNodeType	25
#define TableConstructASTNodeType	26
#define TableConstKeyASTNodeType	27
#define OtherStmtASTNodeType		28

#define CppASTNodeType				29
#define CommentsASTNodeType			30

//Use only 5 bits for styling, so these are unstyled
#define TernaryOpASTNodeType		31
#define PrefixOpASTNodeType			32
#define SuffixOpASTNodeType			33
#define BinaryOpASTNodeType			34
#define UnaryOpASTNodeType			35
#define CallASTNodeType				36
#define CompoundASTNodeType			37
#define	UsingASTNodeType			38

#define DELTA_STYLING_MASK			31

typedef ContainerASTNode	<StmtsASTNodeType, DeltaASTNode, void>						StmtsASTNode;
typedef ContainerASTNode	<ExpressionListASTNodeType, DeltaASTNode, void>				ExpressionListASTNode;
typedef ContainerASTNode	<ArgListASTNodeType, DeltaASTNode, void>					ArgListASTNode;
typedef UnaryASTNode		<UnaryKwdASTNodeType, DeltaASTNode, Literal>				UnaryKwdASTNode;
typedef LeafASTNode			<LeafKwdASTNodeType, Literal>								LeafKwdASTNode;
typedef NaryASTNode			<WhileASTNodeType, 2, DeltaASTNode, void>					WhileASTNode;
typedef NaryASTNode			<ForASTNodeType, 4, DeltaASTNode, void>						ForASTNode;
typedef NaryASTNode			<ForeachASTNodeType, 4, DeltaASTNode, void>					ForeachASTNode;
typedef NaryASTNode			<IfASTNodeType, 3, DeltaASTNode, void>						IfASTNode;
typedef UnaryASTNode		<ElseASTNodeType, DeltaASTNode, void>						ElseASTNode;
typedef NaryASTNode			<TryASTNodeType, 2, DeltaASTNode, void>						TryASTNode;
typedef NaryASTNode			<TrapASTNodeType, 2, DeltaASTNode, void>					TrapASTNode;
typedef NaryASTNode			<FunctionASTNodeType, 4, DeltaASTNode, Literal>				FunctionASTNode;
typedef LeafASTNode			<FunctionNameASTNodeType, void>								FunctionNameASTNode;
typedef LeafASTNode			<StringifiedIdASTNodeType, void>							StringifiedIdASTNode;
typedef LeafASTNode			<StdLibfuncASTNodeType, void>								StdLibfuncASTNode;
typedef LeafASTNode			<LibfuncASTNodeType, void>									LibfuncASTNode;
typedef LeafASTNode			<AttributeASTNodeType, void>								AttributeASTNode;
typedef LeafASTNode			<VariableASTNodeType, void>									VariableASTNode;
typedef LeafASTNode			<ConstASTNodeType, ConstValueType>							ConstASTNode;
typedef LeafASTNode			<ArgASTNodeType, void>										ArgASTNode;
typedef NaryASTNode			<NewAttributeASTNodeType, 3, DeltaASTNode, void>			NewAttributeASTNode;
typedef BinaryASTNode		<TableElemASTNodeType, DeltaASTNode, DeltaASTNode, void>	TableElemASTNode;
typedef ContainerASTNode	<TableElemsASTNodeType, TableElemASTNode, void>				TableElemsASTNode;
typedef ContainerASTNode	<TableIndexListASTNodeType, DeltaASTNode, void>				TableIndexListASTNode;
typedef UnaryASTNode		<TableConstructASTNodeType, TableElemsASTNode, void>		TableConstructASTNode;
typedef LeafASTNode			<TableConstKeyASTNodeType, TableConstKeyType>				TableConstKeyASTNode;
typedef UnaryASTNode		<OtherStmtASTNodeType, DeltaASTNode, void>					OtherStmtASTNode;
typedef NaryASTNode			<TernaryOpASTNodeType, 3, DeltaASTNode, void>				TernaryOpASTNode;
typedef UnaryASTNode		<PrefixOpASTNodeType, DeltaASTNode, Literal>				PrefixOpASTNode;
typedef UnaryASTNode		<SuffixOpASTNodeType, DeltaASTNode, Literal>				SuffixOpASTNode;
typedef BinaryASTNode		<BinaryOpASTNodeType, DeltaASTNode, DeltaASTNode, Literal>	BinaryOpASTNode;
typedef UnaryASTNode		<UnaryOpASTNodeType, DeltaASTNode, Literal>					UnaryOpASTNode;
typedef NaryASTNode			<CallASTNodeType, 2, DeltaASTNode, void>					CallASTNode;
typedef UnaryASTNode		<CompoundASTNodeType, StmtsASTNode, void>					CompoundASTNode;
typedef NaryASTNode			<UsingASTNodeType, 2, DeltaASTNode, Literal>				UsingASTNode;

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_DELTAASTFWDDECLS_H
