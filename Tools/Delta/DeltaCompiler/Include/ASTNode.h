// ASTNode.h
// Generic node covering all cases of Delta ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	ASTNODE_H
#define	ASTNODE_H

#include "utypes.h"
#include "DDebug.h"
#include "TreeNode.h"
#include "AutoCollection.h"
#include <map>
#include <string>

// This is a source line that is taken during parsing,
// being the first line of an expression that is translated
// to quads, and is then forced as the line of the first
// generated quad of this expression.
#define	DELTA_AST_LINE_ATTRIBUTE	"line"

// This is stored during AST creation and is used
// upon translation to restore the source line
// for every processed node.
#define	DELTA_AST_NEWLINE_ATTRIBUTE	"newline"

namespace AST {

class Node;

/////////////////////////////////////////////////////////

template <typename T> struct ItemList : public AutoCollectable {
	std::list<T> l;
	void				Add (const T& x) 
							{ l.push_back(x); }
	static ItemList*	New (const T& x) 
							{ return DNEWCLASS(ItemList, (x)); }
	ItemList (const T& x) { Add(x); }
	~ItemList(){}
};

typedef ItemList<std::string>	IdList;
typedef ItemList<std::string>	StringList;
typedef ItemList<Node*>			NodeList;

#define	NIL_NODE		(Node*) 0
#define	NIL_NODELIST	(NodeList*) 0
#define	NIL_IDLIST		(IdList*) 0
#define	NIL_STRINGLIST	(StringList*) 0

/////////////////////////////////////////////////////////

template <typename T> struct Item : public AutoCollectable {
	T val;
	T&				Get (void) 
						{ return val; }
	static Item*	New (const T& x) 
						{ return DNEWCLASS(Item, (x)); }
	Item (const T& x) : val(x){}
	~Item(){}
};

typedef Item<double>		NumItem;
typedef Item<std::string>	StringItem;

/////////////////////////////////////////////////////////

class Node : public TreeNode, public AutoCollectable {

	public:
	util_ui32		GetLine (void) const
						{ return (util_ui32) const_cast<Node*>(this)->GetAttribute(DELTA_AST_LINE_ATTRIBUTE); }
	util_ui32		GetNewLine (void) const
						{ return (util_ui32) const_cast<Node*>(this)->GetAttribute(DELTA_AST_NEWLINE_ATTRIBUTE); }
	void			SetLine (util_ui32 line) 
						{ DASSERT(line); SetAttribute(DELTA_AST_LINE_ATTRIBUTE, (void*) line); }
	void			SetNewLine (util_ui32 line) 
						{ DASSERT(line); SetAttribute(DELTA_AST_NEWLINE_ATTRIBUTE, (void*) line); }
	void			AddChildren (NodeList* nodes);
	void			AddChildrenEx (NodeList* nodes, const std::string& childTag, const std::string& childId);
	static Node*	Extend (Node* child, const std::string& tag, const std::string& childId);

	virtual void	Delete (void)
						{ DDELETE(this); }

	Node (const std::string& _tag);
	virtual ~Node(){}
};

}	// AST namespace
/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
