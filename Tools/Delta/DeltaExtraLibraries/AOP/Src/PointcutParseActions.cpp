// PointcutParseActions.cpp
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#include "PointcutParseActions.h"
#include "ASTPointcuts.h"
#include "AOPPointcuts.h"
#include "PointcutCombinators.h"

//****************************

Pointcut* Manage_AND(Pointcut* left, Pointcut* right) { return DNEWCLASS(AndPointcut, (left, right)); }
Pointcut* Manage_OR (Pointcut* left, Pointcut* right) { return DNEWCLASS(OrPointcut, (left, right)); }
Pointcut* Manage_NOT(Pointcut* pointcut) { return DNEWCLASS(NotPointcut, (pointcut)); }

//****************************

Pointcut* Manage_AST(const std::string& type) { return DNEWCLASS(ASTPointcut, (type)); }
Pointcut* Manage_ATTRIBUTE() { return DNEWCLASS(AttributePointcut, ()); }
Pointcut* Manage_CHILD(Pointcut* pointcut, const std::string& index) { return DNEWCLASS(ChildPointcut, (pointcut, index)); }
Pointcut* Manage_PARENT(Pointcut* pointcut, const std::string& index) { return DNEWCLASS(ParentPointcut, (pointcut, index)); }
Pointcut* Manage_DESCENDANT(Pointcut* pointcut) { return DNEWCLASS(DescendantPointcut, (pointcut)); }
Pointcut* Manage_ASCENDANT(Pointcut* pointcut) { return DNEWCLASS(AscendantPointcut, (pointcut)); }

//****************************

Pointcut* Manage_FuncPattern(const std::string& funcClass, const std::string& name, IdList* formals) {
	Pointcut* p = DNEWCLASS(ExecutionPointcut, (funcClass, name, *DPTR(formals)));
	DDELETE(formals);
	return p;
}

IdList* Manage_Formals(IdList* l, const char* suffix) {
	if (suffix) DPTR(l)->push_back(suffix);
	return l;
}

IdList* Manage_FormalsEmpty(void) { return DNEW(IdList); }

IdList* Manage_FormalPatternList(IdList* l, const std::string& pattern) {
	DPTR(l)->push_back(pattern);
	return l;
}

IdList* Manage_FormalPattern(const std::string& pattern) { return DNEWCLASS(IdList, (1, pattern)); }

//****************************

