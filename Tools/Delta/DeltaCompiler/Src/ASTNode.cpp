// ASTNode.cpp
// Generic node covering all cases of Delta ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "ASTNode.h"
#include "CompilerAPI.h"

/////////////////////////////////////////////////////////

AST::Node::Node (const std::string& _tag) : TreeNode(_tag) { 
	SetNewLine(DeltaCompiler::GetLine());	// Takes src line during syntax analysis.
}

/////////////////////////////////////////////////////////

AST::Node* AST::Node::Extend (Node* child, const std::string& tag, const std::string& childId) {
	AST::Node* node = DNEWCLASS(AST::Node, (tag));
	node->PushBack(child, childId);
	if (util_ui32 line = DPTR(child)->GetLine())
		DPTR(node)->SetLine(line);
	return node;
}

/////////////////////////////////////////////////////////

void AST::Node::AddChildrenEx (NodeList* nodes, const std::string& childTag, const std::string& childId) {

	DASSERT(nodes);

	std::list<AST::Node*>& l = DPTR(nodes)->l;
	DASSERT(!l.empty());

	for (std::list<AST::Node*>::iterator i = l.begin(); i != l.end(); ++i) {
		AST::Node* node = DNEWCLASS(AST::Node, (childTag));
		DPTR(node)->PushBack(*i, childId);
		if (util_ui32 line = DPTR(*i)->GetLine())
			DPTR(node)->SetLine(line);
		PushBack(node);
	}
}

/////////////////////////////////////////////////////////

void AST::Node::AddChildren (AST::NodeList* nodes) {

	DASSERT(nodes);

	std::list<AST::Node*>& l = DPTR(nodes)->l;
	DASSERT(!l.empty());

	for (std::list<AST::Node*>::iterator i = l.begin(); i != l.end(); ++i)
		PushBack(*i);
}

/////////////////////////////////////////////////////////
