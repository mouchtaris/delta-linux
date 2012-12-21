// ParseFunctionsAndCalls.cpp.
// Translation rules for functions and function calls.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Re-factored last August 2006.
//

#include <string.h>
#include <limits.h>
#include <math.h>

#include "DDebug.h"
#include "ParseActions.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "DebugNamingForStaticVars.h"
#include "TypeCheck.h"
#include "LocalDataHandling.h"
#include "SelectiveStepInPreparator.h"
#include "FunctionValueReturnChecker.h"
#include "DeltaCompErrorDefs.h"
#include "CompileOptions.h"
#include "Optimizer.h"
#include "CompilerAPI.h"
#include "Optimizer.h"

#define	RETVAL_EXPR					DPTR(EXPRFACTORY.GetReturnValue())
#define	DELTA_ISUNDEFINED_LIBFUNC	"std::isundefined"

//------------------------------------------------------------------
// FUNCTIONS, CALL AND DEFINITION.

// Expression lists are constructed in reverse order. Hence,
// e1,e2,...,en becomes en->en-1,...,e2->e1. Since those are used
// as formal arguments, they will put as |en|en-1|...|e2|e1|<-TOS in the
// stack, being the correct placement.
//
DeltaExpr* Translator::Translate_ExpressionListItem (DeltaExpr* expr)
	{ return DNPTR(expr)->AdaptIfBool(); }

DeltaExpr* Translator::Translate_ExpressionList (DeltaExpr* elist, DeltaExpr* expr) {
	if (!expr)
		return elist;
	else {
		DASSERT(!expr->next);
		if (elist)
			DPTR(expr)->next = DPTR(elist);
		else
			DPTR(expr)->next = NIL_EXPR;

		return expr;
	}
}

///////////////////////////////////////////////////////////////////

static void AddArgumentInSignature (CallSig& sig, DeltaExpr* arg) {
	if (arg) {
		std::list<DeltaLibraryTypeInfo> typeList;
		DPTR(arg)->AddAllPlausibleTypes(&typeList);
		DASSERT(!typeList.empty());
		if (typeList.size() == 1)
			sig.push_front(typeList.front());
		else
			sig.push_front(DeltaLibraryTypeInfo(typeList, true));	// Arguments are supplied in reverse order.
	}
}

//******************************

DeltaExpr* Translator::Translate_FunctionCall (DeltaExpr* func, DeltaExpr* actualArgs, DeltaSymbol* explicitTemp) {

	NULL_EXPR_CHECK(func);

	if (!TYPECHECKER.Check_FunctionCall(func))
		return NIL_EXPR;

	if (func->type == DeltaExprString) {
		DeltaSymbol* sym = DELTASYMBOLS.Lookup(DPTR(func)->strConst);
		if (sym && DPTR(sym)->IsLibraryFunction())
			DPTR(func)->SetLibraryFunction(sym);
		else
			DELTACOMP_WARNING_LIBFUNC_UNKNOWN(func);
	}
	else
	if (DPTR(func)->GetType() == DeltaExprTableElement && DPTR(func)->IsAttribute())
		func = DPTR(func)->AdaptIfTableContent();
	
	// Use the explicit temp given, or take a temporary symbol from actualArgs if
	// available. This is necessary for the result expression.
	//
	DeltaSymbol* tmp = explicitTemp;
	if (!tmp)
		if (DPTR(func)->IsTemp())					// Reuse function temp symbol.
			tmp = func->sym;
		else
		for (DeltaExpr* p = actualArgs; p; p = p->next)
			if (DPTR(p)->IsTemp()) {			// Find one of the actual arguments' temp.
				tmp = p->sym;
				break;
			}
	if (!tmp)
		tmp = DELTASYMBOLS.NewTemp();

	DeltaExpr* resultExpr	= EXPRFACTORY.New();
	resultExpr->type		= DeltaExprCall;
	resultExpr->userData	= func;	// Store callee for future reference.
	DPTR(resultExpr)->sym	= DPTR(tmp);

	util_ui32 extraArgs = 1;					// Reserved 'arguments' formal
	if (DPTR(func)->type == DeltaExprProgramFunction && DPTR(DPTR(func)->sym)->funcClass == DELTA_FUNCCLASS_METHOD)
		++extraArgs;							// Reserved 'self' formal.
	util_ui32 totalArgs = 0;

	SELECTIVESTEPIN.OnCallDone(		// Selective step-in activities.
		DPTR(func)->GetUnparsed(),
		Unparse_ExprList(actualArgs),
		QUADS.NextQuadNo()
	);

	// This is a hack to avoid testing for undefined variables provided as arguments
	// to the isundefined() library function.
	//
	bool checkUndefined =	func->GetType() != DeltaExprLibraryFunction	||
							func->sym->GetName() != DELTA_ISUNDEFINED_LIBFUNC;

	// We try to emit as many actual arguments as possible with a single quad.
	// This accounts only to normal arguments, not to late bound arguments.
	// The purpose is making more compact target code and faster execution.
	//
	bool		hasLateBoundsArgs = false;
	CallSig		sig;
	
	while (DNPTR(actualArgs)) {

		if (!actualArgs->IsLateBound()) {

			DeltaExpr* arg1 = actualArgs;
			DeltaExpr* arg2 = NIL_EXPR;
			DeltaExpr* arg3 = NIL_EXPR;

			if (actualArgs->next && !DPTR(actualArgs->next)->IsLateBound()) {
				arg2 = actualArgs = DPTR(actualArgs)->next;
				++totalArgs;

				if (actualArgs->next && !DPTR(actualArgs->next)->IsLateBound()) {
					arg3 = actualArgs = DPTR(actualArgs)->next;	
					++totalArgs; 
				}
			}

			if (checkUndefined) {
				if (arg1) arg1->CheckUninitialised(); 
				if (arg2) arg2->CheckUninitialised(); 
				if (arg3) arg3->CheckUninitialised(); 
			}

			QUADS.Emit(DeltaIC_PUSHARG, arg1, arg2, arg3);

			AddArgumentInSignature(sig, arg1);
			AddArgumentInSignature(sig, arg2);
			AddArgumentInSignature(sig, arg3);
		}
		else {
			hasLateBoundsArgs = true;

			if (!TYPECHECKER.Check_LateBoundArg(actualArgs))
				return NIL_EXPR;

			if (checkUndefined) 
				actualArgs->CheckUninitialised();

			QUADS.Emit(DeltaIC_PUSHLATEBOUNDARG, actualArgs, NIL_EXPR, NIL_EXPR);

			sig.push_back(DeltaLibraryTypeInfo(TagAnyValues));
		}

		actualArgs = DPTR(actualArgs)->next;
		++totalArgs;
	}
	
	DeltaSymbol* funcSym = DPTR(func)->sym;

	if (!hasLateBoundsArgs) {
		if ((DPTR(func)->GetType() == DeltaExprProgramFunction || (DPTR(func)->GetType() == DeltaExprVar && funcSym->IsImportedFunctionVar()))
			&& DPTR(funcSym)->GetTotalFormals() != (totalArgs + extraArgs)) {
			if (DPTR(funcSym)->GetTotalFormals() < (totalArgs + extraArgs) && !DPTR(funcSym)->HasVarArgs())	// More actuals than formals.
				DELTACOMP_WARNING_FORMALS_ACTUALS_MISMATCH(
					DPTR(funcSym)->GetFunctionReadableName(),
					DPTR(funcSym)->GetTotalFormals(), 
					totalArgs
				);
			if (DPTR(funcSym)->GetTotalFormals() > (totalArgs + extraArgs))	// Less actuals than formals.
				TYPECHECKER.Check_ProgramFunctionExactMissingArguments(funcSym, totalArgs + extraArgs);
		}
		else
		if (DPTR(func)->type == DeltaExprLibraryFunction && funcSym)	// Known library function
			TYPECHECKER.Check_NumberOfLibraryFunctionActualArguments(funcSym, totalArgs);
	}

	TYPECHECKER.Check_LibFunctionArguments(funcSym, sig);	
	
	if (func->GetType() == DeltaExprTableElement) 
		QUADS.Emit(
			DPTR(func)->IsBoundedElement() ? DeltaIC_CALLOBJBOUNDEDGETMETHOD : DeltaIC_CALLOBJGETMETHOD,
			func,
			DPTR(func)->index,
			NIL_EXPR
		);
	else {
		QUADS.Emit(
			DPTR(func)->GetType()== DeltaExprLibraryFunction ? DeltaIC_CALLEXTFUNC : DeltaIC_CALLFUNC,
			NIL_EXPR,
			NIL_EXPR,
			func
		);

		if ((funcSym && funcSym == PARSEPARMS.CurrFunction()) || func->GetType() == DeltaExprLambda)
			QUADS.GetQuad(QUADS.CurrQuadNo()).SetUserData(
				PARSEPARMS.CurrFunction()->IsMethod() ? 
					(void*) DELTA_RECURSIVE_METHODFUNC_USERDATA : 
					(void*) DELTA_RECURSIVE_PROGRAMFUNC_USERDATA
			);
	}

	// Now emit an asign from the returned value (if any).

	QUADS.Emit(DeltaIC_GETRETVAL, NIL_EXPR, NIL_EXPR, resultExpr);
	return resultExpr;
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_IdentList (const char* id) {
	DASSERT(PARSEPARMS.InFormalArgs());
	DELTASYMBOLS.NewSymbol(id);
}

void Translator::Translate_VarArgs (void) 
	{ DPTR(PARSEPARMS.CurrFunction())->SetHasVarArgs(); }

///////////////////////////////////////////////////////////////////

void Translator::Translate_HandleCreateFunctionVar (DeltaSymbol* func) {

	DASSERT(!DPTR(func)->GetFunctionAccess());

	if (!DPTR(func)->IsAnonymousFunction()) {
		DASSERT(PARSEPARMS.CurrScope().value() == DPTR(func)->GetScope());	// We should be at the same scope as the function.
		const std::string name = uconstructstr(
			"%s%s%s",
			DELTA_HIDDEN_SYMBOL_ID_PREFIX,
			DELTA_HIDDEN_SYMBOL_ID_PREFIX,
			func->GetName().c_str()
		);
		DeltaSymbol* symbol = DELTASYMBOLS.NewSymbol(name, true);
		DPTR(func)->funcAccess = DNEWCLASS(DeltaSymbol::FuncAccess, (func, symbol));
	}
}

//****************************

void Translator::Translate_HandleAssignFunctionVar (DeltaSymbol* func) {

	if (!DPTR(func)->IsAnonymousFunction()) {
	
		DPTR(func)->funcAccess = DNULLCHECK(DPTR(func)->GetFunctionAccess());
		DeltaExpr* funcExpr		= EXPRFACTORY.New();
		DPTR(funcExpr)->sym		= DPTR(func);
		DPTR(funcExpr)->type	= DeltaExprProgramFunction;

		DeltaExpr* funcVarExpr	= EXPRFACTORY.New();
		DPTR(funcVarExpr)->sym	= DPTR(DPTR(func)->funcAccess)->GetFunctionVar();
		DPTR(funcVarExpr)->SetInitialised();

		if (!COMPOPTIONS.GetProductionMode())							// Except when we are in debug mode.
			OPTIMIZER.ExcludeFromTempElimination(QUADS.CurrQuadNo());	// To ensure optimization will not strip it off.
		else
			QUADS.GetQuad(QUADS.CurrQuadNo()).MarkAsDropped();			// Else we drop it directly
		QUADS.Emit(DeltaIC_ASSIGN, funcExpr, NIL_EXPR, funcVarExpr);

		DPTR(DPTR(func)->funcAccess)->SetAssignQuadNo(QUADS.CurrQuadNo());
	}
}

///////////////////////////////////////////////////////////////////
// Definition of a user defined function (can be member) or an event function.
// If the function name begins with an underscore, it means it
// is a temporary function name, for un-named defined functions.
//
DeltaSymbol* Translator::Translate_Function (const char* id, util_ui8 funcClass, util_ui8 linkage) {

	DASSERT(
		funcClass == DELTA_FUNCCLASS_METHOD		||
		funcClass == DELTA_FUNCCLASS_ONEVENT	||
		funcClass == DELTA_FUNCCLASS_PROGRAMFUNCTION
	);
	DASSERT(
		linkage	== DELTA_FUNCTION_DEFAULT_EXPORT ||
		linkage	== DELTA_FUNCTION_NO_EXPORT
	);

	if (funcClass == DELTA_FUNCCLASS_METHOD) {
		if (!PARSEPARMS.InTableExpr().inside()) {

			if (id && PARSEPARMS.IsOperator(id)) {
				DELTACOMP_ERROR_OPERATOR_MUST_INSIDE_OBJECT_CONSTRUCTOR(id);
				return NIL_SYMBOL;
			}

			// 1) if named method, declare a local var with the method name (warning
			// if already defined as a var) and nullify id, else a temp var.

			DeltaExpr* methodVar;
			if (id) {
				methodVar = PARSEPARMS.CurrScope().value() == DELTA_GLOBAL_SCOPE ? 
							Translate_Lvalue(id) : Translate_Lvalue(id, DELTA_LOCAL_SCOPEOFFSET);

				if (!methodVar)
					return NIL_SYMBOL;

				if (DPTR(methodVar)->GetType() == DeltaExprVar)	
					if (!DPTR(methodVar)->IsVarDeclaration())		// was earlier defined
						DELTACOMP_WARNING_ORPHAN_METHOD_OVERWRITES_USER_VAR(id, DPTR(methodVar->sym)->GetLine());
					else
						methodVar->ResetVarDeclaration();
				unullify(id);
			}
			else
				methodVar = EXPRFACTORY.MakeTempVar();
		
			DPTR(methodVar)->SetInitialised();

			// 2) open artificially a table ctor and mark it is auto produced, storing
			// the artificial method var.

			DeltaExpr* autoTable = Translate_TablePrefix();
			DPTR(autoTable)->SetAutoTableConstruction(methodVar);
		}
	}
	else 
	if (id && PARSEPARMS.IsOperator(id)) {
		DELTACOMP_ERROR_OPERATOR_MUST_BE_METHOD(id);
		return NIL_SYMBOL;
	}
	
	std::string funcName(
		funcClass != DELTA_FUNCCLASS_METHOD && id ?		// User-defined named function?
			id : 
			DELTASYMBOLS.NewTempFuncName(funcClass == DELTA_FUNCCLASS_METHOD)
	);

	DeltaSymbol* sym = DELTASYMBOLS.NewSymbol(funcName, false);
	DPTR(sym)->isUserDefinedFunc	= true;
	DPTR(sym)->funcClass			= funcClass;
	DPTR(sym)->serial				= PARSEPARMS.CurrFuncSerial();
	DPTR(sym)->funcSig				= DNEWCLASS(
										DeltaSymbol::FuncSig,
										(
											funcClass != DELTA_FUNCCLASS_METHOD && 
											id									&& 
											DPTR(sym)->IsGlobalFunction ()		&& 
											linkage == DELTA_FUNCTION_DEFAULT_EXPORT
										)
									);

	Translate_HandleCreateFunctionVar(sym);

	if (linkage == DELTA_FUNCTION_NO_EXPORT && (funcClass == DELTA_FUNCCLASS_METHOD || !id || !PARSEPARMS.InGlobalScope()))
		DELTACOMP_WARNING_REDUNDANT_LOCAL_IN_NON_EXPORTABLE_FUNCTION(sym);

	DPTR(sym)->IncludeReturnType(DeltaLibraryTypeInfo(TagVoid));

	if (id && funcClass == DELTA_FUNCCLASS_METHOD) {
		if (!strcmp(id, DELTA_TOSTRING_SYMBOLIC_NAME))
			DPTR(sym)->methodName = DELTA_OPERATOR_OBJECT_TOSTRING;
		else 
			DPTR(sym)->methodName = id;
	}

	PARSEPARMS.IncFuncSerial();
	PARSEPARMS.PushFunction(sym);
	PARSEPARMS.CurrScope().enter();
	PARSEPARMS.SetInFormalArgs(true);
	SELECTIVESTEPIN.OnFunctionEnter();

	// For member functions, the first formal agrument is the 
	// SELF pointer.
	if (funcClass == DELTA_FUNCCLASS_METHOD)
		DELTASYMBOLS.NewSymbol(DELTA_SELF_POINTER_ID, true);

	// For all user defined functions, the first argument is always
	// the 'arguments' table.
	DELTASYMBOLS.NewSymbol(DELTA_ARGUMENTS_POINTER_ID, true);

	DeltaExpr* expr		= EXPRFACTORY.New();						// Making an expression for the function.
	DPTR(expr)->type	= DeltaExprProgramFunction;
	expr->sym			= sym;

	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);		// Incomplete jump skipping the function code.
	QUADS.Emit(DeltaIC_FUNCENTER, expr, NIL_EXPR, NIL_EXPR);	// Quad to signify the start of the function.

	return sym;
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_FunctionHeader (DeltaSymbol* func) {
	if (func) {
		PARSEPARMS.SetInFormalArgs(false);
		PARSEPARMS.CurrScope().exit();
		LOCALDATA.OnFunctionBegin();
	}
}

///////////////////////////////////////////////////////////////////

DeltaSymbol* Translator::Translate_Function (DeltaSymbol* func, Stmt* block, util_ui32 skipFunctionQuad, util_ui32 orphanMethodLine) {

	if (!func)
		return NIL_SYMBOL;

	// For orphan method we have extra preceeding quads.
	if (DPTR(func)->IsMethod() && DPTR(PARSEPARMS.GetCurrConstructedTable())->IsAutoTableConstruction()) {
		skipFunctionQuad += DELTA_ORPHAN_METHOD_FUNCENTER_OFFSET;
		DASSERT(QUADS.GetQuad(skipFunctionQuad).opcode == DeltaIC_JUMP);
		DASSERT(QUADS.GetQuad(skipFunctionQuad + 1).opcode == DeltaIC_FUNCENTER);
	}

	LOCALDATA.OnFunctionEnd();
	DASSERT(PARSEPARMS.CurrFunction() == func);

	DeltaQuadAddress skipJumpPos = Translate_FunctionEnd(PARSEPARMS.CurrFunction(), skipFunctionQuad + 1);

	PARSEPARMS.PopFunction();
	SELECTIVESTEPIN.OnFunctionExit();
	QUADS.Patch(skipFunctionQuad, skipJumpPos);	// Patch the function skip jump.

	DELTASYMBOLS.PopAndRestoreTempCounter();

	// Handle orphan methods.

	if (DPTR(func)->IsMethod()) {

		DeltaExpr* table = PARSEPARMS.GetCurrConstructedTable();
		DASSERT(table);

		if (DPTR(table)->IsAutoTableConstruction()) {		

			// Make a normal function expression to be added in the table.
			DeltaExpr* methodExpr	= EXPRFACTORY.New();
			DPTR(methodExpr)->type	= DeltaExprProgramFunction;
			DPTR(methodExpr)->sym	= DPTR(func);

			Unparse_FunctionDef(methodExpr, func);

			// Add it as an auto indexed element in the auto table and
			// finish the auto table construction.
			Translate_UnindexedValue(methodExpr, skipFunctionQuad, orphanMethodLine);
			Translate_TableConstructor(table);

			// Assign its first element [0] to the method var (if any, else temp var).
			DeltaExpr* methodVar = DPTR(table)->methodVar;
			table = Translate_TableContent(table, EXPRFACTORY.Make(0));	// Makes a table item
			Translate_AssignExpr(methodVar, DPTR(table)->AdaptIfTableContent());

			DPTR(methodVar)->SetTypeTag(TagMethod);
			DPTR(methodVar)->GetTypeInfo().Set(TagMethod);

			// Finally, make the func symbol aware it is 
			// as an orphan method.
			DPTR(func)->SetOrphanMethod(methodVar);
		}
	}

	return func;
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_HandleAllInnerUses (DeltaSymbol* func) {

	DeltaSymbol::FuncAccess* funcAccess = DPTR(func)->GetFunctionAccess();
	if (DPTR(func)->IsAnonymousFunction())
		{ DASSERT(!funcAccess); return; }
	else
		DASSERT(funcAccess);

	// If a function has a closure, then we need to ensure that any inner
	// function that is accessing it, before we know it has a closure, will still
	// view a function carrying correctly a closure. To do this,
	// we declare a hidden var at the same scope as the function, just before it,
	// that is assigned at runtime the function value (which, due to the way translate
	// operand works, will always provide the closure to the function). Then, we
	// substitute every inner access to the function to refer to this var.

	if (DPTR(func)->HasClosure() && !DPTR(funcAccess)->GetInnerUses().empty()) {
		
		typedef DeltaSymbol::FuncAccess::UseInfoList UseInfoList;
		UseInfoList& innerUses = DPTR(funcAccess)->GetInnerUses();

		for (UseInfoList::iterator i = innerUses.begin(); i != innerUses.end(); ++i) {

			DeltaExpr* funcExpr = i->first;
			DASSERT(
				DPTR(funcExpr)->GetType() == DeltaExprProgramFunction &&
				DPTR(DPTR(funcExpr)->sym)->IsUserDefinedFunction()
			);

			DeltaSymbol* func		= DPTR(funcExpr)->sym;
			DeltaSymbol* clientFunc	= i->second;

			DASSERT(
				DPTR(clientFunc)->IsUserDefinedFunction() &&
				DPTR(clientFunc)->IsInnerFunctionOf(func)
			);

			// Change from the direct use of the function symbol to the
			// use of the function var through closures.

			DPTR(funcExpr)->sym = Translate_UseClosureFunctionVar(func, clientFunc);
			DPTR(funcExpr)->SetType(DeltaExprVar);
		}

		DPTR(funcAccess)->SetFunctionVarUsed();
	}

	DPTR(funcAccess)->ClearInnerUses();

	if (DPTR(funcAccess)->IsFunctionVarUsed()) {
		DASSERT(DPTR(funcAccess)->GetAssignQuadNo());
		QUADS.GetQuad(DPTR(funcAccess)->GetAssignQuadNo()).MarkAsUsed();
	}
}

///////////////////////////////////////////////////////////////////

DeltaQuadAddress Translator::Translate_FunctionEnd (DeltaSymbol* func, util_ui32 startQuad) {

	DASSERT(func);

	DeltaExpr* expr		= EXPRFACTORY.New();
	DPTR(expr)->type	= DeltaExprProgramFunction;
	expr->sym			= DPTR(func);

	QUADS.Emit(DeltaIC_FUNCRET, expr, NIL_EXPR, NIL_EXPR);
	DASSERT(QUADS.GetQuad(startQuad).opcode == DeltaIC_FUNCENTER);

	const DeltaQuadAddress endQuad = QUADS.CurrQuadNo();
	DASSERT(startQuad < endQuad);
	DeltaQuad& funcEnter = QUADS.GetQuad(startQuad);
	DeltaQuad& funcExit	 = QUADS.GetQuad(endQuad);

	if (startQuad + 1 == endQuad) {	// A degenerate empty block.
		funcEnter.arg2	 = funcExit.arg2	= Translate_ConstValue((DeltaNumberValueType) 0);	// No locals.
		funcEnter.result = funcExit.result	= Translate_ConstValue((DeltaNumberValueType) DELTA_MAIN_BLOCK_VALUE);	// Main block.
	}
	else {
		DASSERT(QUADS.GetQuad(startQuad + 1).opcode == DeltaIC_BLOCKENTER);
		DASSERT(QUADS.GetQuad(endQuad - 1).opcode == DeltaIC_BLOCKEXIT);

		DeltaQuad& blockEnter = QUADS.GetQuad(startQuad + 1);
		DASSERT(!funcEnter.arg2 && !funcEnter.result);
		funcEnter.arg2	 = blockEnter.arg1;		// Pass locals
		funcEnter.result = blockEnter.result;	// Pass block id

		DeltaQuad& blockExit = QUADS.GetQuad(endQuad - 1);
		DASSERT(!funcExit.arg2 && !funcExit.result);
		funcExit.arg2	 = blockExit.arg1;		// Pass locals
		funcExit.result  = blockExit.result;	// Pass block id

		blockEnter.MarkAsDropped();
		blockExit.MarkAsDropped();
	}

	if (DPTR(func)->IsFunctionReturningSomething())
		RETVALCHECKER.CheckFunction(startQuad);

	Translate_HandleAssignFunctionVar(func);
	Translate_HandleAllInnerUses(func);

	return endQuad + 1;
}

///////////////////////////////////////////////////////////////////
// We make a func address expression when a function definition
// block appears as an expression inside (). Upon code generation, the
// real address will be taken. For orphan methods we return
// the method var directly.
//
DeltaExpr* Translator::Translate_FunctionParenthesisForm (DeltaSymbol* func) {

	if (!func)
		return NIL_EXPR;
	else 
	if (DPTR(func)->IsOrphanMethod())
		return DPTR(func)->GetMethodVar();
	else {
		DeltaExpr* expr		= EXPRFACTORY.New();
		DPTR(expr)->type	= DeltaExprProgramFunction;
		expr->sym			= DPTR(func);

		return expr;
	}
}

///////////////////////////////////////////////////////////////////
// When allowing direct use of a function as an expression.
//
DeltaExpr* Translator::Translate_FunctionExpresssion (DeltaSymbol* func) {
	DeltaExpr* result = Translate_FunctionParenthesisForm(func);
	Unparse_FunctionDef(result, func);
	return result;
}

///////////////////////////////////////////////////////////////////

Stmt* Translator::Translate_RETURN (DeltaExpr* expr) {
	
	if (!PARSEPARMS.InFunction()) {
		DELTACOMP_ERROR_RETURN_OUTSIDE_FUNCTION();
		return NIL_STMT;
	}
	
	if (expr) {
		DeltaExpr* retVal = DPTR(expr)->AdaptIfBool();
		DPTR(retVal)->CheckUninitialised();
		QUADS.Emit(DeltaIC_RETURNVAL, NIL_EXPR, NIL_EXPR, retVal);
		DPTR(PARSEPARMS.CurrFunction())->IncludeReturnType(DPTR(retVal)->GetTypeInfo());
	}
	else
		QUADS.Emit(DeltaIC_RETURN, NIL_EXPR, NIL_EXPR, NIL_EXPR);

	return NEW_STMT;
}

//------------------------------------------------------------------
