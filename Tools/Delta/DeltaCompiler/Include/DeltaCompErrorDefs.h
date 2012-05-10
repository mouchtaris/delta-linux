// DeltaCompErrorDefs.h
// All compile errors collected together.
// ScriptFighter Project.
// A. Savidis, May 2009.
//

#ifndef	DELTACOMPERRORDEFS_H
#define	DELTACOMPERRORDEFS_H

//////////////////////////////////////////////////////

#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "Expr.h"
#include "ustrings.h"

#define	DELTACOMP_ERROR_REDEFINITION_WAS_FUNCTION(sym)							\
	DeltaCompError("Collision with function named '%s' defined at line %d",		\
		ucstringarg(sym->GetName()),											\
		sym->line																\
	)

#define	DELTACOMP_ERROR_REDEFINITION_WAS_LIBFUNCTION(sym)						\
	DeltaCompError("Collision with visible '%s' library function",				\
		ucstringarg(sym->GetName())												\
	)

#define	DELTACOMP_ERROR_REDEFINITION_WAS_LIBCONST(sym)							\
	DeltaCompError("Collision with visible '%s' library const",					\
		ucstringarg(sym->GetName())												\
	)

#define	DELTACOMP_ERROR_REDEFINITION_WAS_VAR(sym)								\
	DeltaCompError("Variable '%s' already defined at line %d",					\
		ucstringarg(sym->GetName()),											\
		sym->line																\
	)

#define	DELTACOMP_ERROR_DIVISION_BY_ZERO()										\
	DeltaCompError("Division by zero (assuming 1)")

#define	DELTACOMP_ERROR_INVALID_OP_FOR_STRING_CONSTS(opStr)						\
	DeltaCompError("Operator '%s' not allowed for string constants", ucstringarg(opStr));

#define	DELTACOMP_ERROR_ILLEGAL_ASSIGN_TO_CONST(id)								\
	DeltaCompError("Illegal expression assigned to const '%s'", ucstringarg(id))

#define	DELTACOMP_ERROR_CONST_REDEFINITION(id, sym)								\
	DeltaCompError(																\
		"Redefinition as 'const' of '%s' already defined at line '%d'",			\
		ucstringarg(id), DPTR(sym)->line										\
	)

#define	DELTACOMP_ERROR_USING_NEWSELF_OUTSIDE_OBJECT_CTOR()						\
	DeltaCompError("Use of '@self' allowed only in object constructors")

#define	DELTACOMP_ERROR_USING_LAMBDA_REF_OUTSIDE_FUNCTIONS()					\
	DeltaCompError("Use of '@lambda' allowed only in functions")

#define	DELTACOMP_ERROR_USING_ARGUMENTS_OUTSIDE_FUNCTIONS()						\
	DeltaCompError("Use of 'arguments' allowed only in functions")

#define	DELTACOMP_ERROR_METHOD_OUTSIDE_OBJECT_CTOR()							\
	DeltaCompError("Method definitions allowed only in object constructors")

#define	DELTACOMP_ERROR_OPERATOR_MUST_BE_METHOD(id)								\
	DeltaCompError("Illegal 'function%s' since operators must be methods", ucstringarg(id))

#define	DELTACOMP_ERROR_OPERATOR_MUST_INSIDE_OBJECT_CONSTRUCTOR(id)				\
	DeltaCompError("Illegal 'method @operator%s' outside object constructor", ucstringarg(id))

#define	DELTACOMP_ERROR_RETURN_OUTSIDE_FUNCTION()								\
	DeltaCompError("Use of 'return' while not in a function");

#define	DELTACOMP_ERROR_NOT_ALLOWED_OUTSIDE_LOOPS(what)							\
	DeltaCompError("Use of '%s' while not in a loop", ucstringarg(what))

#define	DELTACOMP_ERROR_ILLEGAL_OPERAND_IN_RELATIONAL(op, pos)					\
	DeltaCompError("In relational operator '%s' illegal %s operand", ucstringarg(op), ucstringarg(pos))

#define	DELTACOMP_ERROR_INCOMPATIBLE_OPERANDS_IN_RELATIONAL(op)					\
	DeltaCompError("In relational operator '%s' operands are incompatible", ucstringarg(op))

#define	DELTACOMP_ERROR_INACCESSIBLE_SYMBOL(name, func)							\
	DeltaCompError("Cannot access '%s' inside '%s'", ucstringarg(name), ucstringarg(func))

#define	DELTACOMP_ERROR_ATTRIBUTES_ALLOWED_IN_METHODS(id)						\
	DeltaCompError("Attributes as '%s' allowed only in methods", ucstringarg(id))

#define	DELTACOMP_ERROR_USING_SELF_OUTSIDE_METHODS()							\
	DeltaCompError("Use of 'self' allowed only in methods")

#define	DELTACOMP_ERROR_ILLEGAL_STATIC_QUALIFIER(id, role)						\
	DeltaCompError("Illegal use of 'static' for '%s' %s", ucstringarg(id), ucstringarg(role))

#define	DELTACOMP_ERROR_NOT_STATIC_BEFORE(id, line)								\
	DeltaCompError(																\
		"Using '%s' as 'static' in contrsast to first appearence at line %d",	\
		ucstringarg(id), line													\
	)

#define	DELTACOMP_ERROR_CANT_FIND_GLOBAL(id)									\
	DeltaCompError("No global variable '::%s' exists", ucstringarg(id))

#define	DELTACOMP_ERROR_ILLEGAL_METHOD_INDEXING(table, index)					\
	DeltaCompError(																\
		"illegal indexing '%s.%s': only '%s' is allowed on methods",			\
		ucstringarg(table->GetUnparsed()),										\
		ucstringarg(index),														\
		DELTA_SELF_POINTER_ID													\
	)

#define	DELTACOMP_ERROR_NOT_A_FUNCTION(func)									\
	DeltaCompError("Illegal use of '%s' as a function", func->GetTypeString())

#define	DELTACOMP_ERROR_CONST_STRING_NOT_STRINGIFIED_FORM(str)						\
	DeltaCompError(																	\
		"Illegal call of const string '%s' as a function (use a stringified form)",	\
		ucstringarg(str)															\
	)

#define	DELTACOMP_ERROR_EXPR_NOT_A_CALLABLE_VALUE(expr, types)					\
	DeltaCompError(																\
		"Expression '%s' not a callable value (its type is %s)",				\
		ucstringarg(expr), ucstringarg(types)									\
	)

#define	DELTACOMP_HANDLER_CALL_LIBFUNC_NO_SIG_MATCHES(func, n, handler)						\
	(*handler)(																				\
		"In calling lib function '%s' none of the %u signatures matches the arguments",		\
		ucstringarg(func), (n) 																\
	)

#define	DELTACOMP_HANDLER_CALL_LIBFUNC_SIG_DOES_NOT_MATCH(func, error, handler)		\
	(*handler)(																		\
		"In calling lib function '%s' %s",											\
		ucstringarg(func), ucstringarg(error)										\
	)

#define	DELTACOMP_ERROR_ILLEGAL_TYPE_IN_DYNAMIC_ARGUMENTS(e, type)						\
		DeltaCompError(																	\
			"Illegal dynamic arguments |'%s'| (its type is %s)",						\
			ucstringarg(e), ucstringarg((type))											\
		)

#define	DELTACOMP_ERROR_ILLEGAL_TYPE_AS_OBJECT(t, type)							\
	DeltaCompError("Illegal use of '%s' as an object (type is %s)",				\
		ucstringarg(t), (type)													\
	)

#define	DELTACOMP_ERROR_ARITHMETIC(op, e)										\
	DeltaCompError(																\
		"Arithmetic operator %s not allowed for '%s'",							\
		op, ucstringarg(DPTR(e)->GetTypeString())								\
	)

#define	DELTACOMP_WARNING_ARITHMETIC(op, e)										\
	DeltaCompWarning(															\
		"Arithmetic operator %s on '%s'; is this the intent?",					\
		op, ucstringarg(DPTR(e)->GetTypeString())								\
	)

#define	DELTACOMP_ERROR_ASSIGNMENT(e)											\
	DeltaCompError("Trying to assign to '%s'", DPTR(e)->GetTypeString())

#define	DELTACOMP_ERROR_FOREACH_ILLEGAL_CONTAINER(e,type)						\
	DeltaCompError(																\
		"Illegal container '%s' in 'foreach' (its type is %s)",					\
		ucstringarg(e), (type)													\
	)

#define	DELTACOMP_ERROR_USER_TYPE_REDEFINED(name, where)						\
	DeltaCompError(																\
		"User-defined type '%s' already defined in %s",							\
		ucstringarg(name), ucstringarg(where)									\
	)

#define	DELTACOMP_ERROR_ILLEGAL_TYPE_DEFINITION(def)							\
	DeltaCompError(																\
		"Illegal type definition '%s'",											\
		ucstringarg(def)														\
	)

#define	DELTACOMP_ERROR_LIBCONST_ILLEGAL_NAME_OR_DEF(def)						\
	DeltaCompError(																\
		"Illegal library const name / definition '%s'",							\
		ucstringarg(def)														\
	)

#define	DELTACOMP_ERROR_LIBCONST_ALREADY_DEFINED(id, type)						\
	DeltaCompError(																\
		"In library const def '%s' symbol was earlier defined (as %s)",			\
		ucstringarg(id), ucstringarg(type)										\
	)

#define	DELTACOMP_ERROR_LIBFUNC_ALREADY_DEFINED_AS_CONST(id)					\
	DeltaCompError(																\
		"In library func def '%s' symbol was ealrier defined as const",			\
		ucstringarg(id)															\
	)

#define	DELTACOMP_ERROR_LIBCONST_VALUE(id, error)								\
	DeltaCompError(																\
		"In library const '%s' value '%s'",										\
		ucstringarg(id), ucstringarg(error)										\
	)

#define	DELTACOMP_ERROR_LIBFUNC_ILLEGAL_NAME_OR_PROTO(name)						\
	DeltaCompError(																\
		"Illegal library function name / prototype '%s'",						\
		ucstringarg(name)														\
	)

#define	DELTACOMP_ERROR_LIBFUNC_ILLEGAL_SIG(name, error)						\
	DeltaCompError(																\
		"In library function '%s' %s",											\
		ucstringarg(name), ucstringarg(error)									\
	)

#define	DELTACOMP_ERROR_LIBFUNC_ILLEGAL_BASES_DEFINITION(def, error)			\
	DeltaCompError(																\
		"In user-defined type '%s' %s",											\
		ucstringarg(def), ucstringarg(error)									\
	)

#define	DELTACOMP_ERROR_NAMESPACE_ILLEGAL_IDENT(name)							\
	DeltaCompError(																\
		"Illegal namespace ident '%s' after '%s'",								\
		ucstringarg(name),														\
		DELTA_LIBRARYNAMESPACE_ENTER											\
	)

#define	DELTACOMP_ERROR_NAMESPACE_NOT_FOUND(name)								\
	DeltaCompError(																\
		"In 'using' could not find namespace '%s' in any opened namespace",		\
		ucstringarg(name)														\
	)

#define	DELTACOMP_ERROR_USING_NAMESPACE_PATH_INVALID(fullPath)					\
	DeltaCompError(																\
		"In 'using' the namespace path '%s' is invalid",						\
		ucstringarg(fullPath)													\
	)

#define	DELTACOMP_ERROR_NAMESPACE_FUNCTION_NOT_FOUND(ns, func)					\
	DeltaCompError(																\
		"Could not find library function '%s' within namespace '%s'",			\
		ucstringarg(func), ucstringarg(ns)										\
	)

#define	DELTACOMP_ERROR_TYPE_INVALID_NAMESPACE_PATH(path, type)					\
	DeltaCompError(																\
		"Invalid namespace path '%s' in getting user-defined type '%s'",		\
		ucstringarg(path), ucstringarg(type)									\
	)

#define	DELTACOMP_ERROR_FIRST_IDENT_NOT_A_NAMESPACE(path, id, type)				\
	DeltaCompError(																\
		"Invalid namespace path '%s' (first id '%s' is currently a '%s')",		\
		ucstringarg(path), ucstringarg(id), ucstringarg(type)					\
	)

#define	DELTACOMP_ERROR_LIBFUNC_INVALID_NAMESPACE_PATH(path, func)				\
	DeltaCompError(																\
		"Invalid namespace path '%s' in getting library function '%s'",			\
		ucstringarg(path), ucstringarg(func)									\
	)

#define	DELTACOMP_ERROR_HAS_ALREADY_BASE_TYPE(type, base)						\
	DeltaCompError(																\
		"In definition of type '%s' base '%s' already defined",					\
		ucstringarg(type), ucstringarg(base)									\
	)

#define	DELTACOMP_ERROR_TYPE_IN_NAMESPACE_PATH_NOT_FOUND(path, type)			\
	DeltaCompError(																\
		"In namespace via path '%s' no user-defined type '%s' is found",		\
		ucstringarg(path), ucstringarg(type)									\
	)

#define	DELTACOMP_ERROR_FUNCTION_IN_MULTIPLE_NAMESPACES(func)					\
	DeltaCompError(																\
		"Function '%s' found in more than one namespaces (use namespace path)",	\
		ucstringarg(func)														\
	)

#define	DELTACOMP_ERROR_TYPE_NOT_IN_CURRENT_OR_ENCLOSING_NAMESPACE(type, ns)	\
	DeltaCompError(																\
		"User-defined type '%s' not in current '%s' or enclosing namespace",	\
		ucstringarg(type), ucstringarg(ns) 										\
	)

#define	DELTACOMP_ERROR_USING_NAMESPACE_AMBIGUOUS(ns, opt1, opt2)				\
	DeltaCompError(																\
		"In 'using %s' namespace is ambiguous (can be '%s' or '%s')",			\
		ucstringarg(ns), ucstringarg(opt1), ucstringarg(opt2)					\
	)

#define	DELTACOMP_ERROR_NAMESPACE_CONFLICTS_BYTECODE_LIBRARY(ns, lib)				\
	DeltaCompError(																	\
		"Name conflict between opened namespace '%s' and byte code library '%s'",	\
		ucstringarg(ns), ucstringarg(lib)											\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_NAMESPACE_CONFLICT(id,ns)						\
	DeltaCompError(																		\
		"In 'using' byte code library, id '%s' conflicts with opened namespace '%s'",	\
		ucstringarg(id), ucstringarg(ns)												\
	)

#define	DELTACOMP_ERROR_CONFLICTS_VISIBLE_LIBRARY_SYMBOL(id1, t1, l, id2, t2)	\
	DeltaCompError(																\
		"Use of '%s' %s (defined at line %u) conflicts with visible %s '%s'",	\
		ucstringarg(id1), ucstringarg(t1), l, ucstringarg(t2), ucstringarg(id2)	\
	)

#define	DELTACOMP_ERROR_FUNCTION_NAMESPACE_PATH_AMBIGUOUS(path, func)			\
	DeltaCompError(																\
		"Function '%s%s' matches more than one paths (use namespace path)",		\
		ucstringarg(path), ucstringarg(func)									\
	)

#define	DELTACOMP_ERROR_USING_NAMESPACE_NOT_IN_GLOBAL_SCOPE()					\
	DeltaCompError("Illegal 'using' directive (not at global scope)")

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_REDEFINITION(id)						\
	DeltaCompError(																\
		"In 'using' byte code library with id '%s' (already defined)",			\
		ucstringarg(id)															\
	)

#define	DELTACOMP_ERROR_NO_BYTECODE_LOADING_PATH(file)							\
	DeltaCompError(																\
		"In 'using' don't know where to search byte code file '%s' "			\
		"(no byte code loading path supplied)",									\
		ucstringarg(file)														\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_NOT_FOUND(file, path)					\
	DeltaCompError(																\
		"In 'using' failed to locate byte code file '%s' in '%s' path",			\
		ucstringarg(file), ucstringarg(path)									\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_AMBIGUOUS(file,paths)					\
	DeltaCompError(																\
		"In 'using' ambiguous byte code file name '%s' (paths '%s' match)",		\
		ucstringarg(file), ucstringarg(paths)									\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_OPEN_FAILED(file)						\
	DeltaCompError(																\
		"In 'using' failed to open byte code file '%s' (check file access)",	\
		ucstringarg(file)														\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_READ_ERROR(file, error)				\
	DeltaCompError(																\
		"In 'using' byte code file '%s' loading failed (%s)",					\
		ucstringarg(file),	ucstringarg(error)									\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_VAR_IMMUTABLE(op, name)				\
	DeltaCompError(																\
		"Illegal lvalue in '%s' (it is a byte code library ident '%s') ",		\
		ucstringarg(op), ucstringarg(name)										\
	)

#define	DELTACOMP_ERROR_BYTECODE_LIBRARY_FUNC_NOT_FOUND(lib, func)				\
	DeltaCompError(																\
		"Function '%s' not found in byte code library '%s'",					\
		ucstringarg(func), ucstringarg(lib)										\
	)

#define	DELTACOMP_ERROR_LVALUE_NEEDED_IN_EXCEPTION_VAR(e)						\
	DeltaCompError(																\
		"In 'trap' an lvalue expression is required ('%s' is immutable)",		\
		ucstringarg(e)															\
	)

//////////////////////////////////////////////////////

#define	DELTACOMP_WARNING_SELF_INDEX(table, index)								\
	DeltaCompWarning(															\
		"in '%s.%s' the use of '%s' as a string index; is this the intent?",	\
		ucropstr(table->GetUnparsed(), 16).c_str(),								\
		ucstringarg(index),														\
		ucstringarg(DELTA_SELF_POINTER_ID)										\
	)

#define	DELTACOMP_WARNING_NEVER_IF()											\
	DeltaCompWarning("preceeding 'if (false)': statement unreachable")

#define	DELTACOMP_WARNING_ALWAYS_IF()											\
	DeltaCompWarning("preceeding 'if (true)': statement always taken")

#define	DELTACOMP_WARNING_ALWAYS_ELSE()											\
	DeltaCompWarning("preceeding 'if (false) else': 'else' always taken")

#define	DELTACOMP_WARNING_NEVER_ELSE()											\
	DeltaCompWarning("preceeding 'if (true) else': 'else' never taken")

#define	DELTACOMP_WARNING_WHILE_TRUE()											\
	DeltaCompWarning("preceeding 'while (true)': potentially infinite loop"	)

#define	DELTACOMP_WARNING_WHILE_FALSE()											\
	DeltaCompWarning("preceeding 'while (false)': loop never taken")

#define	DELTACOMP_WARNING_FOR_TRUE()											\
	DeltaCompWarning("preceeding 'for (; true; )': potentially infinite loop")

#define	DELTACOMP_WARNING_FOR_FALSE()											\
	DeltaCompWarning("preceeding 'for (; false; )': loop never taken")

#define	DELTACOMP_WARNING_LIBFUNC_UNKNOWN(func)									\
	DeltaCompWarning("In call to '%s'() name not a known library function",		\
		ucstringarg(DPTR(func)->strConst)										\
	)

#define	DELTACOMP_HANDLER_FORMALS_ACTUALS_MISMATCH(f, func, totalFormals, totalArgs)	\
	(*f)("In calling '%s' passed %s actuals than the defined formals",					\
		ucstringarg(func),																\
		(totalFormals) > (totalArgs) ? "less" : "more"									\
	)

#define	DELTACOMP_WARNING_FORMALS_ACTUALS_MISMATCH(func, totalFormals, totalArgs)		\
	DELTACOMP_HANDLER_FORMALS_ACTUALS_MISMATCH(											\
		&DeltaCompWarning, func, totalFormals, totalArgs								\
	)

#define	DELTACOMP_WARNING_MISSING_ACTUALS(func, error)	\
	DeltaCompWarning("In '%s' %s", ucstringarg(func), ucstringarg(error))

#define	DELTACOMP_HANDLER_LIBFUNC_NUMACTUALS_NOMATCH(f, func, numSigs, totalArgs)	\
	(*f)("In calling lib function '%s' none of the %u signatures has %u arguments",	\
		ucstringarg(func), (numSigs), (totalArgs)									\
	)

#define	DELTACOMP_WARNING_UNINITIALIZED_USED(name, possibly)					\
	DeltaCompWarning("Use of %suninitialized variable '%s'",					\
		possibly ? "possibly ": "",												\
		name																	\
	)

#define	DELTACOMP_WARNING_NOT_ALL_EXIT_PATHS_RETURN_A_VALUE(func)				\
	DeltaCompWarning("Not all exit paths of '%s' return a value",				\
		func->GetFunctionReadableName().c_str()									\
	)

#define	DELTACOMP_WARNING_INVALID_NAMESPACE_EXIT(pos)							\
	DeltaCompWarning(															\
		"Exiting namespace at position '%u' but not in a namespace (ignored)",	\
		pos																		\
	)

#define	DELTACOMP_WARNING_MISSING_NAMESPACE_EXITS()								\
	DeltaCompWarning(															\
		"Still in namespace while at end of function list (will auto exit)"	\
	)

#define	DELTACOMP_WARNING_TYPE_AT_EOF()											\
	DeltaCompWarning(															\
		"At end of library defs '%s' found; ignored",							\
		DELTA_LIBRARYNAMESPACE_TYPE												\
	)

#define	DELTACOMP_WARNING_CONST_AT_EOF()										\
	DeltaCompWarning(															\
		"At end of library defs '%s' found; ignored",							\
		DELTA_LIBRARYNAMESPACE_CONST											\
	)

#define	DELTACOMP_WARNING_ENTER_AT_EOF()										\
	DeltaCompWarning(															\
		"At end of function list '%s' found; ignored",							\
		DELTA_LIBRARYNAMESPACE_ENTER											\
	)

#define	DELTACOMP_WARNING_OPEN_NAMESPACE_LIBSYM_CONFLICT(ns, type, id)			\
	DeltaCompWarning(															\
		"After 'using %s' a name conflict for its %s '%s' is caused",			\
		ucstringarg(ns), ucstringarg(type), ucstringarg(id)						\
	)

#define	DELTACOMP_WARNING_OPEN_NAMESPACE_USERSYM_CONFLICT(ns, t1, id, t2, l)	\
	DeltaCompWarning(															\
		"After 'using %s' a name conflict for "									\
		"its %s '%s' with %s (defined at line %u) is caused",					\
		ucstringarg(ns), ucstringarg(t1), ucstringarg(id), ucstringarg(t2), l	\
	)

#define	DELTACOMP_WARNING_OPEN_NAMESPACE_INNER_CONFLICT(opened, inner)			\
	DeltaCompWarning(															\
		"After 'using %s' a name conflict for its namespace '%s' is caused",	\
		ucstringarg(opened), ucstringarg(inner)									\
	)

#define	DELTACOMP_WARNING_OUTERFUNC_CLOSUREVAR_STILL_VISIBLE(f1,id,f2,l)		\
	if (true) {																	\
	DeltaCompWarning(															\
		"In function '%s' id '%s' binds to closure var of outer function '%s'",	\
		ucstringarg(f1), ucstringarg(id), ucstringarg(f2)						\
	);																			\
	DeltaCompWarning(															\
		"Firstly used in '%s' at line %u in a block whose scope is inactive"	\
		" (but closure var '%s' still correctly accessible)",					\
		ucstringarg(f2), l, ucstringarg(id)										\
	);																			\
	} else

#define	DELTACOMP_WARNING_SELF_IS_CLOSUREVAR_ACCESS(func,self,method)			\
	DeltaCompWarning(															\
		"In function '%s' use of '%s' is closure var of outer method '%s'",		\
		ucstringarg(func), ucstringarg(self), ucstringarg(method)				\
	)

#define	DELTACOMP_WARNING_OUTERFUNC_CLOSUREVAR_ACCESS(f1,id,f2,l)				\
	if (true) {																	\
	DeltaCompWarning(															\
		"In function '%s' id '%s' binds to closure var of outer function '%s'",	\
		ucstringarg(f1), ucstringarg(id), ucstringarg(f2)						\
	);																			\
	DeltaCompWarning(															\
		"Firstly used in '%s' at line %u in a block that is still active",		\
		ucstringarg(f2), l														\
	);																			\
	} else

#define	DELTACOMP_WARNING_ORPHAN_METHOD_OVERWRITES_USER_VAR(id,line)			\
	DeltaCompWarning(															\
		"Orphan method '%s' overwrites user var '%s' (declared at line %u)",	\
		ucstringarg(id), ucstringarg(id), line									\
	)

#define	DELTACOMP_WARNING_REDUNDANT_LOCAL_IN_NON_EXPORTABLE_FUNCTION(sym)		\
	DeltaCompWarning(															\
		"In '%s' 'local' qualifier is redundant (non exportable anyway)",		\
		ucstringarg(sym->GetFunctionReadableName())								\
	)

#define	DELTACOMP_WARNING_EMPTY_STMT()											\
	DeltaCompWarning(															\
		"';' empty statement found, is this the intent?"						\
	)

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
