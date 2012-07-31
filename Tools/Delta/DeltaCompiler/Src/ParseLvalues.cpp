// ParseLvalues.cpp.
// Translation rules for l-values.
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
#include "LibraryNamespaceHolder.h"
#include "TypeCheck.h"
#include "DeltaCompErrorDefs.h"
#include "DeltaParser.h"
#include "TypeTag.h"

//------------------------------------------------------------------
// LVALUES.

static DeltaExpr* Translate_ByteCodeLibraryFunctionLvalue (const std::string& libName, const std::string& funcName, bool* wasLib) {
	if (DeltaSymbol* lib = DELTASYMBOLS.Lookup(libName, DELTA_GLOBAL_SCOPE))
		if (DPTR(lib)->IsImportedLibVar()) {
			*wasLib = true;
			if (DeltaLibraryNamespace* ns = DELTANAMESPACES.LookupNamespace(libName))
				DELTACOMP_ERROR_NAMESPACE_CONFLICTS_BYTECODE_LIBRARY(ns->GetFullyQualifiedName(), libName);
			else
			if (DeltaSymbol* func = lib->GetImportedLibVar()->Get(funcName)) {
				DPTR(func)->GetImportedFuncVar()->SetIsUsed();
				return DeltaExpr::MakeInternalVar(func);
			}
			else
				DELTACOMP_ERROR_BYTECODE_LIBRARY_FUNC_NOT_FOUND(libName, funcName);
		}
	return NIL_EXPR;
}

/////////////////////////////////////////////////////////

#define	TRY_LOOKUP_IN_BYTECODE_LIBRARY(_ns, _id)														\
	if (true) {																							\
		bool wasLib = false;																			\
		if (_ns.size() == 1) {																			\
			if (DeltaExpr* func = Translate_ByteCodeLibraryFunctionLvalue(_ns.front(), _id, &wasLib))	\
				return func;																			\
			else																						\
			if (wasLib)																					\
				return NIL_EXPR;																		\
		}																								\
	} else

/////////////////////////////////////////////////////////

DeltaExpr* Translate_NamespaceLvalue (const NameList& nsPath, const std::string& id, std::string* ns) {

	DASSERT(!nsPath.empty());

	if (nsPath.front() == DELTA_LIBRARYNAMESPACE_SEPARATOR)
		if (nsPath.size() == 1) {															// ::ident
			*ns = std::string(DELTA_LIBRARYNAMESPACE_SEPARATOR) + id;
			return Translate_Lvalue(id.c_str(), ParseParms::CurrScope().value()); 
		}
		else {
			NameList copiedPath(nsPath);
			copiedPath.pop_front();															// ::namespace::ident

			TRY_LOOKUP_IN_BYTECODE_LIBRARY(copiedPath, id);
			return Translate_NamespaceLvalueAsLibraryFunctionOrConst(copiedPath, id, ns, true);
		}
	else {
		TRY_LOOKUP_IN_BYTECODE_LIBRARY(nsPath, id);
		return Translate_NamespaceLvalueAsLibraryFunctionOrConst(nsPath, id, ns, false);	// namespace::ident
	}
}

/////////////////////////////////////////////////////////

DeltaExpr* Translate_NamespaceLvalue (const std::string& id, std::string* ns) {

	DeltaExpr* result = Translate_NamespaceLvalue(ParseParms::GetNamespacePath(), id, ns);
	ParseParms::ClearNamespacePath();
	return result;
}

/////////////////////////////////////////////////////////

static DeltaExpr* SetLvalueTypeInformation (DeltaSymbol* sym, DeltaExpr* lvalue) {
	if (DPTR(sym)->isUserDefinedFunc) {
		lvalue->type = DeltaExprProgramFunction;
		DASSERT(sym->funcClass != DELTA_FUNCCLASS_METHOD);	// Methods can't be referred by name.
		DPTR(lvalue)->SetTypeTag(TagFunction);
		DPTR(lvalue)->GetTypeInfo().Set(TagFunction);
	}
	else
	if (DPTR(sym)->isLibraryFunc) {
		DPTR(lvalue)->type = DeltaExprLibraryFunction;
		DPTR(lvalue)->SetTypeTag(TagLibFunction);
		DPTR(lvalue)->GetTypeInfo().Set(TagLibFunction);
	}

	return lvalue;
}

/////////////////////////////////////////////////////////

static bool HasConflictWithLibraryFunctionOrConst (DeltaSymbol* sym) {
	if (sym->IsLibraryFunction())
		return false;
	else
	if (DeltaSymbol* l = DELTANAMESPACES.LookupFunctionOrConst(sym->GetName())) {
		DELTACOMP_ERROR_CONFLICTS_VISIBLE_LIBRARY_SYMBOL(
			sym->GetName(),
			sym->GetTypeDescription(),
			sym->GetLine(),
			l->GetName(),
			l->GetTypeDescription()
		);
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////

static bool	Translate_ShoudlUseFunctionViaClosureFunctionVar (DeltaSymbol* func, DeltaSymbol* clientFunc) {
	return	clientFunc									&&	// Used in a function.
			!DPTR(func)->IsAnonymousFunction()			&&
			DPTR(func)->HasClosure()					&&
			DPTR(func)->GetMyFunction() != clientFunc	&&	// Client is not the direct enclosing function.
			func != clientFunc
			;												// Client is not itself.
}

//***************************
// We make a closure var with the name of the function, not the
// hidden name of the func var.

static DeltaSymbol* Translate_UseClosureFunctionVar (
		DeltaSymbol*	funcVar, 
		DeltaSymbol*	func, 
		DeltaSymbol*	clientFunc
	) {
	
	DASSERT(clientFunc);

	if (DPTR(clientFunc)->HasClosure())
		if (DeltaSymbol* sym = DPTR(clientFunc)->GetClosure()->Get(func->GetName()))	// Function already visible via a closure var?
			return sym;																	// Then return it.

	// Move up recursively until either conditions hold: 
	// (i) Initial func is met
	//		- Case where client is enclosed in the initial func
	// (ii) Client func and the initial func have the same enclosing func 
	//		- Case where client is enclosed to some function defined 
	//		  later than, but the at the same scope as, the initial func.

	if (func != clientFunc && func->GetMyFunction() != clientFunc->GetMyFunction()) 
		funcVar =	Translate_UseClosureFunctionVar(				
						funcVar, 
						func, 
						clientFunc->GetMyFunction()
					);

	DeltaSymbol* closureVar =	DNEWCLASS(
									DeltaSymbol, 
									(func->GetName(), DeltaSymbol_NotAVar)		// Make a non-var symbol.
								);
	DPTR(closureVar)->SetIsClosureVarAccess(funcVar, clientFunc);				// Associate it with func var.
	DELTASYMBOLS.RegisterClosureFunctionVar(closureVar, clientFunc);			// Install it at client side.

	return closureVar;
}

//***************************

DeltaSymbol* Translate_UseClosureFunctionVar (DeltaSymbol* func, DeltaSymbol* clientFunc) {

	DPTR(DPTR(func)->GetFunctionAccess())->SetFunctionVarUsed();
	if (util_ui32 quadNo = DPTR(DPTR(func)->GetFunctionAccess())->GetAssignQuadNo())	// May not have been set yet.
		QUADS.GetQuad(quadNo).MarkAsUsed();

	return	Translate_UseClosureFunctionVar(
				DNULLCHECK(DPTR(DPTR(func)->GetFunctionAccess())->GetFunctionVar()),
				func, 
				clientFunc
			);
}

/////////////////////////////////////////////////////////

static DeltaSymbol* Translate_ClosureVar (DeltaSymbol* sym, const std::string& id) {

	DeltaSymbol* outerFunc	= DPTR(ParseParms::CurrFunction())->GetMyFunction();
	DeltaSymbol* closureVar	= NIL_SYMBOL;
	
	// An outer non-top global or a local / formal / closure var requiring closure access?
	if (DPTR(sym)->IsGlobal() || DPTR(sym)->GetMyFunction() == outerFunc) {
		if ((closureVar = DPTR(ParseParms::CurrFunction())->GetClosureVar(id)))
			DASSERT(
				DPTR(closureVar)->IsClosureVarAccess() && 
				sym == DPTR(closureVar)->GetClosureVarAccessed()	// Existing closure var sould access sym.
			); 
		else {
			closureVar = DELTASYMBOLS.NewSymbol(id, false);	// Install as a non-var symbol.
			DPTR(closureVar)->SetIsClosureVarAccess(sym, ParseParms::CurrFunction());	// Associate it with sym

			// Accessed symbol happens to be a closure var of the outer function.
			if (DPTR(sym)->IsClosureVarAccess()) {
				DELTACOMP_WARNING_OUTERFUNC_CLOSUREVAR_ACCESS(
					 DPTR(ParseParms::CurrFunction())->GetName(),
					 id,
					 DPTR(outerFunc)->GetName(),
					 closureVar->GetLine()
				);
			}
		}
		sym = closureVar;	// sym should now point to the closure var.
	}
	else	// Accessing a closure var of the outer function in an inactive block?
	if (outerFunc && (closureVar = DPTR(outerFunc)->GetClosureVar(id))) {
		DELTACOMP_WARNING_OUTERFUNC_CLOSUREVAR_STILL_VISIBLE(
			 DPTR(ParseParms::CurrFunction())->GetName(),
			 id,
			 DPTR(outerFunc)->GetName(),
			 closureVar->GetLine()
		);
		sym = closureVar;	// We actually access the closure var of the outer function.
		closureVar = DELTASYMBOLS.NewSymbol(id, false);	// So we make a new closure var in curr function.
		DPTR(closureVar)->SetIsClosureVarAccess(sym, ParseParms::CurrFunction()); // And associate it with the outer closure var (at sym).
		sym = closureVar;	// Now sym should point to the closure var.
	}
	else {	// Any other case concerns inaccessible symbols.
		DELTACOMP_ERROR_INACCESSIBLE_SYMBOL(
			DPTR(sym)->GetName().c_str(),
			ParseParms::CurrFunction()->GetFunctionReadableName().c_str()
		);
		return NIL_SYMBOL;
	}

	return sym;
}

/////////////////////////////////////////////////////////

DeltaExpr* Translate_Lvalue (const std::string& id) {	// Unqualified (pure) ident.

	// Overall we check if already a defined symbol, 
	// else make a new one (that is always an uninitialised var).

	DeltaSymbol* sym = DELTASYMBOLS.Lookup(id);

	if (sym && (DPTR(sym)->IsUserDefinedConst() || DPTR(sym)->IsLibraryConst())) { 
		if (DPTR(sym)->IsLibraryConst())
			QUADS.AddUsedLibraryConst(sym);
		else
		if (HasConflictWithLibraryFunctionOrConst(sym))
			return NIL_EXPR;
		return DeltaExpr::CopyConst(sym);
	}
	else {
		if (sym && ParseParms::InFunction()) {
			if (sym == ParseParms::CurrFunction())	// sym is referring to current function.
				return Translate_LAMBDA_REF(sym);
			else
			if (DPTR(sym)->IsUserDefinedFunction())
				if (Translate_ShoudlUseFunctionViaClosureFunctionVar(sym, ParseParms::CurrFunction()))
					sym = Translate_UseClosureFunctionVar(sym, ParseParms::CurrFunction());	// access via closure func var.
				else
					;
			else
			if (DPTR(sym)->IsTopGlobal()		||		// sym is a legally accessed top-global / static / library function
				DPTR(sym)->IsStatic()			|| 
				DPTR(sym)->IsLibraryFunction()	||
				DPTR(sym)->GetMyFunction() == ParseParms::CurrFunction())	// sym is a var defined in the current context.
				;
			else
			if (!(sym = Translate_ClosureVar(sym, id)))
				return NIL_EXPR;
		}

		// Check for possible errors in scope access to a library symbol (like
		// ambiguous access). Although we get a symbol, the namespace holder returns
		// null for an id when lookup ambiguities exist.

		if (sym)  {
			if (HasConflictWithLibraryFunctionOrConst(sym))
				return NIL_EXPR;
			else
			if (DPTR(sym)->IsLibrarySymbol() && !DELTANAMESPACES.LookupFunctionOrConst(id))
				return NIL_EXPR;
		}
		else
		if ((sym = DELTANAMESPACES.LookupFunctionOrConst(id)) && sym->IsLibraryConst()) {
			QUADS.AddUsedLibraryConst(sym);
			return DeltaExpr::CopyConst(sym);
		}
		
		DeltaExpr* expr = DNEW(DeltaExpr);	// In this case we need an expression.
		if (!sym) {
			sym = DELTASYMBOLS.NewSymbol(id);
			DPTR(expr)->SetVarDeclaration();		// First appearence of the symbol (var decl).
		}
		DPTR(expr)->sym = DPTR(sym);
		SetLvalueTypeInformation(sym, expr);

		// Handle inner uses since the function may get a closure later.
		if (DPTR(sym)->IsUserDefinedFunction()	&& 
			ParseParms::CurrFunction()			&&
			ParseParms::CurrFunction()->IsInnerFunctionOf(sym))
			DPTR(DPTR(sym)->GetFunctionAccess())->RecordInnerUse(expr, ParseParms::CurrFunction());

		return expr;
	}
}

/////////////////////////////////////////////////////////
// Looks for an id either globally or locally.

DeltaExpr* Translate_Lvalue (const std::string& id, util_i16 scopeOffset) {

	DASSERT(
		scopeOffset == DELTA_LOCAL_SCOPEOFFSET ||					// local
		((util_ui32)scopeOffset == ParseParms::CurrScope().value())	// ::
	);

	util_ui16 scope = ParseParms::CurrScope().value() - scopeOffset;

	// Lookup with scope required. If not found, and
	// scopeOffset is for local vars, make new local var. Else,
	// we firstly look for a library function in global namespace. 
	// If not found, it is an error.

	DeltaSymbol*	sym			= DELTASYMBOLS.Lookup(id, scope);
	bool			isVarDecl	= false;
	if (sym && DPTR(sym)->IsUserDefinedConst())
		return DeltaExpr::CopyConst(sym);
	else
	if (!sym) {

		// We need to check for gloabl scope first since when :: is used
		// while at global scope, scopeOffset is still 0

		if (scope == DELTA_GLOBAL_SCOPE) {
			if (!(sym = DELTANAMESPACES.LookupFunctionOrConstAtGlobalNamespace(id))) {
				DELTACOMP_ERROR_CANT_FIND_GLOBAL(id);
				return NIL_EXPR;
			}
		}
		else {
			DASSERT(scopeOffset == DELTA_LOCAL_SCOPEOFFSET);
			sym = DELTASYMBOLS.NewSymbol(id), isVarDecl = true;
		}
	}

	DASSERT(sym);
	DeltaExpr* expr = DNEW(DeltaExpr);
	DPTR(expr)->sym = DPTR(sym);
	if (isVarDecl)
		DPTR(expr)->SetVarDeclaration();

	return SetLvalueTypeInformation(sym, expr);
}

/////////////////////////////////////////////////////////

DeltaExpr* Translate_NamespaceLvalueAsLibraryFunctionOrConst (
		const NameList&		namespacePath,
		const std::string&	funcName, 
		std::string*		ns, 
		bool				atGlobalNamespace
	) {

	DeltaExpr*		result		= NIL_EXPR;
	DeltaSymbol*	funcOrConst	= NIL_SYMBOL;
	
	ns->clear();
	if (atGlobalNamespace) {
		funcOrConst = DELTANAMESPACES.LookupFunctionOrConstAtGlobalNamespace(namespacePath, funcName);
		*ns = DELTA_LIBRARYNAMESPACE_SEPARATOR;	// Global namespace prefix.
	}
	else
		funcOrConst = DELTANAMESPACES.LookupFunctionOrConst(namespacePath, funcName);
	
	if (funcOrConst) {

		if (funcOrConst->IsLibraryFunction()) {

			DASSERT(DPTR(funcOrConst)->isLibraryFunc);

			result				= DNEW(DeltaExpr);
			DPTR(result)->sym	= DPTR(funcOrConst);
			DPTR(result)->type	= DeltaExprLibraryFunction;
			DPTR(result)->SetTypeTag(TagLibFunction);
			DPTR(result)->GetTypeInfo().Set(TagLibFunction);
		}
		else {
			DASSERT(funcOrConst->IsLibraryConst());
			QUADS.AddUsedLibraryConst(funcOrConst);
			return DPTR(DPTR(funcOrConst)->GetConst())->Copy();
		}

		ns->append(
			DELTANAMESPACES.MakeFullyQualifiedName(namespacePath)	+ 
			DELTA_LIBRARYNAMESPACE_SEPARATOR						+
			funcName
		);

		DASSERT(result);		// fallback means we found the Lvalue
	}
	else 
		DASSERT(!result);		// this path implies no Lvalue is found

	return result;
}

/////////////////////////////////////////////////////////
// Equivalent to self.id but via get attribute, not simple get.
//
DeltaExpr* Translate_AttrLvalue (const std::string& id) {
	if (ParseParms::InMethod()) {
		DeltaExpr* self		=  DeltaExpr::MakeInternalVar(DELTA_SELF_POINTER_ID);
		DPTR(self)->index	= Translate_ConstValue(id);
		DPTR(self)->type	= DeltaExprTableElement;
		DPTR(self)->SetIsAttribute();
		return self;
	}
	else {
		DELTACOMP_ERROR_ATTRIBUTES_ALLOWED_IN_METHODS(std::string(DELTA_TOSTRING_SYMBOLIC_NAME) + id);
		return NIL_EXPR;
	}
}

/////////////////////////////////////////////////////////

DeltaExpr* Translate_StaticLvalue (const std::string& id) {

	// If static is used for formal arguments, it is completely ignored.
	//
	if (ParseParms::InFormalArgs()) {
		DELTACOMP_ERROR_ILLEGAL_STATIC_QUALIFIER(id, "formal argument");
		return Translate_Lvalue(id);
	}
	else
	if (ParseParms::CurrScope().value() == DELTA_GLOBAL_SCOPE) {
		DELTACOMP_ERROR_ILLEGAL_STATIC_QUALIFIER(id, "global variable");
		return Translate_Lvalue(id);
	}
	else {

		// Check if already defined symbol in current scope.

		DeltaSymbol* sym = DELTASYMBOLS.Lookup(id, ParseParms::CurrScope().value());
		bool isVarDecl = false;

		if (sym) {
			if (!sym->IsStatic()) {
				DELTACOMP_ERROR_NOT_STATIC_BEFORE(id, sym->GetLine());
				return NIL_EXPR;
			}
		}
		else {
			sym = DELTASYMBOLS.NewStatic(id);
			DEBUGSTATICS.AddStaticVar(sym);
			isVarDecl = true;
		}

		DeltaExpr* expr = DNEW(DeltaExpr);
		DPTR(expr)->sym = DPTR(sym);
		if (isVarDecl)
			DPTR(expr)->SetVarDeclaration();	// First appearence of the symbol.

		return expr;
	}
}

/////////////////////////////////////////////////////////
// 'self' is for method similar to 'this'.
//
DeltaExpr* Translate_SELF (void) {
	if (ParseParms::InMethod())
		return DeltaExpr::MakeInternalVar(DELTA_SELF_POINTER_ID);
	else
	if (ParseParms::InFunction()						&& 
		ParseParms::CurrFunction()->GetMyFunction()) {

			DeltaSymbol* sym = DELTASYMBOLS.Lookup(DELTA_SELF_POINTER_ID);
			if (!sym) {
				DASSERT(!ParseParms::CurrFunction()->GetMyFunction()->IsMethod());
				DELTACOMP_ERROR_USING_SELF_OUTSIDE_METHODS();
				return NIL_EXPR	;
			}

			// Either first time met (not yet a closure var) or not a closure var of the
			// current function.
			if (!DPTR(sym)->IsClosureVarAccess() || !ParseParms::CurrFunction()->GetClosureVar(DELTA_SELF_POINTER_ID)) {
				DELTACOMP_WARNING_SELF_IS_CLOSUREVAR_ACCESS(
					ParseParms::CurrFunction()->GetFunctionReadableName(),
					DELTA_SELF_POINTER_ID,
					ParseParms::CurrFunction()->GetMyFunction()->GetFunctionReadableName()
				);
				sym = Translate_ClosureVar(sym, DELTA_SELF_POINTER_ID);
			}

			DASSERT(sym && DPTR(sym)->IsClosureVarAccess());
			DeltaExpr* expr = DNEW(DeltaExpr);
			DPTR(expr)->sym = sym;

			return expr;
	}
	else {
		DELTACOMP_ERROR_USING_SELF_OUTSIDE_METHODS();
		return NIL_EXPR	;
	}
}

/////////////////////////////////////////////////////////
// Not an Lvalue.
// '@self' is the currently constructed table.
//
DeltaExpr* Translate_NEWSELF (void) {

	if (ParseParms::InTableExpr().inside())
		return DeltaExpr::GetNewSelf();
	else {
		DELTACOMP_ERROR_USING_NEWSELF_OUTSIDE_OBJECT_CTOR();
		return NIL_EXPR;
	}
}

/////////////////////////////////////////////////////////
// Not an Lvalue.
// '@lambda' refers to the function / method itself without
// needing a name.
//
DeltaExpr* Translate_LAMBDA_REF (DeltaSymbol* func) {

	DASSERT(!func || func == ParseParms::CurrFunction());

	if (!func && !ParseParms::InFunction()) {
		DELTACOMP_ERROR_USING_LAMBDA_REF_OUTSIDE_FUNCTIONS();
		return NIL_EXPR;
	}
	else
	if (ParseParms::InMethod() || ParseParms::CurrFunction()->HasClosure())
		return DeltaExpr::GetLambdaRef();
	else {
		if (!func)
			func = ParseParms::CurrFunction();
		DeltaExpr*	expr = DNEW(DeltaExpr);

		DPTR(expr)->sym = DPTR(func);
		expr->type		= DeltaExprProgramFunction;
		expr->SetTypeTag(TagFunction);
		expr->GetTypeInfo().Set(TagFunction);
	
		return expr;
	}
}

/////////////////////////////////////////////////////////

DeltaExpr* Translate_ARGUMENTS (void) {
	if (!ParseParms::InFunction()) {
		DELTACOMP_ERROR_USING_ARGUMENTS_OUTSIDE_FUNCTIONS();
		return NIL_EXPR;
	}
	else {
		DeltaExpr* result = DeltaExpr::MakeInternalVar(DELTA_ARGUMENTS_POINTER_ID);
		result->SetTypeTag(TagObject);
		result->GetTypeInfo().Set(TagObject);
		return result;
	}
}

/////////////////////////////////////////////////////////

DeltaExpr* Translate_TRIPLE_DOT (void) {
	DeltaExpr* expr = Translate_ARGUMENTS(); 
	DNPTR(expr)->SetLateBound(); 
	return expr;
}

/////////////////////////////////////////////////////////
// We do not make a new expression, but only change the type of
// the expression to be a table element. The index is kept 
// in the 'index' field. The reason is that a table element expression
// will be crated once the table content is used with its adaptation
// function.
//
DeltaExpr* Translate_TableContent (DeltaExpr* table, const char* index) {
	
	NULL_EXPR_CHECK(table);

	if (!TypeCheck_Table(table))
		return NIL_EXPR;

	if (!strcmp(index, DELTA_SELF_POINTER_ID)) {
		if (DPTR(table)->IsSelfOrArguments() || DPTR(table)->GetType() == DeltaExprTableConstruction) 
			DELTACOMP_WARNING_SELF_INDEX(table, index);
	}
	else
	if (DPTR(table)->GetType() == DeltaExprLambda) {
		DELTACOMP_ERROR_ILLEGAL_METHOD_INDEXING(table, index);
		return NIL_EXPR;
	}

	// Make a constant expression with index as string.
	// The index should be nil.
	//
	DASSERT(!DPTR(table)->index);

	DPTR(table)->index	= Translate_ConstValue(index);
	table->type			= DeltaExprTableElement;

	return table;	
}

//*****************************

DeltaExpr* Translate_TableContent (DeltaExpr* table, DeltaExpr* index) {

	NULL_EXPR_PAIR_CHECK(table, index);

	if (!TypeCheck_Table(table) || !TypeCheck_TableIndex(index))
		return (DeltaExpr*) 0;

	// The index should be nil.
	//
	DASSERT(!DPTR(table)->index);

	if (index->GetType() == DeltaExprString) {
		std::string indexVal = DPTR(index)->strConst;
		if (indexVal ==  DELTA_SELF_POINTER_ID) {
			if (DPTR(table)->IsSelfOrArguments() || DPTR(table)->GetType() == DeltaExprTableConstruction) 
				DELTACOMP_WARNING_SELF_INDEX(table, indexVal.c_str());
		}
		else
		if (DPTR(table)->GetType() == DeltaExprLambda) {
			DELTACOMP_ERROR_ILLEGAL_METHOD_INDEXING(table, indexVal.c_str());
			return NIL_EXPR;
		}
	}

	DPTR(table)->index	= DPTR(index)->AdaptIfBool();
	table->type			= DeltaExprTableElement;

	return table;
}

//*****************************

DeltaExpr* Translate_BoundedTableContent (DeltaExpr* table, const char* index) {
	table = Translate_TableContent(table, index);
	if (table)
		DPTR(table)->SetBoundedElement();
	return table;
}

DeltaExpr* Translate_BoundedTableContent (DeltaExpr* table, DeltaExpr* index) {
	table = Translate_TableContent(table, index);
	if (table)
		DPTR(table)->SetBoundedElement();
	return table;
}

//------------------------------------------------------------------
