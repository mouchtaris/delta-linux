/*
 * RegExprTreeVisitor.h
 * Abstract class
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef REGEXPRTREEVISITOR_H
#define REGEXPRTREEVISITOR_H

#include "RegExpr.h"

namespace frep {

	class RegExprTreeVisitor {
		public:
			
		virtual void VisitOr			(OrExpr *)				= 0;	
		virtual void VisitTag			(TagExpr *)				= 0;
		virtual void VisitString		(StringExpr *)			= 0;
		virtual void VisitConcat		(ConcatExpr *)			= 0;
		virtual void VisitEpsilon		(EpsilonExpr *)			= 0;
		virtual void VisitAnyChar		(AnyCharExpr *)			= 0;
		virtual void VisitOneOrMore		(OneOrMoreExpr *)		= 0;
		virtual void VisitDeltaIdent	(DeltaIdentExpr *)		= 0;
		virtual void VisitWhiteSpace	(WhiteSpaceExpr *)		= 0;
		virtual void VisitZeroOrMore	(ZeroOrMoreExpr *)		= 0;
		virtual void VisitIntegerExpr	(IntegerExpr *)			= 0;
		virtual void VisitQuotedString	(QuotedStringExpr *)	= 0;
		
		

		virtual ~RegExprTreeVisitor(void) {}
	};

} // namespace frep

#endif
