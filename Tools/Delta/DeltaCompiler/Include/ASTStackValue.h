// ASTEvaluationHelpers.h
// Helper classes for evaluation over an AST.
// ScriptFighter Project.
// A. Savidis, October 2009.
//
#ifndef	ASTSTACKVALUE_H
#define	ASTSTACKVALUE_H

#include "DDebug.h"
#include "uvector.h"
#include "ufunctors.h"
#include "ustlhelpers.h"
#include "Symbol.h"
#include "Expr.h"
#include "InterCode.h"
#include <string>

/////////////////////////////////////////////////////////

namespace AST {

/////////////////////////////////////////////////////////
// Evaluation stack value.

class StackValue {
	
	public:
	enum Type {
		str_t		= 0,
		expr_t		= 1, 
		sym_t		= 2, 
		elems_t		= 3, 
		quadno_t	= 4, 
		stmt_t		= 5, 
		undef_t		= 6
	};
	
	private:
	Type						type;
	union {
		const char*				str;
		DeltaExpr*				expr;
		DeltaSymbol*			sym;
		TableElements*			elems;
		DeltaQuadAddress		quadNo;
		Stmt*					stmt;
	} data;
	
	public:
	void					operator=(const StackValue& v)			
								{ new (this) StackValue(v); }
	bool					operator==(const StackValue& v) const	
								{ return !memcmp(this, &v, sizeof(StackValue)); }
	bool					operator!=(const StackValue& v) const	
								{ return !(*this == v); }
	
	Type					GetType (void) const	
								{ return type; }

	const char*				str (void)						
								{ DASSERT(type == str_t);	return data.str; }
	DeltaExpr*				expr (void)						
								{ DASSERT(type == expr_t);	return data.expr; }
	DeltaSymbol*			sym (void)						
								{ DASSERT(type == sym_t); return data.sym; }
	TableElements*			elems (void)					
								{ DASSERT(type == elems_t); return data.elems; }
	DeltaQuadAddress		quadNo (void)					
								{ DASSERT(type == quadno_t); return data.quadNo; }
	Stmt*					stmt (void)						
								{ DASSERT(type == stmt_t); return data.stmt; }
	
	const char*				operator= (const char*		p)	
								{ new (this) StackValue(p); return p; }
	DeltaExpr*				operator= (DeltaExpr*		p)	
								{ new (this) StackValue(p); return p; }
	DeltaSymbol*			operator= (DeltaSymbol*		p)	
								{ new (this) StackValue(p); return p; }
	TableElements*			operator= (TableElements*	p)	
								{ new (this) StackValue(p); return p; }
	DeltaQuadAddress		operator= (DeltaQuadAddress	p)	
								{ new (this) StackValue(p); return p; }
	Stmt*					operator= (Stmt*			p)	
								{ new (this) StackValue(p); return p; }

	StackValue (const char*			p)	: type(str_t)	{ data.str		= p; }
	StackValue (DeltaExpr*			p)	: type(expr_t)	{ data.expr		= p; }
	StackValue (DeltaSymbol*		p)	: type(sym_t)	{ data.sym		= p; }
	StackValue (TableElements*		p)	: type(elems_t)	{ data.elems	= p; }
	StackValue (DeltaQuadAddress	p)	: type(quadno_t){ data.quadNo	= p; }
	StackValue (Stmt*				p)	: type(stmt_t)	{ data.stmt		= p; }

	StackValue (void) { memset(this, 0, sizeof(StackValue)); type = undef_t; }
	StackValue (const StackValue& v) { memcpy(this, &v, sizeof(StackValue)); }
};
}

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
