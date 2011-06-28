// TeeNode.cpp
// Generic untyped (type unsafe) flexible tree.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#include "TreeNode.h"
#include "TreeVisitor.h"
#include "ustlhelpers.h"
#include <functional>
#include <algorithm>

/////////////////////////////////////////////////////////

struct IsDescendantPred : public std::binary_function<std::pair<std::string, TreeNode*>, const TreeNode*, bool> {
	bool operator()(const std::pair<std::string, TreeNode*>& me, const TreeNode* node) const
		{ return me.second == node || me.second->IsDescendant(node); }
};

struct MatchesIdPred : public std::binary_function<std::pair<std::string, TreeNode*>, std::string, bool> {
	bool operator()(const std::pair<std::string, TreeNode*>& me, const std::string& id) const
		{ return me.first== id; }
};

struct MatchesInstPred : public std::binary_function<std::pair<std::string, TreeNode*>, const TreeNode*, bool> {
	bool operator()(const std::pair<std::string, TreeNode*>& me, const TreeNode* node) const
		{ return me.second == node; }
};

/////////////////////////////////////////////////////////

TreeNode* TreeNode::New (const std::string& tag) 
	{ return DNEWCLASS(TreeNode, (tag)); }

void TreeNode::Delete (TreeNode*& node) {

	DASSERT(node);
	
	if (DPTR(node)->GetParent())
		DPTR(node)->RemoveFromParent();
	
	for (Nodes::iterator i = node->children.begin(); i != node->children.end(); ++i) {
		DPTR(i->second)->ResetParent(); 
		Delete(i->second);
	}

	node->Delete();
	unullify(node);
}

/////////////////////////////////////////////////////////

bool TreeNode::Invariant (void) const 
	{ return !tag.empty(); }

/////////////////////////////////////////////////////////


bool TreeNode::IsDescendant (const TreeNode* node) const {
	return	std::find_if(
				children.begin(),
				children.end(),
				std::bind2nd(IsDescendantPred(), node)
			) != children.end();
}

/////////////////////////////////////////////////////////

void* TreeNode::GetAttribute (const std::string& id) {
	Attrs::iterator i = attrs.find(id);
	return i != attrs.end() ? i->second : (void*) 0;
}

/////////////////////////////////////////////////////////

util_ui32 TreeNode::GetTotalChildren (void) const
	{ DASSERT(Invariant()); return (util_ui32) children.size(); }

//**********************

TreeNode* TreeNode::GetChild (util_ui32 i) {
	DASSERT(Invariant() && i < GetTotalChildren());
	return ulistgetbyindex(children, i).second;
}

//**********************

TreeNode* TreeNode::GetChild (const std::string& id) {
	DASSERT(Invariant() && !id.empty());
	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesIdPred(), id)
						);
	return i != children.end() ? i->second : (TreeNode*) 0;
}

/////////////////////////////////////////////////////////

void TreeNode::PushBack (TreeNode* node, const std::string& id) {
	DASSERT(Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());
	children.push_back(Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::PushFront (TreeNode* node, const std::string& id) {
	DASSERT(Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());
	children.push_front(Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::InsertBefore (const std::string& before, TreeNode* node, const std::string& id) {

	DASSERT(Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());

	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesIdPred(), before)
						);	
	DASSERT(i != children.end());

	children.insert(i, Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::InsertBefore (util_ui32 before, TreeNode* node, const std::string& id) {

	DASSERT(Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());
	
	Nodes::iterator i =	ulistgetiterbyindex(children, before);
	DASSERT(i != children.end());

	children.insert(i, Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::InsertBefore (TreeNode* before, TreeNode* node, const std::string& id) {

	DASSERT(before->Invariant() && Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());
	
	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesInstPred(), before)
						);	
	DASSERT(i != children.end());

	children.insert(i, Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::InsertAfter (const std::string& after, TreeNode* node, const std::string& id) {

	DASSERT(Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());

	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesIdPred(), after)
						);	
	DASSERT(i != children.end());

	++i;
	children.insert(i, Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::InsertAfter (util_ui32 after, TreeNode* node, const std::string& id) {
	
	DASSERT(Invariant() && (id.empty() || !GetChild(id)));

	Nodes::iterator i =	ulistgetiterbyindex(children, after);
	DASSERT(i != children.end());

	++i;
	children.insert(i, Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::InsertAfter (TreeNode* after, TreeNode* node, const std::string& id) {

	DASSERT(after->Invariant() && Invariant() && (id.empty() || !GetChild(id)) && !node->GetParent());

	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesInstPred(), after)
						);	
	DASSERT(i != children.end());

	++i;
	children.insert(i, Entry(id, node));
	node->SetParent(this);
}

//**********************

void TreeNode::PopBack (void) {
	DASSERT(Invariant() && !children.empty() && children.back().second->GetParent() == this);
	children.back().second->ResetParent();
	children.pop_back();
}

//**********************

void TreeNode::PopFront (void) {
	DASSERT(Invariant() && !children.empty() && children.front().second->GetParent() == this);
	children.front().second->ResetParent();
	children.pop_front();
}

//**********************

void TreeNode::Remove (const std::string& id) {

	DASSERT(Invariant() && !children.empty());

	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesIdPred(), id)
						);
	DASSERT(i != children.end() && i->second->GetParent() == this);

	i->second->ResetParent();
	children.erase(i);
}

//**********************

void TreeNode::Remove(util_ui32 i) {

	DASSERT(Invariant() && !children.empty());

	Nodes::iterator j =	ulistgetiterbyindex(children, i);
	DASSERT(j != children.end() && j->second->GetParent() == this);

	j->second->ResetParent();
	children.erase(j);
}

//**********************

void TreeNode::Remove (TreeNode* node) {

	DASSERT(Invariant() && !children.empty());

	Nodes::iterator i =	std::find_if(
							children.begin(), 
							children.end(), 
							std::bind2nd(MatchesInstPred(), node)
						);
	DASSERT(i != children.end() && i->second->GetParent() == this);

	i->second->ResetParent();
	children.erase(i);
}

//**********************

void TreeNode::RemoveFromParent (void) {
	DASSERT(Invariant() && GetParent());
	GetParent()->Remove(this);
}

/////////////////////////////////////////////////////////

bool TreeNode::AcceptPostOrder (TreeVisitor* visitor, const std::string& childId) {
	for (Nodes::iterator i = children.begin();  i != children.end(); ++i)
		if (!(i->second)->AcceptPostOrder(visitor, i->first))
			return false;
	return	visitor->OnVisitEntering(this, childId) != TreeVisitor::VisitStopped && 
			visitor->OnVisitLeaving(this, childId) != TreeVisitor::VisitStopped;
}

/////////////////////////////////////////////////////////

bool TreeNode::AcceptPreOrder (TreeVisitor* visitor, const std::string& childId) {

	switch (visitor->OnVisitEntering(this, childId)) {

		case TreeVisitor::VisitStopped:	
			return false;

		case TreeVisitor::VisitContinue:
			for (Nodes::iterator i = children.begin();  i != children.end() && !visitor->ShouldLeave(); ++i)
				if (!(i->second)->AcceptPreOrder(visitor, i->first))
					return false;
			break;

		case TreeVisitor::VisitLeave: 
			{ visitor->Continue(); break; }

		default:
			DASSERT(false);
	}

	return visitor->OnVisitLeaving(this, childId) != TreeVisitor::VisitStopped; 
}

/////////////////////////////////////////////////////////
