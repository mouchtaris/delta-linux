// PointcutParseActions.cpp
// ScriptFigher Project.
// Y. Lilis, August 2013.
//

#include "PointcutParseActions.h"
#include "ASTPointcuts.h"
#include "AOPPointcuts.h"
#include "PointcutCombinators.h"

//****************************

Pointcut* Manage_AND(Pointcut* left, Pointcut* right) { return new AndPointcut(left, right); }
Pointcut* Manage_OR (Pointcut* left, Pointcut* right) { return new OrPointcut(left, right); }
Pointcut* Manage_NOT(Pointcut* pointcut) { return new NotPointcut(pointcut); }

//****************************

Pointcut* Manage_AST(const std::string& type) { return new ASTPointcut(type); }
Pointcut* Manage_ATTRIBUTE() { return new AttributePointcut(); }
Pointcut* Manage_CHILD(Pointcut* pointcut, const std::string& index) { return new ChildPointcut(pointcut, index); }
Pointcut* Manage_PARENT(Pointcut* pointcut, const std::string& index) { return new ParentPointcut(pointcut, index); }
Pointcut* Manage_DESCENDANT(Pointcut* pointcut) { return new DescendantPointcut(pointcut); }
Pointcut* Manage_ASCENDANT(Pointcut* pointcut) { return new AscendantPointcut(pointcut); }

//****************************

