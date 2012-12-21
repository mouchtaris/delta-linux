// ASTLib.h
// Garbage collectable ast type for delta.
// ScriptFighter Project.
// Y. Lilis, August 2011.
//

#ifndef	ASTLIB_H
#define	ASTLIB_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "DeltaStdClassNames.h"
#include "TreeNode.h"

//////////////////////////////////////////////////////////////////////

#define	DLIB_GET_AST_EX(name)	DLIB_GET_COLLECTABLESIMPLE_ARG(name, TreeNode, AST_TYPE_STR)
#define	DLIB_GET_AST			DLIB_GET_AST_EX(ast)

//////////////////////////////////////////////////////////////////////

DVM_FUNC void		RemoveRefCounters (TreeNode* node);
DVM_FUNC bool		HasRefCounter (TreeNode* node);

DVM_FUNC void		DeltaAST_Make (DeltaValue* at, TreeNode* ast);
DVM_FUNC TreeNode*	DeltaAST_Get (DeltaValue& obj);
DVM_FUNC TreeNode*	DeltaAST_Get (void* val, const std::string& typeStr);

//////////////////////////////////////////////////////////////////////

extern void Install_DeltaAST_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaAST_Lib (void);

#endif	// Do not add stuff beyond this point.