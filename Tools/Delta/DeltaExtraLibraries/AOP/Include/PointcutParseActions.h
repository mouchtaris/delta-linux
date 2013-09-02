// PointcutParseActions.h
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#ifndef POINTCUT_PARSE_ACTIONS_H
#define POINTCUT_PARSE_ACTIONS_H

#include "Pointcut.h"

Pointcut* Manage_AND(Pointcut* left, Pointcut* right);
Pointcut* Manage_OR (Pointcut* left, Pointcut* right);
Pointcut* Manage_NOT(Pointcut* pointcut);

//****************************

Pointcut* Manage_AST(const std::string& type);
Pointcut* Manage_ATTRIBUTE();
Pointcut* Manage_CHILD(Pointcut* pointcut, const std::string& index = "");
Pointcut* Manage_PARENT(Pointcut* pointcut, const std::string& index = "");
Pointcut* Manage_DESCENDANT(Pointcut* pointcut);
Pointcut* Manage_ASCENDANT(Pointcut* pointcut);

//****************************

Pointcut* Manage_FuncPattern(const std::string& funcClass, const std::string& name, IdList* formals);
IdList* Manage_Formals(IdList* l, const char* suffix);
IdList* Manage_FormalsEmpty(void);
IdList* Manage_FormalPatternList(IdList* l, const std::string& pattern);
IdList* Manage_FormalPattern(const std::string& pattern);

//****************************

#endif