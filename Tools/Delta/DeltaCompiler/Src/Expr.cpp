// Expr.cpp.
// Managing Delta expressions in Delta compiler.
// ScriptFigher Project.
// A. Savidis, October 1999.
//

#include "DDebug.h"
#include "Expr.h"
#include "InterCode.h"
#include "Symbol.h"
#include "ParseParms.h"
#include "ParseActions.h"
#include "ExprCleaner.h"
#include "Optimizer.h"
#include "DeltaStdDefs.h"

//-----------------------------------------------------------------
// The default expression type created is a storage
// variable.
//
DeltaExpr::DeltaExpr (void) :	typeTag (TagAnyValue),
								typeInfo(TagAnyValue) {
	
	type			= DeltaExprVar;
	trueList		= 
	falseList		= 0;

	strConst		= "";
	boolConst		= 
	lateBindArg		= 
	isBounded		= 
	isAttr			= 
	isVarDecl		=
	isNewSelf		= 
	isAutoTable		= 
	isStringified	= false;

	sym				= NIL_SYMBOL;

	next			= 
	index			= 
	tableItem		= 
	methodVar		= NIL_EXPR;

	unullify(userData);
}

DeltaExpr::~DeltaExpr() {}

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::Copy (void) {

	DeltaExpr* copy = DNEW(DeltaExpr);

	copy->type			= type;
	copy->typeTag		= typeTag;
	copy->sym			= sym;		
	copy->strConst		= strConst;
	copy->numConst		= numConst;
	copy->boolConst 	= boolConst;
	copy->lateBindArg	= lateBindArg;
	copy->index			= index;
	copy->tableItem		= tableItem;
	copy->isBounded		= isBounded;

	unullify(copy->next);
	copy->trueList = copy->falseList = DELTA_NIL_QUAD_LABEL;

	copy->SetUnparsed(GetUnparsed());
	return copy;
}

///////////////////////////////////////////////////////////////////

void DeltaExpr::SetBoolean (bool val) {
	type		=	DeltaExprBoolean; 
	SetTypeTag(TagBool);
	GetTypeInfo().Set(TagBool);
	boolConst	= val;
}

void DeltaExpr::SetNumber (DeltaNumberValueType val) {
	type		=	DeltaExprNumber; 
	SetTypeTag(TagNumber);
	GetTypeInfo().Set(TagNumber);
	numConst	= val;
}

void DeltaExpr::SetString (const std::string& val) {
	type		=	DeltaExprString; 
	SetTypeTag(TagString);
	GetTypeInfo().Set(TagString);
	strConst	= val;
}

void DeltaExpr::SetNil (void) {
	type		=	DeltaExprNil; 
	SetTypeTag(TagNil);
	GetTypeInfo().Set(TagNil);
}

void DeltaExpr::SetLibraryFunction (DeltaSymbol* func) {
	type		=	DeltaExprLibraryFunction; 
	sym			=	func;
	SetTypeTag(TagLibFunction);
	GetTypeInfo().Set(TagLibFunction);
}

///////////////////////////////////////////////////////////////////

static const char* typeStrings[]= {
	DELTA_NUMBER_TYPESTRING,
	DELTA_STRING_TYPESTRING,
	DELTA_NIL_TYPESTRING,
	DELTA_BOOL_TYPESTRING,
	"arithmetic expression",
	"logical expression",
	DELTA_PROGRAMFUNC_TYPESTRING,
	DELTA_LIBRARYFUNC_TYPESTRING,
	"object constructor",
	"object member",
	"variable",
	"lambda",
	"new attribute",
	"function call"
};

const char*	 DeltaExpr::GetTypeString (void) const {
	 DASSERT(type < TOTAL_EXPR_TYPES);
	 return typeStrings[(util_ui32) type];
}

///////////////////////////////////////////////////////////////////
// Functions are not considered as temp symbols
// which can be re-used for storage (in making
// intermediate expressions).
//
bool DeltaExpr::IsTemp (void) const 
	{ return sym && DPTR(sym)->IsTempVar(); }

///////////////////////////////////////////////////////////////////

bool DeltaExpr::IsSelfOrArguments (void) const {
	return	type == DeltaExprVar &&
			(DPTR(sym)->GetName() == DELTA_SELF_POINTER_ID ||
			 DPTR(sym)->GetName() == DELTA_ARGUMENTS_POINTER_ID);
}

bool DeltaExpr::IsTableElementOnNewSelf(void) const {
	return	type == DeltaExprTableElement	&&
			!sym							&&
			isNewSelf;
}

bool DeltaExpr::IsNewSelf(void) const {
	if (type == DeltaExprNewSelf)
		{ DASSERT(!sym); return true; }
	else
		return false;
}

///////////////////////////////////////////////////////////////////

bool DeltaExpr::IsNamedMethod (void) const {
	return	type == DeltaExprProgramFunction				&&
			DPTR(sym)->funcClass == DELTA_FUNCCLASS_METHOD	&&
			!DPTR(sym)->methodName.empty();
}

///////////////////////////////////////////////////////////////////

bool DeltaExpr::IsAutoTableConstruction (void) const {
	if (isAutoTable) {
		DASSERT(type == DeltaExprTableConstruction);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::GetNewSelf (void) {
	DeltaExpr* newSelfExpr			= DNEW(DeltaExpr);
	DPTR(newSelfExpr)->type			= DeltaExprNewSelf;
	DPTR(newSelfExpr)->isNewSelf	= true;
	DPTR(newSelfExpr)->SetTypeTag(TagObject);
	DPTR(newSelfExpr)->GetTypeInfo().Set(TagObject);
	return newSelfExpr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::MakeBool (bool val) {
	DeltaExpr* expr		= DNEW(DeltaExpr);
	DPTR(expr)->type	= DeltaExprBoolean;
	expr->boolConst		= val;
	DPTR(expr)->SetTypeTag(TagBool);
	DPTR(expr)->GetTypeInfo().Set(TagBool);
	return expr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::One (bool minus) 
	{ return minus ? Make(-1) : Make(1); }

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::MakeConst (const std::string& s) {
	DeltaExpr* expr			= DNEW(DeltaExpr);
	DPTR(expr)->type		= DeltaExprString;
	DPTR(expr)->strConst	= s;
	DPTR(expr)->SetTypeTag(TagString);
	DPTR(expr)->GetTypeInfo().Set(TagString);
	return expr;
}

DeltaExpr* DeltaExpr::MakeStringifiedConst (const std::string& s) {
	DeltaExpr* result = MakeConst(s);
	DPTR(result)->SetIsStringified();
	return result;
}

///////////////////////////////////////////////////////////////////
// Internal vars are not assumed to be variable decls
// (thats is initially undefined) since they are used to carry 
// reserved values (builtin names like 'self', or temps).

DeltaExpr* DeltaExpr::MakeInternalVar (const char* id) {
	DeltaSymbol* sym = DELTASYMBOLS.Lookup(id);
	DASSERT(sym);
	DeltaExpr* expr = MakeInternalVar(sym);
	return expr;
}

DeltaExpr* DeltaExpr::MakeInternalVar (DeltaSymbol* sym) {
	DeltaExpr* expr		= DNEW(DeltaExpr);
	DPTR(expr)->sym		= sym;
	DPTR(expr)->SetTypeTag(TagAnyValue);
	DPTR(expr)->GetTypeInfo().Set(TagAnyValue);
	return expr;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::MakeTempVar (void) {
	DeltaExpr* expr		= DNEW(DeltaExpr);
	DPTR(expr)->sym		= DELTASYMBOLS.NewTemp();
	DPTR(expr)->SetTypeTag(TagAnyValue);
	DPTR(expr)->GetTypeInfo().Set(TagAnyValue);
	return expr;
}

DeltaExpr* DeltaExpr::Make (DeltaNumberValueType num) {
	DeltaExpr* expr		=	DNEW(DeltaExpr);
	DPTR(expr)->type	=	DeltaExprNumber;
	expr->numConst		=	num;
	DPTR(expr)->SetTypeTag(TagNumber);
	DPTR(expr)->GetTypeInfo().Set(TagNumber);
	return expr;
}

///////////////////////////////////////////////////////////////////

void DeltaExpr::SetInitialised (void) {
	if (this && type == DeltaExprVar && sym && !DPTR(sym)->IsTempVar()) {
		ParseParms::AssumeToBeInitialised(sym);
		DPTR(sym)->SetInitialised();								
	}
}

///////////////////////////////////////////////////////////////////
// We need to exclude from basic block optimization the
// two assignments to the temp variable, as both are needed. 
// This is an exceptional case where temp value is retained
// across its basic block.
//
#define	ADAPT_IF_BOOL_ACTIONS(expr)																\
	if (true) {																					\
		QUADS.Backpatch(trueList, QUADS.NextQuadNo());											\
		QUADS.Backpatch(falseList, QUADS.NextQuadNoPlus(2));									\
		QUADS.Emit(DeltaIC_ASSIGN, TRUE_EXPR, NIL_EXPR, expr);									\
		OPTIMIZER.ExcludeFromTempElimination(QUADS.CurrQuadNo());								\
		QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR, QUADS.NextQuadNoPlus(2));		\
		QUADS.Emit(DeltaIC_ASSIGN, FALSE_EXPR, NIL_EXPR, expr);									\
		OPTIMIZER.ExcludeFromTempElimination(QUADS.CurrQuadNo());								\
	} else

// Boolean expressions not being constants are adapted,
// by making a temp variable getting a true or false
// in the expression true or false list respectively.
// Boolean constants are adapted by backpatching their
// true or false list to the next quad.
//
DeltaExpr* DeltaExpr::AdaptIfBool (void) {
	
	if (this && type == DeltaExprLogical) {

		DeltaExpr* expr = DNEW(DeltaExpr);
		expr->sym = IsTemp() ? sym : DELTASYMBOLS.NewTemp();
		ADAPT_IF_BOOL_ACTIONS(expr);
		DPTR(expr)->SetUnparsed(GetUnparsed()); // Retains unparsed form.
		return expr;
	}
	else
		return this;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* DeltaExpr::SelfAdaptIfBool (void) {
	if (this && type == DeltaExprLogical) {	// Turn self to a variable carrying the boolean expr value.
		type = DeltaExprVar;
		SetTypeTag(TagAnyValue);
		if (!IsTemp())
			sym = DELTASYMBOLS.NewTemp();
		ADAPT_IF_BOOL_ACTIONS(this);
		return this;
	}
	else
		return this;
}

///////////////////////////////////////////////////////////////////

void DeltaExpr::PreEvaluateIfConstBool (void) {
	if (type != DeltaExprBoolean && IsComputableBoolean()) {
		boolConst	= ToBool();
		type		= DeltaExprBoolean;
		SetTypeTag(TagBool);
		GetTypeInfo().Set(TagBool);
	}
}

///////////////////////////////////////////////////////////////////
// In case an expression is not boolean, it makes an evaluation 
// as a boolean. It is the opposite of the previous.
//

DeltaExpr* DeltaExpr::AdaptToBool (void) {

	if (this && type != DeltaExprLogical) {

		PreEvaluateIfConstBool();

		if (type == DeltaExprBoolean) {
			if (!trueList && !falseList) {	// To avoid readapting.
				QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
				if (boolConst)
					trueList = QUADS.MakeList(QUADS.CurrQuadNo());
				else
					falseList = QUADS.MakeList(QUADS.CurrQuadNo());
			}
		}
		else {
			type = DeltaExprLogical;
			DELTA_EXPR_EMIT_BOOL_TEST(this, this, true);
		}
	}
		
	return this;
}

///////////////////////////////////////////////////////////////////
// The following is used when table content is employed as an 
// r-value expr. In case of table content, we have to use new temps.
//
DeltaExpr* DeltaExpr::AdaptIfTableContent (void) {

	if (!this)	// Null caller allowed.
		return NIL_EXPR;
	else
	if (type != DeltaExprTableElement)
		return this;
	else {

		DeltaExpr* content = DNEW(DeltaExpr);
		DPTR(content)->sym = DELTASYMBOLS.NewTemp();

		QUADS.Emit(
			QUADS.WhichGetOpcode(this),
			this,
			index, 
			content
		);

		// The new expression keeps track that it was
		// a table content expression.
		//
		content->tableItem = this;
		DPTR(content)->SetUnparsed(GetUnparsed()); // Retains unparsed form.
		return content;
	}
}

///////////////////////////////////////////////////////////////////
// The following adaptation is necessary in all cases that a 
// programmer variable is used as an agrument, i.e. in function calls,
// table enumerated lists or table indexed lists. Since the left to right
// evaluation rule applies in Delta semantically, we need to ensure that
// even when a variable is changed, the original value would have been stored.
// For example:
// x=1; f(x, ++x, x, --x); <=>
// x=1; f(1, 2, 2, 1);
// But with i-code of the form:
// x	=	1
// x	=	x+1
// _t1	=	x
// x	=	x-1
// _t2	=	x
// push		x
// push		_t1
// push		x
// push		_t2
// the call erroneously becomes: f(1, 2, 1, 1);
//
DeltaExpr* DeltaExpr::AdaptAsArgumentVariable (void) {

	if (!this)	// Null caller allowed.
		return NIL_EXPR;
	
	DPTR(this);	// Check self.

	// If a variable, we need to hold the original value 
	// in case it changes due to another expression in the list.
	// So we make a temporary symbol to hold the result.
	//
	if (type == DeltaExprVar && !IsTemp()) {

		DeltaExpr* expr = DNEW(DeltaExpr);
		DPTR(expr)->sym = DELTASYMBOLS.NewTemp();
		if (IsVarDeclaration())
			DPTR(expr)->userData = this;	// Store original var for future reference.

		QUADS.Emit(
			DeltaIC_ASSIGN,
			AdaptIfBool(),
			NIL_EXPR,
			expr
		);

		DPTR(expr)->SetUnparsed(GetUnparsed());	// Retains unparsed form.
		return expr;
	}
	else
		return this;
}

///////////////////////////////////////////////////////////////////

const std::string DeltaExpr::Handle (DeltaExpr* expr) {
	
	if (!expr)
		return uconstructstr("%s", "_");
	else
	if (DPTR(expr)->IsConst()) {
		if (expr->type == DeltaExprString)
			return DPTR(expr)->strConst;
		else
		if (expr->type == DeltaExprBoolean)
			return ubool2conststr(expr->boolConst);
		else
		if (expr->type == DeltaExprNil)
			return "NIL";
		else
		if (expr->type == DeltaExprNumber)
			return uconstructstr("%f", expr->numConst);
		else {
			DASSERT(false);
			return "";
		}
	}
	else 
	if (expr->type == DeltaExprLambda)
		return "LAMBDA";
	else 
	if (expr->IsNewSelf() || expr->IsTableElementOnNewSelf())
		return "@SELF";
	else {
		DASSERT(expr->sym);
		return DPTR(DPTR(expr)->sym)->GetName();
	}
}

///////////////////////////////////////////////////////////////////

void DeltaExpr::AddAllPlausibleTypes (TypeList* typeList) const {
	if (type == DeltaExprVar)
		if (userData)	// Ask the original r-value for the type tags.
			{ DASSERT(isVarDecl || IsTemp()); DPTR((DeltaExpr*) userData)->AddAllPlausibleTypes(typeList); }
		else			// A variable stand alone.
			DeltaLibraryTypeInfo::AddInTypeListIfNotInside(typeList, DeltaLibraryTypeInfo(isVarDecl ? TagUndefined : TagAnyValue));
	else
	if (type == DeltaExprCall)	// Ask the original callee for the type tags.
		DPTR((DeltaExpr*) userData)->AddAllPlausibleReturnTypes(typeList);
	else
		DeltaLibraryTypeInfo::AddInTypeListIfNotInside(typeList, typeInfo);
}

///////////////////////////////////////////////////////////////////

void DeltaExpr::AddAllPlausibleReturnTypes (TypeList* typeList) const {
	if (type == DeltaExprVar && userData)																// Ask the original r-value for the return types.
		{ DASSERT(isVarDecl || IsTemp()); DPTR((DeltaExpr*) userData)->AddAllPlausibleReturnTypes(typeList); }
	else
	if (type == DeltaExprVar && sym->IsImportedFunctionVar())											// Use the imported func's return types
		DeltaLibraryTypeInfo::AppendInTypeListIfNotInside(typeList, DPTR(sym)->GetFunctionSignature()->GetReturnTypes());
	else
	if (type == DeltaExprLibraryFunction && sym->GetLibraryFuncSignatures())
		DPTR(DPTR(sym)->GetLibraryFuncSignatures())->AddAllPlausibleReturnTypes(typeList);
	else
	if (type == DeltaExprProgramFunction && !ParseParms::IsOuterFunction(sym))							// Can't be sure for all its return types if enclosing.
		DeltaLibraryTypeInfo::AppendInTypeListIfNotInside(typeList, DPTR(sym)->GetFunctionSignature()->GetReturnTypes());
	else
		DeltaLibraryTypeInfo::AddInTypeListIfNotInside(typeList, DeltaLibraryTypeInfo(TagAnyValue));	// We assume it return anything.
}

///////////////////////////////////////////////////////////////////
// The expr can be a list. The whole list is connected
// at tail, and tail is adjusted as needed. Normally, expr
// is connected to the expr list.
//
void ExprList::Append (DeltaExpr* expr) {

	if (!expr)
		return;
	
	if (!head)						// Making head the expr, also tail temporarily.
		head = tail = DPTR(expr);
	else
		DPTR(tail)->next = expr;	// Tail point to the expr.

	while (DPTR(tail)->next)		// Now, re-locate tail at the last element.
		tail = tail->next;
}

///////////////////////////////////////////////////////////////////
// If elist is not emptty, it is appended.
//
void ExprList::Append (ExprList* elist) {
	if (elist && DPTR(elist)->head)
		Append(elist->head);
}

//-----------------------------------------------------------------
