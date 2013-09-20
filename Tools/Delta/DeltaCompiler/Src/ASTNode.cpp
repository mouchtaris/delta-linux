// ASTNode.cpp
// Generic node covering all cases of Delta ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "ASTNode.h"
#include "ASTChainOfSourceLineOriginInfo.h"
#include "ParseParms.h"
#include "ucallbacks.h"

/////////////////////////////////////////////////////////

void AST::Node::SetLocation (const Location& location) {
	if (location.startLine)
		SetStartLine(location.startLine);
	if (location.endLine)
		SetEndLine(location.endLine);
	if (location.startPos)
		SetStartPos(location.startPos);
	if (location.endPos)
		SetEndPos(location.endPos);
}

const AST::Node::Location AST::Node::GetLocation (void) const
	{ return Location(GetStartLine(), GetEndLine(), GetStartPos(), GetEndPos()); }

/////////////////////////////////////////////////////////

void AST::Node::SetSource (const std::string& source)
	{ DASSERT(!source.empty()); SetAttribute(DELTA_AST_SOURCE_ATTRIBUTE, source); }

const std::string AST::Node::GetSource (void) const
	{ return HasAttribute(DELTA_AST_SOURCE_ATTRIBUTE) ? GetAttribute(DELTA_AST_SOURCE_ATTRIBUTE)->GetString() : ""; }

/////////////////////////////////////////////////////////

#define IMPLEMENT_UINT_ATTRIBUTE(attr, getter, setter)						\
	util_ui32 AST::Node::getter (void) const								\
		{ return HasAttribute(attr) ? GetAttribute(attr)->GetUInt() : 0; }	\
	void AST::Node::setter (util_ui32 val)									\
		{ DASSERT(val); SetAttribute(attr, val); }

IMPLEMENT_UINT_ATTRIBUTE(DELTA_AST_LINE_ATTRIBUTE, GetLine, SetLine)
IMPLEMENT_UINT_ATTRIBUTE(DELTA_AST_STARTLINE_ATTRIBUTE, GetStartLine, SetStartLine)
IMPLEMENT_UINT_ATTRIBUTE(DELTA_AST_ENDLINE_ATTRIBUTE, GetEndLine, SetEndLine)
IMPLEMENT_UINT_ATTRIBUTE(DELTA_AST_STARTPOS_ATTRIBUTE, GetStartPos, SetStartPos)
IMPLEMENT_UINT_ATTRIBUTE(DELTA_AST_ENDPOS_ATTRIBUTE, GetEndPos, SetEndPos)

/////////////////////////////////////////////////////////

AST::Node* AST::Node::Extend (Node* child, const std::string& tag, const std::string& childId) {
	AST::Node* node = child->New(tag);
	DPTR(node)->PushBack(child, childId);
	DPTR(node)->SetLocation(DPTR(child)->GetLocation());
	if (util_ui32 line = DPTR(child)->GetLine())
		DPTR(node)->SetLine(line);
	const std::string source = DPTR(child)->GetSource();
	if (!source.empty())
		DPTR(node)->SetSource(source);
	if (const ChainOfSourceLineOriginInfo* info = GetChainOfSourceLineOrigin(child))
		AddChainOfSourceLineOrigin(node, *info);
	return node;
}

/////////////////////////////////////////////////////////

void AST::Node::AddChildrenEx (NodeList* nodes, const std::string& childTag, const std::string& childId) {

	DASSERT(nodes);

	std::list<AST::Node*>& l = DPTR(nodes)->l;
	DASSERT(!l.empty());

	for (std::list<AST::Node*>::iterator i = l.begin(); i != l.end(); ++i)
		PushBack(Extend(*i, childTag, childId));
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

TreeNode* AST::Node::Clone (void) const
	{ return TreeNode::Clone(ubind1st(umemberfunctionpointer(&Node::New), this)); }

/////////////////////////////////////////////////////////

AST::Node* AST::Node::New (const std::string& tag) const
	{ return DNEWCLASS(Node, (tag, DPTR(collector))); }

/////////////////////////////////////////////////////////

AST::Node::Node (const std::string& tag, AutoCollector* collector) :
	TreeNode(tag), AutoCollectable(collector) {}

/////////////////////////////////////////////////////////

AST::NodeList* AST::Factory::NewNodeList (void) const
	{ return AutoCollectableFactory<NodeList>(DPTR(collector)).New(); }

AST::Node* AST::Factory::NewNode (const std::string& tag) const
	{ return DNEWCLASS(Node, (tag, DPTR(collector))); }

/////////////////////////////////////////////////////////
