// TreeNode.h
// Generic untyped (type unsafe) flexible tree.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	TREENODE_H
#define	TREENODE_H

#include "DeltaSyntaxTreeDefs.h"
#include "TreeAttribute.h"
#include "DDebug.h"
#include "ufunctors.h"
#include <map>
#include <list>
#include <string>

/////////////////////////////////////////////////////////

class TreeVisitor;

class SYNTAXTREELIB_CLASS TreeNode {

	DFRIENDDESTRUCTOR()

	public:
	typedef std::map<std::string, TreeAttribute*>	Attrs;
	typedef std::pair<util_ui32, std::string>		Index;

	protected:
	typedef std::pair<std::string, TreeNode*>		Entry;
	typedef std::list<Entry>						Nodes;

	std::string			tag;
	Attrs				attrs;
	Nodes				children;
	TreeNode*			parent;

	bool				Invariant (void) const;
	void				SetParent (TreeNode* node) { DASSERT(!parent && node); parent = node; }
	void				ResetParent (void) { unullify(parent); }
	virtual void		Delete (void) { DDELETE(this); }

	TreeNode (const std::string& _tag) : tag(_tag), parent((TreeNode*) 0){ DASSERT(!tag.empty()); }
	virtual ~TreeNode(){ std::for_each(attrs.begin(), attrs.end(), udestroy_second<Attrs::value_type>()); }

	/////////////////////////////////////////////////////////

	public:

	struct DefaultCreator { TreeNode* operator()(const std::string& tag) const { return TreeNode::New(tag); } };

	template <typename Creator>
	TreeNode*			Clone (Creator creator) const {	// Everything but the parent
		TreeNode* node = creator(tag);
		for (Attrs::const_iterator i = attrs.begin(); i != attrs.end(); ++i)
			node->SetAttribute(i->first, *i->second);
		for (Nodes::const_iterator i = children.begin(); i != children.end(); ++i)
			node->PushBack(i->second->Clone(creator), i->first);
		return node;
	}

	const std::string&	GetTag				(void) const { return tag; }
	TreeNode*			GetChild			(util_ui32 i);
	const TreeNode*		GetChild			(util_ui32 i) const;
	TreeNode*			GetChild			(const std::string& id);
	const TreeNode*		GetChild			(const std::string& id) const;
	const Index			GetChildIndex		(const TreeNode* node) const;
	util_ui32			GetTotalChildren	(void) const;
	TreeNode*			GetParent			(void) { return parent; }
	const TreeNode*		GetParent			(void) const { return parent; }
	TreeNode*			GetRoot				(void);
	const TreeNode*		GetRoot				(void) const;
	bool				IsDescendant		(const TreeNode* node) const;
	
	void				PushBack			(TreeNode* node, const std::string& id = "");
	void				PushFront			(TreeNode* node, const std::string& id = "");
	void				InsertAfter			(const std::string& after, TreeNode* node, const std::string& id = "");
	void				InsertBefore		(const std::string& before, TreeNode* node, const std::string& id = "");
	void				InsertAfter			(util_ui32 after, TreeNode* node, const std::string& id = "");
	void				InsertBefore		(util_ui32 before, TreeNode* node, const std::string& id = "");
	void				InsertAfter			(TreeNode* after, TreeNode* node, const std::string& id = "");
	void				InsertBefore		(TreeNode* before, TreeNode* node, const std::string& id = "");
	void				PopBack				(void);
	void				PopFront			(void);
	void				Remove				(const std::string& id);
	void				Remove				(util_ui32 i);
	void				Remove				(TreeNode* node);
	void				RemoveFromParent	(void);
	void				Replace				(TreeNode* oldNode, TreeNode* newNode);

	bool					HasAttribute	(const std::string& id) const;
	TreeAttribute*			GetAttribute	(const std::string& id);
	const TreeAttribute*	GetAttribute	(const std::string& id) const;
	void					SetAttribute	(const std::string& id, TreeAttribute* attribute);
	void					SetAttribute	(const std::string& id, const TreeAttribute& attribute);
	void					RemoveAttribute (const std::string& id);

	const Attrs&			GetAttributes (void) const 
								{ DASSERT(Invariant()); return attrs; }
	util_ui32				GetTotalAttributes (void) const 
								{ return (util_ui32) attrs.size(); } 

	bool				AcceptPostOrder		(TreeVisitor* visitor, const std::string& id = "");
	bool				AcceptPreOrder		(TreeVisitor* visitor, const std::string& id = "");

	virtual TreeNode*	Clone				(void) const;

	static TreeNode*	New					(const std::string& tag);
	static void			Delete				(TreeNode*& node);	// Nullifies arg too.
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
