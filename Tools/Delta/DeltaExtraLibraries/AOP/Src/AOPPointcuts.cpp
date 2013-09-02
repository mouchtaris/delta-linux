// AOPPointcuts.cpp
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#include "AOPPointcuts.h"
#include "ASTTags.h"
#include "DeltaAOPLib.h"
#include "UtilVisitors.h"

#include <regex>

typedef std::tr1::regex Regex;

/////////////////////////////////////////////////////////

static Regex MakeRegex(const std::string& pattern) {
	std::string p;
	for (std::string::const_iterator i = pattern.begin(); i != pattern.end(); ++i)
		if (*i == '*') {
			if (p.empty() || p.back() != '*')
				p.append(".*");
		}
		else if (*i == '.')
			p.append("\\.");
		else
			p.append(std::string(1, *i));
	return Regex(p);
}

static inline bool MatchRegex(const std::string& val, const std::string& pattern)
	{ return std::tr1::regex_match(val, MakeRegex(pattern)); }

/////////////////////////////////////////////////////////

const ASTSet ExecutionPointcut::Evaluate(TreeNode* ast) const {
	ASTSet result;
	const ASTSet nodes = NodeCollector(AST_TAG_FUNCTION)(ast);
	for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		const std::string funcClass = DPTR(*i)->GetAttribute(AST_ATTRIBUTE_CLASS)->GetString();
		if (funcClassPattern != "*" && funcClass != funcClassPattern)
			continue;

		TreeNode* nameNode = DPTR(*i)->GetChild(AST_CHILD_NAME);
		if (nameNode && DPTR(nameNode)->GetTag() != AST_TAG_NAME)
			continue;
		const std::string name = nameNode ? NAME(nameNode) : "";
		if (!MatchRegex(name, namePattern))
			continue;

		TreeNode* formals = DPTR(*i)->GetChild(AST_CHILD_FORMALS);
		const util_ui32 totalArgs = DPTR(*i)->GetTotalChildren();
		if (formalsPattern.empty() && totalArgs > 0)
			continue;
		bool match = true;
		IdList::const_iterator iter = formalsPattern.begin();
		for (util_ui32 j = 0; j < totalArgs; ++j) {
			if (*iter == AOP_MATCH_MULTIPLE)
				break;
			TreeNode* child = DPTR(formals)->GetChild(j);
			DASSERT(child);
			if (DPTR(child)->GetTag() != AST_TAG_NAME					||
				!MatchRegex(NAME(child), *iter)							||
				++iter == formalsPattern.end() && j != totalArgs - 1
			) {
				match = false;
				break;
			}
		}
		if (match)
			result.insert(DPTR(*i));
	}

	return result;
}

/////////////////////////////////////////////////////////
