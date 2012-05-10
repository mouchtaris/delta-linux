// TypeCheck.cpp
// Basic type checking for the Delta compiler.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Revisited June 2005 (read the header).
// Extended September 2009, type checking of function calls and
// library function arguments.
//

#include "DDebug.h"
#include "Symbol.h"
#include "Expr.h"
#include "TypeCheck.h"
#include "DeltaCompErrorMsg.h"
#include "DeltaCompErrorDefs.h"
#include "TypeTag.h"
#include "LibraryDefsParser.h"
#include "LibraryFuncSignatures.h"

////////////////////////////////////////////////////////////

static bool TryMatchByTypeInfo (DeltaExpr* expr, util_ui8* tags, util_ui32 n, std::string* typesString) {
	DeltaExpr::TypeList types;
	DPTR(expr)->AddAllPlausibleTypes(&types);
	return DeltaLibraryTypeInfo::MatchAnyTypeTag(types, tags, n, typesString);
}

////////////////////////////////////////////////////////////

static util_ui8 callAllowance[TOTAL_TYPE_TAGS] = {
/*	AnyValue	*/	1,
/*	AnyValues	*/	2,	// No way we meet this.
/*	ExternId	*/	1,
/*	Object		*/	1,
/*	Bool		*/	0,
/*	Number		*/	0,
/*	String		*/	1,
/*	Function	*/	1,
/*	Method		*/	1,
/*	LibFunction	*/	1,
/*	Callable	*/	1,
/*	Void		*/	0,
/*	Undefined	*/	0,
/*	Nil			*/	0
};

bool TypeCheck_FunctionCall (DeltaExpr* func) {

	DASSERT(func);

	if (func->IsConst())
		if (DPTR(func)->GetType() != DeltaExprString) {
			DELTACOMP_ERROR_NOT_A_FUNCTION(func);
			return false;
		}
		else
		if (!DPTR(func)->IsStringified()) {
			DELTACOMP_ERROR_CONST_STRING_NOT_STRINGIFIED_FORM(func->strConst);
			return false;
		}
		else
			return true;
	else {
		std::string typesString;
		if (!TryMatchByTypeInfo(func, callAllowance, TOTAL_TYPE_TAGS, &typesString)) {
			DELTACOMP_ERROR_EXPR_NOT_A_CALLABLE_VALUE(func->GetUnparsed(), typesString);
			return false;
		}
		else
			return true;
	}
}

////////////////////////////////////////////////////////////

void TypeCheck_ProgramFunctionExactMissingArguments (DeltaSymbol* func,  util_ui32 n) {

	std::list<std::string>& formals = DPTR(func)->GetFormals();
	DASSERT(n < formals.size());

	std::string sig;
	bool atLeastTwo = false;
	n = (util_ui32) (formals.size() - n);

	for (std::list<std::string>::reverse_iterator i = formals.rbegin(); n && i != formals.rend(); ++i, --n) {
		if (i != formals.rbegin())
			{ atLeastTwo = true;  sig = std::string(", ") + sig; }
		sig = *i + sig;
	}

	sig = std::string("missing '") + sig + (atLeastTwo ? "' arguments" : "' argument");
	DELTACOMP_WARNING_MISSING_ACTUALS(func->GetFunctionReadableName(), sig);
}

////////////////////////////////////////////////////////////

void TypeCheck_NumberOfLibraryFunctionActualArguments (DeltaSymbol* func, util_ui32 n) {

	if (DeltaLibraryFuncSignatures*	sigsObj = DPTR(func)->GetLibraryFuncSignatures()) {

		typedef DeltaLibraryFuncSignatures::Sigs	Sigs;
		typedef DeltaLibraryFuncSignatures::Sig		Sig;

		void(*handler)(const char*,...) = &DeltaCompWarning;
		if (sigsObj->ShouldPostErrorOnTypeConflict())
			handler = &DeltaCompError;

		const Sigs& sigs =  DPTR(sigsObj)->GetSignatures();
		DASSERT(!sigs.empty());

		if (sigs.size() == 1) {		// Just a single signature?
			if (!sigs.front().MatchesTotalActuals(n)) {
				util_ui32 m = sigs.front().TotalExplicitArgs();
				DELTACOMP_HANDLER_FORMALS_ACTUALS_MISMATCH(
					handler, 
					DPTR(func)->GetName(), 
					m, 
					n
				);
			}
		}
		else {

			for (Sigs::const_iterator i = sigs.begin(); i != sigs.end(); ++i)
				if (i->MatchesTotalActuals(n))
					return;

			// Fallback case means no perfect matching encountered.

			DELTACOMP_HANDLER_LIBFUNC_NUMACTUALS_NOMATCH(
				handler, 
				DPTR(func)->GetName(), 
				sigs.size(), 
				n
			);
		}
	}
}

////////////////////////////////////////////////////////////

static util_ui8 actualFormalMatching[TOTAL_TYPE_TAGS][TOTAL_TYPE_TAGS] = {
					/*	  1 2 3 4 5 6 7 8 9 A B C D E	FORMALS */
/*  ACTUALS			*/
/*	1:AnyValue		*/	{ 1,2,1,1,1,1,1,1,1,1,1,1,1,0 },
/*	2:AnyValues		*/	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,0 },
/*	3:ExternId		*/	{ 1,2,1,0,0,0,0,0,0,0,0,0,0,0 },
/*	4:Object		*/	{ 1,2,1,1,0,0,0,1,1,1,1,0,0,0 },
/*	5:Bool			*/	{ 1,2,1,0,1,1,0,0,0,0,0,0,0,0 },
/*	6:Number		*/	{ 1,2,1,0,1,1,0,0,0,0,0,0,0,0 },
/*	7:String		*/	{ 1,2,1,0,0,0,1,0,0,0,0,0,0,0 },
/*	8:Function		*/	{ 1,2,1,0,0,0,0,1,0,0,1,0,0,0 },
/*	9:Method		*/	{ 1,2,1,0,0,0,0,0,1,0,1,0,0,0 },
/*	A:LibFunction	*/	{ 1,2,1,0,0,0,0,0,0,1,1,0,0,0 },
/*	B:Callable		*/	{ 1,2,1,1,0,0,0,0,0,0,1,0,0,0 },
/*	C:Void			*/	{ 0,2,0,0,0,0,0,0,0,0,0,0,0,0 },
/*	D:Undefined		*/	{ 1,2,1,0,0,0,0,0,0,0,0,0,1,0 },
/*	E:Nil			*/	{ 1,2,1,0,0,0,0,0,0,0,0,0,0,1 }
};

//*******************************

static bool IsActualAppropriateForFormal (DeltaTypeTag actual, DeltaTypeTag formal) {
	util_ui32 b = actualFormalMatching[actual][formal];
	DASSERT(b == 1 || b == 0);
	return b == 1;
}

////////////////////////////////////////////////////////////

static bool MatchSignatures (
		const DefSig&				d_sig,
		DefSig::const_iterator&		di,
		const CallSig&				c_sig,
		CallSig::const_iterator&	ci,
		const IdSig&				i_sig,
		IdSig::const_iterator&		ii,
		util_ui32*					formalNum,
		std::string*				error
	) {
	while (true)
		if (di == d_sig.end())
			return ci == c_sig.end();
		else
		if (ci != c_sig.end() && ci->IsVoid())
			{ error->append(uconstructstr("argument at pos %u is of type '%s'", *formalNum, ci->ToString().c_str())); return false; }
		else
		if (di->IsBuiltIn() && di->GetBuiltIn() == TagAnyValues)	// ...
			return true;
		else
		if (ci == c_sig.end())	{		// No more actuals though we have explicit formals.
			error->append("missing ");
			bool atLeastTwo = false;

			for (IdSig::const_iterator i = ii; i != i_sig.end(); ++i) {
				if (i != ii) 
					{ error->append(" ,"); atLeastTwo = true; }
				error->append(std::string("'") + *i + "'");
			}

			error->append(atLeastTwo ? " arguments" : "argument");
			return false;
		}
		else {
			if (ci->IsBuiltIn() && ci->GetBuiltIn() == TagAnyValues)	// |E| matches zero or more arguments.
				if (++ci == c_sig.end()) // When |E| is the last actual it matches all the rest formals.
					return true;
				else {
					// Else we try match the rest of actuals, after |E|, with the formals,
					// allowing a sequence of initial formals not to match with the current actual,
					// assuming those match with the earlier actual |E|.

					DASSERT(di != d_sig.end() && ii != i_sig.end());
					do 
						if (MatchSignatures(d_sig, ++di, c_sig, ci, i_sig, ++ii, formalNum, error))
							return true;
					while (di != d_sig.end());
				}
			else
			if (DeltaLibraryTypeInfo::Match(&IsActualAppropriateForFormal, *di, *ci)) {
				++di, ++ci, ++ii, ++*formalNum; 
				continue; 
			}
			
			// Fallback case here means error (no matching).
			*error =	uconstructstr(
							"argument '%s' (pos %u) of type '%s' expected but %s supplied", 
							ii->c_str(),
							*formalNum,
							di->ToString().c_str(),
							ci->ToString().c_str()
						);
			return false;
		}
}

//*******************************

bool TypeCheck_LibFunctionArguments (DeltaSymbol* func, const CallSig& c_sig) {
	if (!func)
		return true;	// This is the case to invoke a string not known to be a lif func.
	else
	if (!DPTR(func)->IsLibraryFunction())
		return true;	// No typed signatures for program functions.
	else
	if (DeltaLibraryFuncSignatures* sigsObj = DPTR(func)->GetLibraryFuncSignatures()) {

		typedef DeltaLibraryFuncSignatures::Sigs Sigs;
		const Sigs& sigs = DPTR(sigsObj)->GetSignatures();
		DASSERT(!sigs.empty());

		std::string error;
		for (Sigs::const_iterator i = sigs.begin(); i != sigs.end(); ++i) {

			const DefSig&			d_sig	= i->types;
			const IdSig				i_sig	= i->names;
			CallSig::const_iterator	ci		= c_sig.begin();
			DefSig::const_iterator	di		= d_sig.begin();
			IdSig::const_iterator	ii		= i_sig.begin();

			error.clear();
			util_ui32 formalNum = 0;

			if (MatchSignatures(d_sig, di, c_sig, ci,  i_sig, ii, &formalNum, &error))
				return true;
		}
		
		// Fallback case means no libfunc signatures matched.
		void(*handler)(const char*,...) = &DeltaCompWarning;
		if (sigsObj->ShouldPostErrorOnTypeConflict())
			handler = &DeltaCompError;

		if (sigs.size() == 1)
			DELTACOMP_HANDLER_CALL_LIBFUNC_SIG_DOES_NOT_MATCH(func->GetName(), error, handler);
		else
			DELTACOMP_HANDLER_CALL_LIBFUNC_NO_SIG_MATCHES(func->GetName(), sigs.size(), handler);
		return false;
	}
	else
		return true;	// No signature to check against.
}

////////////////////////////////////////////////////////////

static bool lateBoundArgAllowance[TOTAL_EXPR_TYPES] = {
/*	Number	*/	false,
/*	String	*/	false,
/*	Nil		*/	false,
/*	Boolean	*/	false,
/*	Arith	*/	true,
/*	Logical	*/	true,
/*	Func	*/	false,
/*	Lib		*/	false,
/*	Table	*/	true,
/*	Elem	*/	true,
/*	Var		*/	true,
/*	Lambda	*/	false,
/*	NewAttr	*/	false,
/*	NewSelf */	true,
/*	Call */		true

};

static util_ui8 lateBoundArgAllowanceByTypeTag[TOTAL_TYPE_TAGS] = {
/*	AnyValue	*/	1,
/*	AnyValues	*/	2,	// No way we meet this.
/*	ExternId	*/	1,
/*	Object		*/	1,
/*	Bool		*/	0,
/*	Number		*/	0,
/*	String		*/	0,
/*	Function	*/	0,
/*	Method		*/	0,
/*	LibFunction	*/	0,
/*	Callable	*/	1,	// Can be an object, in principle.
/*	Void		*/	0,
/*	Undefined	*/	0,
/*	Nil			*/	0
};

bool TypeCheck_LateBoundArg (DeltaExpr* arg) {

	DASSERT(arg && uarraysize(lateBoundArgAllowance) == TOTAL_EXPR_TYPES);
	if (!lateBoundArgAllowance[arg->GetType()]) {
		DELTACOMP_ERROR_ILLEGAL_TYPE_IN_DYNAMIC_ARGUMENTS(arg->GetUnparsed(), arg->GetTypeString());
		return false;
	}
	else {
		std::string typesString;
		if (!TryMatchByTypeInfo(arg, lateBoundArgAllowanceByTypeTag, TOTAL_TYPE_TAGS, &typesString)) {
			DELTACOMP_ERROR_ILLEGAL_TYPE_IN_DYNAMIC_ARGUMENTS(arg->GetUnparsed(), typesString.c_str());		
			return true;
		}
		else
			return true;
	}
}

////////////////////////////////////////////////////////////

static bool tableAllowance[TOTAL_EXPR_TYPES] = {
/*	Number	*/	false,
/*	String	*/	false,
/*	Nil		*/	false,
/*	Boolean	*/	false,
/*	Arith	*/	true,
/*	Logical	*/	true,
/*	Func	*/	false,
/*	Lib		*/	false,
/*	Table	*/	true,
/*	Elem	*/	true,
/*	Var		*/	true,
/*	Lambda	*/	true,
/*	NewAttr	*/	false,
/*	NewSelf */	true,
/*	Call */		true
};

#define	tableBoundArgAllowanceByTypeTag lateBoundArgAllowanceByTypeTag

bool TypeCheck_Table (DeltaExpr* t) {
	
	DASSERT(t && uarraysize(tableAllowance) == TOTAL_EXPR_TYPES);
	
	if (!tableAllowance[DPTR(t)->GetType()]) {
		DELTACOMP_ERROR_ILLEGAL_TYPE_AS_OBJECT(t->GetUnparsed(), t->GetTypeString());
		return false;
	}
	else {
		std::string typesString;
		if (!TryMatchByTypeInfo(t, tableBoundArgAllowanceByTypeTag, TOTAL_TYPE_TAGS, &typesString)) {
			DELTACOMP_ERROR_ILLEGAL_TYPE_AS_OBJECT(t->GetUnparsed(), typesString.c_str());		
			return true;
		}
		else
			return true;
	}
}

////////////////////////////////////////////////////////////
// As a table index, we can use virtually anything now.
// Hence, constants, tables, and functions are all legal.
//
bool TypeCheck_TableIndex (DeltaExpr* index) 
	{ return true; }

////////////////////////////////////////////////////////////
// Everything is convertible to boolean.
//
bool TypeCheck_UseAsBoolean (DeltaExpr* expr) 
	{ return true; }

////////////////////////////////////////////////////////////
// Compatibility for ordering operators > < >= <= for expression
// types. Since they are allowed only for numeric types, it is easier
// to provide a single column array.
//
static bool orderingAllowance[TOTAL_EXPR_TYPES] = {
/*	Number	*/	true,
/*	String	*/	true,
/*	Nil		*/	false,
/*	Boolean	*/	false,
/*	Arith	*/	true,
/*	Logical	*/	false,
/*	Func	*/	false,
/*	Lib		*/	false,
/*	Table	*/	false,
/*	Elem	*/	true,
/*	Var		*/	true,
/*	Lambda	*/	false,
/*	NewAttr	*/	false,
/*	NewSelf */	false,
/*	Call */		true
};

bool TypeCheck_InRelational (DeltaExpr* expr, bool onlyEquality) {
	if (onlyEquality)
		return true;
	else {
		DASSERT(uarraysize(orderingAllowance) == TOTAL_EXPR_TYPES);
		return orderingAllowance[ (util_ui8) DPTR(expr)->GetType() ];
	}
}

////////////////////////////////////////////////////////////

bool TypeCheck_InRelational (DeltaExpr* e1, DeltaExpr* e2, DeltaICOpcode relOp) {

	if (relOp == DeltaIC_JEQ || relOp == DeltaIC_JNE)
		return true;						// All types can be compared this way.
	else
	if (e1->GetType() == DeltaExprString || e2->GetType() == DeltaExprString) // Ordering comparison of string consts only between them.
		return e1->GetType() == DeltaExprString && e2->GetType() == DeltaExprString;
	else {
		DASSERT(TypeCheck_InRelational(e1, false) && TypeCheck_InRelational(e2, false));
		return true;
	}
}

////////////////////////////////////////////////////////////

static bool arihtmeticAllowance[TOTAL_EXPR_TYPES] = {
/*	Number	*/	true,
/*	String	*/	false,
/*	Nil		*/	false,
/*	Boolean	*/	false,
/*	Arith	*/	true,
/*	Logical	*/	false,
/*	Func	*/	false,
/*	Lib		*/	false,
/*	Table	*/	true,
/*	Elem	*/	true,
/*	Var		*/	true,
/*	Lambda	*/	false,
/*	NewAttr	*/	false,
/*	NewSelf */	true,
/*	Call */		true
};

bool TypeCheck_InArithmetic (DeltaExpr* expr, DeltaICOpcode arithOp, const char* opStr, bool alwaysError) {
	
	if (arihtmeticAllowance[(util_ui8) DPTR(expr)->GetType()])
		return true;

	if (expr->GetType() == DeltaExprString && arithOp == DeltaIC_ADD)
		return true;

	if (expr->GetType() == DeltaExprBoolean || expr->GetType() == DeltaExprLogical) 
		if (arithOp == DeltaIC_ADD || arithOp == DeltaIC_SUB || arithOp == DeltaIC_MUL)
			return true;
	
	if (alwaysError)
		DELTACOMP_ERROR_ARITHMETIC(opStr, expr);
	else
		DELTACOMP_WARNING_ARITHMETIC(opStr, expr);
	return false;
}

////////////////////////////////////////////////////////////
// External / local functions are not allowed to be assigned
// values, although born in the grammar via lvalues. 
//
bool TypeCheck_Assign (DeltaExpr* lvalue) {
	if (DPTR(lvalue)->IsInvariantValue()) {
		DELTACOMP_ERROR_ASSIGNMENT(lvalue);
		return false;
	}
	else
		return true;
}

////////////////////////////////////////////////////////////

static bool foreachContainerAllowance[TOTAL_EXPR_TYPES] = {
/*	Number	*/	false,
/*	String	*/	false,
/*	Nil		*/	false,
/*	Boolean	*/	false,
/*	Arith	*/	true,
/*	Logical	*/	true,
/*	Func	*/	false,
/*	Lib		*/	false,
/*	Table	*/	true,
/*	Elem	*/	true,
/*	Var		*/	true,
/*	Lambda	*/	false,
/*	NewAttr	*/	false,
/*	NewSelf */	true,
/*	Call */		true
};

static util_ui8 foreachContainerAllowanceByTypeTag[TOTAL_TYPE_TAGS] = {
/*	AnyValue	*/	1,
/*	AnyValues	*/	2,	// No way we meet this.
/*	ExternId	*/	1,
/*	Object		*/	1,
/*	Bool		*/	0,
/*	Number		*/	0,
/*	String		*/	0,
/*	Function	*/	0,
/*	Method		*/	0,
/*	LibFunction	*/	0,
/*	Callable	*/	0,
/*	Void		*/	0,
/*	Undefined	*/	0,
/*	Nil			*/	0
};

bool TypeCheck_ForeachContainer (DeltaExpr* container) {

	DASSERT(uarraysize(foreachContainerAllowance) == TOTAL_EXPR_TYPES);

	if (!foreachContainerAllowance[DPTR(container)->GetType()]) {
		DELTACOMP_ERROR_FOREACH_ILLEGAL_CONTAINER(container->GetUnparsed(), container->GetTypeString());
		return false;
	}
	else {
		std::string typesString;
		if (!TryMatchByTypeInfo(container, foreachContainerAllowanceByTypeTag, TOTAL_TYPE_TAGS, &typesString)) {
			DELTACOMP_ERROR_FOREACH_ILLEGAL_CONTAINER(container->GetUnparsed(), typesString.c_str());		
			return true;
		}
		else
			return true;
	}
}

////////////////////////////////////////////////////////////
