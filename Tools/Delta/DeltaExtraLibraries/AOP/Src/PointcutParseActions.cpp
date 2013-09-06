// PointcutParseActions.cpp
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#include "PointcutParseActions.h"
#include "ASTPointcuts.h"
#include "AOPPointcuts.h"
#include "PointcutCombinators.h"
#include "DeltaAOPLib.h"

//****************************

Pointcut* Manage_AND(Pointcut* left, Pointcut* right) { return DNEWCLASS(AndPointcut, (left, right)); }
Pointcut* Manage_OR (Pointcut* left, Pointcut* right) { return DNEWCLASS(OrPointcut, (left, right)); }
Pointcut* Manage_NOT(Pointcut* pointcut) { return DNEWCLASS(NotPointcut, (pointcut)); }

//****************************

Pointcut* Manage_AST (const std::string& type) { return DNEWCLASS(ASTPointcut, (type)); }
Pointcut* Manage_ATTRIBUTE (void) { return DNEWCLASS(AttributePointcut, ()); }
Pointcut* Manage_CHILD (Pointcut* pointcut, const std::string& index) { return DNEWCLASS(ChildPointcut, (pointcut, index)); }
Pointcut* Manage_PARENT (Pointcut* pointcut, const std::string& index) { return DNEWCLASS(ParentPointcut, (pointcut, index)); }
Pointcut* Manage_DESCENDANT (Pointcut* pointcut) { return DNEWCLASS(DescendantPointcut, (pointcut)); }
Pointcut* Manage_ASCENDANT (Pointcut* pointcut) { return DNEWCLASS(AscendantPointcut, (pointcut)); }

//****************************

IdList* Manage_IdList (IdList* l, const char* id) {
	if (!l) l = DNEW(IdList);
	if (id) DPTR(l)->push_back(id);
	return l;
}

//****************************

IdList* Manage_Arguments (IdList* l, const char* suffix) {
	if (suffix) DPTR(l)->push_back(suffix);
	return l;
}

IdList* Manage_ArgumentsEmpty (void) { return DNEW(IdList); }

IdList* Manage_ArgumentPatternList (IdList* l, const std::string& pattern) {
	DPTR(l)->push_back(pattern);
	return l;
}

IdList* Manage_ArgumentPattern (const std::string& pattern) { return DNEWCLASS(IdList, (1, pattern)); }

//****************************

Pointcut* Manage_FuncPattern (const std::string& funcClass, const std::string& name, IdList* formals) {
	Pointcut* p = DNEWCLASS(ExecutionPointcut, (funcClass, name, *DPTR(formals)));
	DDELETE(formals);
	return p;
}

Pointcut* Manage_NormalCall (IdList* qualifiedName, IdList* args) {
	Pointcut* p = DNEWCLASS(NormalCallPointcut, (*DPTR(qualifiedName), *DPTR(args)));
	DDELETE(qualifiedName);
	DDELETE(args);
	return p;
}

Pointcut* Manage_MethodCall (const std::string& object, bool localLookup, const std::string& name, IdList* args) {
	Pointcut* p = DNEWCLASS(MethodCallPointcut, (object, name, *DPTR(args), localLookup));
	DDELETE(args);
	return p;
}

Pointcut* Manage_EXCEPTION (const std::string pattern) {
	//exception(id) is a shortcut
	//TODO
	DASSERT(false);
	return DNEWCLASS(AttributePointcut, ());
}

Pointcut* Manage_CLASS (const std::string pattern) {
	//class(id) is a shortcut for ast(TableConstructor) and descendant(ast(return)) and descendant(execution(function id(..)))
	Pointcut* p1 = Manage_AST(AST_TAG_TABLE_CONSTRUCTOR);
	Pointcut* p2 = Manage_DESCENDANT(Manage_AST(AST_TAG_RETURN));
	Pointcut* funcPattern = DNEWCLASS(ExecutionPointcut, (AST_VALUE_FUNCCLASS_PROGRAMFUNCTION, pattern, IdList(1, AOP_MATCH_MULTIPLE)));
	Pointcut* p3 = Manage_DESCENDANT(funcPattern);
	return Manage_AND(Manage_AND(p1, p2), p3);
}
