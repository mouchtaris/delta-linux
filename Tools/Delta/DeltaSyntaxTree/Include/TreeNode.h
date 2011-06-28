// TreeNode.h
// Generic untyped (type unsafe) flexible tree.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	TREENODE_H
#define	TREENODE_H

#include "utypes.h"
#include "DDebug.h"
#include <map>
#include <list>
#include <string>

//---------------------------------------------------------------

#if defined(SYNTAXTREELIB_IMPORT) && defined(SYNTAXTREELIB_EXPORT)
#error "Can't define both SYNTAXTREELIB_EXPORT and SYNTAXTREELIB_IMPORT!"
#endif

#if	defined(SYNTAXTREELIB_EXPORT)
#define	SYNTAXTREELIB_CLASS	UDLL_EXPORT_CPP_CLASS
#define	SYNTAXTREELIB_FUNC	UDLL_EXPORT_CPP
#define	SYNTAXTREELIB_CFUNC	UDLL_EXPORT_C
#elif defined(SYNTAXTREELIB_IMPORT)
#define	SYNTAXTREELIB_CLASS	UDLL_IMPORT_CPP_CLASS
#define	SYNTAXTREELIB_FUNC	UDLL_IMPORT_CPP
#define	SYNTAXTREELIB_CFUNC	UDLL_IMPORT_C
#else
#define	SYNTAXTREELIB_CLASS
#define	SYNTAXTREELIB_FUNC	extern
#define	SYNTAXTREELIB_CFUN	extern
#endif

/////////////////////////////////////////////////////////

class TreeVisitor;

class SYNTAXTREELIB_CLASS TreeNode {

	DFRIENDDESTRUCTOR()

	public:
	typedef std::map<std::string, void*>		Attrs;

	protected:
	typedef std::pair<std::string, TreeNode*>	Entry;
	typedef std::list<Entry>					Nodes;

	std::string			tag;
	Attrs				attrs;
	Nodes				children;
	TreeNode*			parent;

	bool				Invariant (void) const;
	void				SetParent (TreeNode* node) { DASSERT(!parent && node); parent = node; }
	void				ResetParent (void) { unullify(parent); }
	virtual void		Delete (void) { DDELETE(this); }

	TreeNode (const std::string& _tag) : tag(_tag), parent((TreeNode*) 0){ DASSERT(!tag.empty()); }
	virtual ~TreeNode(){}

	/////////////////////////////////////////////////////////

	public:
	const std::string&	GetTag				(void) const { return tag; }
	TreeNode*			GetChild			(util_ui32 i);
	TreeNode*			GetChild			(const std::string& id);
	util_ui32			GetTotalChildren	(void) const;
	TreeNode*			GetParent			(void) { return parent; }
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

	void*				GetAttribute (const std::string& id);
	template <typename Tscalar>
	void				SetAttribute (const std::string& id, Tscalar data)
							{ DASSERT(Invariant()); attrs[id] = (void*) data; }
	const Attrs&		GetAttributes (void) const 
							{ DASSERT(Invariant()); return attrs; }
	util_ui32			GetTotalAttributes (void) const 
							{ return (util_ui32) attrs.size(); } 

	bool				AcceptPostOrder		(TreeVisitor* visitor, const std::string& id = "");
	bool				AcceptPreOrder		(TreeVisitor* visitor, const std::string& id = "");

	static TreeNode*	New					(const std::string& tag);
	static void			Delete				(TreeNode*& node);	// Nullifies arg too.
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
