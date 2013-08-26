// AOPLibrary.h
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	AOP_LIBRARY_H
#define	AOP_LIBRARY_H

#include "TreeNode.h"

/////////////////////////////////////////////////////////

#define BEFORE_STR	"BEFORE"
#define AFTER_STR	"AFTER"
#define AROUND_STR	"AROUND"

/////////////////////////////////////////////////////////

class AOPLibrary {
public:
	enum AdviceType {
		BEFORE	= 1,
		AFTER	= 2,
		AROUND	= 3
	};
	typedef std::list<TreeNode*> ASTList;

	static bool IsValidAdviceType (const std::string& str);
	static AdviceType ToAdviceType (const std::string& str);

	static void Aspect(TreeNode* target, const std::string& pointcut, AdviceType type, TreeNode* advice);
	static ASTList Match(TreeNode* target, const std::string& pointcut);
	static void Advise(TreeNode* target, AdviceType type, TreeNode* advice);
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.