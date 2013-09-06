// PointcutParseActions.h
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#ifndef POINTCUT_PARSE_ACTIONS_H
#define POINTCUT_PARSE_ACTIONS_H

#include "Pointcut.h"

Pointcut*	Manage_AND(Pointcut* left, Pointcut* right);
Pointcut*	Manage_OR	(Pointcut* left, Pointcut* right);
Pointcut*	Manage_NOT(Pointcut* pointcut);

//****************************

Pointcut*	Manage_AST			(const std::string& type);
Pointcut*	Manage_ATTRIBUTE	(void);
Pointcut*	Manage_CHILD		(Pointcut* pointcut, const std::string& index = "");
Pointcut*	Manage_PARENT		(Pointcut* pointcut, const std::string& index = "");
Pointcut*	Manage_DESCENDANT	(Pointcut* pointcut);
Pointcut*	Manage_ASCENDANT	(Pointcut* pointcut);

//****************************

IdList*		Manage_IdList (IdList* l = (IdList*) 0, const char* id = (const char*) 0);

//****************************

IdList*		Manage_Arguments			(IdList* l, const char* suffix);
IdList*		Manage_ArgumentsEmpty		(void);
IdList*		Manage_ArgumentPatternList	(IdList* l, const std::string& pattern);
IdList*		Manage_ArgumentPattern		(const std::string& pattern);

//****************************

Pointcut*	Manage_FuncPattern		(const std::string& funcClass, const std::string& name, IdList* formals);

Pointcut*	Manage_NormalCall		(IdList* qualifiedName, IdList* args);
Pointcut*	Manage_MethodCall		(const std::string& object, bool localLookup, const std::string& name, IdList* args);

Pointcut*	Manage_EXCEPTION		(const std::string pattern);
Pointcut*	Manage_CLASS			(const std::string pattern);

//****************************

#endif