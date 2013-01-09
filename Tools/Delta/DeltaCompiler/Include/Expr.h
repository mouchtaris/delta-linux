// Expr.h.
// Managing Delta expressions in Delta compiler.
// ScriptFigher Project.
// A. Savidis, October 1999.
// Many revisions followed.
//

#ifndef	EXPR_H
#define	EXPR_H

#include "CompilerComponentDirectory.h"
#include "DeltaByteCodeTypes.h"
#include "TypeTag.h"
#include "LibraryTypes.h"
#include "DDebug.h"
#include "Symbol.h"
#include "AutoCollection.h"
#include "Unparsable.h"
#include "utypes.h"
#include <functional>

enum DeltaExprType {

	// For constant expressions.
	DeltaExprNumber				=	0,
	DeltaExprString				=	1,
	DeltaExprNil				=	2,		
	DeltaExprBoolean			=	3,

	// Binary operators creating expressions.
	DeltaExprArithmetic			=	4,
	DeltaExprLogical			=	5,

	// Functions.
	DeltaExprProgramFunction	=	6,	// Normal, on event (global) and methods.
	DeltaExprLibraryFunction	=	7,	// Library functions (already bound).

	// Table related expresssion.
	DeltaExprTableConstruction	=	8,	// A table construction expression.
	DeltaExprTableElement		=	9,	// A table element.

	DeltaExprVar				=	10,	// For identifiers or temps a known type.
	DeltaExprLambda				=	11,	// For lambda referring to the current method / function.
	DeltaExprNewAttribute		=	12,	// For table attribute definitions
	DeltaExprNewSelf			=	13,	// For self referring to the current constructed table.
	DeltaExprCall				=	14	// For all invocations.
};

#define	TOTAL_EXPR_TYPES	15

// DeltaExpr instances are always dynamic and created via DeltaExprFactory.
//
struct DeltaExpr :	public ucomponentdirectoryclient,
					public AutoCollectable, 
					public Unparsable {
	public:
	DFRIENDDESTRUCTOR()
	friend class DeltaExprFactory;

	typedef std::list<DeltaLibraryTypeInfo> TypeList;

	DeltaExprType			type;			// Expr type.
	DeltaSymbol*			sym;			// Temp symbol. Is null for consts.
	DeltaTypeTag			typeTag;		// Type tag to be filled when type is predictable.
	DeltaLibraryTypeInfo	typeInfo;		// Type info to be filled when type is predictable.
	DeltaMemAddress			trueList;		// True list for bools.
	DeltaMemAddress			falseList;		// False list for bools
	
	std::string				strConst;		// If string const.
	bool					isStringified;	// If string const is a stringified expression.
	DeltaNumberValueType	numConst;		// If num const.
	bool					boolConst;		// If boolean constant.
	bool					lateBindArg;	// If a late binded actual arg.
	bool					isVarDecl;		// If the expression is first appearence of the var.

	DeltaExpr*				index;			// Index expression if table element.
	bool					isNewSelf;		// if indexing on new self (meaning no sym entry).

	DeltaExpr*				tableItem;		// Holding original table item expression, in
											// case we have a var that was extracted as table
											// content. Helps keep track of indexing.

	bool					isBounded;		// If table element is bounded.
	bool					isAttr;			// If table element is an attribute.
	bool					isAutoTable;	// If table ctor is auto for an orphan method
	DeltaExpr*				methodVar;		// If table cotor for an orhpan method, the method's var

	void*					userData;		// Anything extra we may carry as part of a stmt.

	DeltaExpr*				next;			// To easily make lists.
							

	////////////////////////////////////////

	virtual void			Delete (void)
								{ DDELETE(this); }

	DeltaExprType			GetType (void) const 
								{ DASSERT(type < TOTAL_EXPR_TYPES); return type; }
	void					SetType (DeltaExprType t)
								{ type = t; }
	const char*				GetTypeString (void) const;

	DeltaLibraryTypeInfo&	GetTypeInfo (void) 
								{ return typeInfo; }
	const DeltaLibraryTypeInfo&	
							GetTypeInfo (void) const
								{ return typeInfo; }

	DeltaTypeTag			GetTypeTag (void) const 
								{ return typeTag; }
	void					SetTypeTag (DeltaTypeTag tag) 
								{ typeTag  = tag; }

	DeltaExpr*				CheckUninitialised (void) {
								if (this && type == DeltaExprVar && sym) {
									if (sym->IsTempVar()) {
										if (userData)
											DPTR((DeltaExpr*) userData)->CheckUninitialised();
									}
									else
										DPTR(sym)->CheckUninitialised();
								}
								return this;
							}

	void					SetInitialised (void);
	void					SetLateBound (void) 
								{ if (this) lateBindArg = true; }
	void					SetBoundedElement (void) 
								{ DASSERT(type == DeltaExprTableElement); isBounded = true; }
	void					SetVarDeclaration (void)
								{ DASSERT(type == DeltaExprVar); isVarDecl = true; }
	void					ResetVarDeclaration (void)
								{ DASSERT(type == DeltaExprVar && isVarDecl); isVarDecl = false; }

	////////////////////////////////////////
	// Post constructors.

	void					SetBoolean (bool val);
	void					SetNumber (DeltaNumberValueType val);
	void					SetString (const std::string& val);
	void					SetIsStringified (void) 
								{ DASSERT(type == DeltaExprString); isStringified = true; }
	void					SetNil (void);
	void					SetLibraryFunction (DeltaSymbol* func);
	void					SetIsAttribute (void)
								{ DASSERT(type == DeltaExprTableElement); isAttr = true; }

	////////////////////////////////////////
	// Testers.

	bool					IsTemp (void) const;
	bool					IsSelfOrArguments (void) const;
	bool					IsTableElementOnNewSelf(void) const;
	bool					IsNewSelf(void) const;
	bool					IsNamedMethod (void) const;
	bool					IsLateBound (void) const { return lateBindArg; }
	bool					IsVarDeclaration (void) const { return isVarDecl; }
	bool					IsAutoTableConstruction (void) const;
	void					SetAutoTableConstruction (DeltaExpr* _methodVar){ 
								DASSERT(type == DeltaExprTableConstruction); 
								isAutoTable = true;
								methodVar = DNULLCHECK(_methodVar);
							}
	bool					IsBoundedElement (void) const 
								{ DASSERT(type == DeltaExprTableElement); return isBounded; }
	
	bool					IsAttribute (void) const 
								{ DASSERT(type == DeltaExprTableElement); return isAttr; }

	bool					IsInvariantValue (void) const {
								return	IsConst() ||
										type == DeltaExprProgramFunction ||
										type == DeltaExprLibraryFunction  ||
										type == DeltaExprLambda;
							}

	bool					IsComputableBoolean (void) const {
								return	IsInvariantValue()						||
										type == DeltaExprTableConstruction		||
										IsSelfOrArguments();
							}

	bool					IsConst (void) const {
								return	type == DeltaExprNumber ||
										type == DeltaExprString ||
										type == DeltaExprNil	||
										type == DeltaExprBoolean;
							}

	bool					IsStringified (void) const
								{ DASSERT(type == DeltaExprString); return isStringified; }

	////////////////////////////////////////
	// Converters.

	DeltaNumberValueType	ToNumber (void) const {

								DASSERT(IsConst());
								
								switch (type) {

									case DeltaExprNumber :	return numConst;
									case DeltaExprNil :		return 0;
									case DeltaExprString :	return strConst.length();
									case DeltaExprBoolean : return boolConst ? 1 : 0;

									default: { DASSERT(false); return 0; }
								}
							}

	bool					ToBool (void) const {

								DASSERT(IsComputableBoolean());
								
								switch (type) {

									case DeltaExprNumber :	return numConst !=0;
									case DeltaExprNil :		return false;
									case DeltaExprString :	return !strConst.empty();
									case DeltaExprBoolean : return boolConst;

									case DeltaExprTableConstruction: 
									case DeltaExprNewSelf:
									case DeltaExprProgramFunction:
									case DeltaExprLibraryFunction: 
									case DeltaExprLambda:
									case DeltaExprVar: return true;

									default: { DASSERT(false); return false; }
								}
							}

	////////////////////////////////////////
	// Type adapters.

	DeltaExpr*				AdaptIfBool (void);
	DeltaExpr*				SelfAdaptIfBool (void);			// returns self
	DeltaExpr*				AdaptToBool (void);				// returns self
	DeltaExpr*				AdaptIfTableContent (void);
	DeltaExpr*				AdaptAsArgumentVariable (void);
	void					PreEvaluateIfConstBool (void);

	////////////////////////////////////////

	static const std::string	Handle (DeltaExpr* expr);
	void						AddAllPlausibleTypes (TypeList* typeList) const;
	void						AddAllPlausibleReturnTypes (TypeList* typeList) const;

	private:
	DeltaExpr (ucomponentdirectory* directory);
	~DeltaExpr();
};

// The lists made via expressions directly, are reversed.
// We need lists which maintain the order, while we want 
// to also concatenate lists quickly. The following does
// the job.
//
struct ExprList : public AutoCollectable {
	friend struct TableElements;
	DFRIENDDESTRUCTOR()

	DeltaExpr* head;
	DeltaExpr* tail;

	void Append (DeltaExpr* expr);
	void Append (ExprList* elist);

	private:
	ExprList (AutoCollector* collector) : AutoCollectable(collector) { head = tail = (DeltaExpr*) 0; }
	~ExprList(){}
};

#define	NIL_EXPR	(DeltaExpr*) 0

//-----------------------------------------------------------------

class DeltaExprFactory : public ucomponentdirectoryclient {
	public:
	DeltaExpr*	New (void) const;
	DeltaExpr*	Copy (DeltaExpr* expr) const;
	DeltaExpr*	CopyConst (DeltaSymbol* constSym) const;

	DeltaExpr*	One (bool minus = false) const;
	DeltaExpr*	MakeBool (bool) const;
	DeltaExpr*	MakeTempVar (void) const;
	DeltaExpr*	MakeInternalVar (const char* id) const;
	DeltaExpr*	MakeInternalVar (DeltaSymbol* sym) const;
	DeltaExpr*	MakeConst (const std::string& s) const;
	DeltaExpr*	MakeStringifiedConst (const std::string& s) const;
	DeltaExpr*	Make (DeltaNumberValueType num) const;

	DeltaExpr*	GetNewSelf (void) const;

	void		Delete (DeltaExpr* expr) const;

	DeltaExprFactory (ucomponentdirectory *directory) : ucomponentdirectoryclient(directory) {}
	~DeltaExprFactory() {}
};

//-----------------------------------------------------------------

#define EXPRFACTORY_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), DeltaExprFactory)))

#define EXPRFACTORY	EXPRFACTORY_EX(GET_COMPONENT_DIRECTORY())

#define	TRUE_EXPR	EXPRFACTORY.MakeBool(true)
#define	FALSE_EXPR	EXPRFACTORY.MakeBool(false)

//-----------------------------------------------------------------

#endif	// Do not add stuff beyond this point.