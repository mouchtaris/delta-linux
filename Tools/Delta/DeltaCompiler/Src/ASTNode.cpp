// ASTNode.cpp
// Generic node covering all cases of Delta ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "ASTNode.h"
#include "ParseParms.h"
#include "ucallbacks.h"

/////////////////////////////////////////////////////////

void AST::Node::AddSourceReference (const SourceInfo& info, bool front) {
	DASSERT(!info.first.empty() && info.second);
	struct SourceInfoReferencesTraits {
		static void* Copier(void* val) {
			SourceInfoReferences* refs = (SourceInfoReferences*) val;
			return DNEWCLASS(SourceInfoReferences, (DPTR(refs)->begin(), DPTR(refs)->end()));
		}
		static void Cleaner(void* val) {
			SourceInfoReferences* refs = (SourceInfoReferences*) val;
			DPTR(refs);
			DDELETE(refs);
		}
	};
	if (TreeAttribute* attr = GetAttribute(DELTA_AST_SOURCE_REFERENCES_ATTRIBUTE)) {
		SourceInfoReferences* l = (SourceInfoReferences*) DPTR(attr)->GetPointer();
		if (front)
			DPTR(l)->push_front(info);
		else
			DPTR(l)->push_back(info);
	}
	else {
		attr = DNEWCLASS(
			TreeAttribute,
			(DNEWCLASS(SourceInfoReferences, (1, info)), SourceInfoReferencesTraits::Copier, SourceInfoReferencesTraits::Cleaner)
		);
		SetAttribute(DELTA_AST_SOURCE_REFERENCES_ATTRIBUTE, attr);
	}
}

void AST::Node::AddSourceReferences (const SourceInfoReferences& refs, bool front) {
	std::for_each(
		refs.begin(),
		refs.end(),
		ubind2nd(ubind1st(umemberfunctionpointer(&Node::AddSourceReference), this), front)
	);
}
const AST::Node::SourceInfoReferences* AST::Node::GetSourceReferences (void) const {
	const TreeAttribute* attr = GetAttribute(DELTA_AST_SOURCE_REFERENCES_ATTRIBUTE);
	return (const SourceInfoReferences*)(attr ? attr->GetPointer() : 0);
}

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
	if (const SourceInfoReferences* refs = DPTR(child)->GetSourceReferences())
		DPTR(node)->AddSourceReferences(*refs);
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

AST::Node* AST::Node::New (const std::string& tag) const {
	Node* node = DNEWCLASS(Node, (tag));
	node->SetAutoCollector(DPTR(collector));
	return node;
}

/////////////////////////////////////////////////////////

AST::Node::Node (const std::string& tag) : TreeNode(tag) {}

/////////////////////////////////////////////////////////

AST::NodeList* AST::Factory::NewNodeList (void) const {
	AutoCollectableFactory<NodeList> factory;
	factory.SetAutoCollector(DPTR(collector));
	return factory.New();
}

AST::Node* AST::Factory::NewNode (const std::string& tag) const {
	Node* node = DNEWCLASS(Node, (tag));
	node->SetAutoCollector(DPTR(collector));
	return node;
}

/////////////////////////////////////////////////////////