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

//****************************

static bool MatchIdList(TreeNode* node, const IdList& ids) {
	DASSERT(node);
	const util_ui32 total = DPTR(node)->GetTotalChildren();
	if (ids.empty() && total > 0)
		return false;
	IdList::const_iterator iter = ids.begin();
	for (util_ui32 j = 0; j < total; ++j) {
		if (*iter == AOP_MATCH_MULTIPLE)
			break;
		TreeNode* child = DPTR(node)->GetChild(j);
		DASSERT(child);
		if (DPTR(child)->GetTag() != AST_TAG_NAME	||
			!MatchRegex(NAME(child), *iter)			||
			++iter == ids.end() && j != total - 1
		)
			return false;
	}
	return true;
}

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
		const util_ui32 totalArgs = DPTR(formals)->GetTotalChildren();
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

const std::string CallPointcut::GetChildName(TreeNode* node) {
	std::string name;
	if (node && DPTR(node)->GetTag() == AST_TAG_LVALUE_IDENT)
		if (TreeNode* nameNode = DPTR(node)->GetChild(AST_CHILD_NAME))
			if (DPTR(nameNode)->GetTag() == AST_TAG_NAME)
				name = NAME(nameNode);
	return name;
}

//****************************

bool CallPointcut::MatchArgs (TreeNode* call) const {
	if (TreeNode* args = DPTR(call)->GetChild(AST_CHILD_ACTUALS)) {
		const util_ui32 totalArgs = DPTR(args)->GetTotalChildren();
		if (argsPattern.empty() && totalArgs > 0)
			return false;
		IdList::const_iterator iter = argsPattern.begin();
		for (util_ui32 j = 0; j < totalArgs; ++j) {
			if (*iter == AOP_MATCH_MULTIPLE)
				break;			
			//TODO: When other arg types are supported check them here
			//For now just assert that only * is allowed
			DASSERT(*iter == "*");
			if (++iter == argsPattern.end() && j != totalArgs - 1)
				return false;
		}
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////

bool NormalCallPointcut::MatchQualifiedName(TreeNode* node, const IdList& namePattern) {
	bool result = false;
	if (node) {
		const std::string tag = DPTR(node)->GetTag();
		if (tag == AST_TAG_LVALUE_IDENT || tag == AST_TAG_LVALUE_LOCAL_IDENT || tag == AST_TAG_LVALUE_STATIC_IDENT) {
			const std::string name = GetChildName(node);
			result = !name.empty() && namePattern.size() == 1 && MatchRegex(name, namePattern.front());
		}
		else if (tag == AST_TAG_LVALUE_NAMESPACE_IDENT)
			result = MatchIdList(node, namePattern);
	}
	return result;
}

//****************************

const ASTSet NormalCallPointcut::Evaluate(TreeNode* ast) const {
	ASTSet result;
	const ASTSet nodes = NodeCollector(AST_TAG_FUNCTION_CALL)(ast);
	for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
		if (MatchQualifiedName(DPTR(*i)->GetChild(AST_CHILD_FUNCTION), namePattern) && MatchArgs(*i))
			result.insert(*i);
	return result;
}

/////////////////////////////////////////////////////////

const std::string MethodCallPointcut::GetDotIndexIdent(TreeNode* node) {
	std::string name;
	if (node && DPTR(node)->GetTag() == AST_TAG_DOTINDEX_IDENT)
		name = VALUE_STRING(node);
	return name;
}

//****************************

const std::string MethodCallPointcut::GetBracketConstString(TreeNode* node) {
	std::string name;
	if (node && DPTR(node)->GetTag() == AST_TAG_PRIMARY_EXPRESSION)
		if (TreeNode* constNode = DPTR(node)->GetChild(AST_CHILD_EXPR))
			if (DPTR(constNode)->GetTag() == AST_TAG_STRING_CONST)
				if (TreeAttribute* attr = DPTR(constNode)->GetAttribute(AST_ATTRIBUTE_ITEMS))
					if (DPTR(attr)->IsStringList()) {
						std::list<std::string> l = DPTR(attr)->GetStringList();
						for (std::list<std::string>::const_iterator i = l.begin(); i != l.end(); ++i)
							name.append(*i); 
					}
	return name;
}

//****************************

bool MethodCallPointcut::IsMethodCall(TreeNode* call, std::string* object, std::string* method, bool *localLookup) {
	if (TreeNode* func = DPTR(call)->GetChild(AST_CHILD_FUNCTION)) {
		const std::string tag = DPTR(func)->GetTag();
		bool isDot = tag == AST_TAG_TABLE_CONTENT_DOT || tag == AST_TAG_TABLE_CONTENT_DOUBLE_DOT;
		bool isBracket = tag == AST_TAG_TABLE_CONTENT_BRACKET || tag == AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET;
		if (isDot || isBracket) {
			std::string objName, funcName;
			if (!(objName = GetChildName(DPTR(func)->GetChild(AST_CHILD_TABLE))).empty()) {
				const std::string (*funcGetter)(TreeNode* node) = isDot ? GetDotIndexIdent : GetBracketConstString;
				if (!(funcName = (*funcGetter)(DPTR(func)->GetChild(AST_CHILD_INDEX))).empty()) {
					*object = objName;
					*method = funcName;
					*localLookup = tag == AST_TAG_TABLE_CONTENT_DOUBLE_DOT || tag == AST_TAG_TABLE_CONTENT_DOUBLE_BRACKET;
					return true;
				}
			}
		}
	}
	return false;
}

//****************************

const ASTSet MethodCallPointcut::Evaluate(TreeNode* ast) const {
	ASTSet result;
	const ASTSet nodes = NodeCollector(AST_TAG_FUNCTION_CALL)(ast);
	for (ASTSet::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
		std::string object;
		std::string method;
		bool local;
		if (IsMethodCall(*i, &object, &method, &local)	&&
			MatchRegex(object, objectPattern)			&&
			MatchRegex(method, namePattern)				&&
			local == localLookup						&&
			MatchArgs(*i)
		)
			result.insert(*i);
	}
	return result;
}

/////////////////////////////////////////////////////////
