// ASTCreationHelpers.cpp
// Actions helpers for creating the Delta AST.
// ScriptFighter Project.
// Y. Lilis, May 2011.
// Moved here from ASTCreationActions.cpp (original version by A.Savidis)
//

#ifndef	ASTCREATIONHELPERS_H
#define	ASTCREATIONHELPERS_H

#include "DDebug.h"
#include "ustrings.h"
#include "ParseActions.h"

/////////////////////////////////////////////////////////////

#define	MAKE_FROM_NODELIST(tag, nodes)						\
	AST::Node* node = ASTFACTORY.NewNode(tag);				\
	if (nodes)												\
		DPTR(node)->AddChildren(nodes);						\
	return node

#define	MAKE_FROM_NODELIST_EX(tag, chTag, chId, nodes)		\
	AST::Node* node = ASTFACTORY.NewNode(tag);				\
	if (nodes)												\
		DPTR(node)->AddChildrenEx(nodes, chTag, chId);		\
	return node

#define	MAKE_WITH_ONE_CHILD(tag, name, child)				\
	AST::Node* node = ASTFACTORY.NewNode(tag);				\
	DASSERT(child);											\
	DPTR(node)->PushBack(child, name);						\
	return node

#define	MAKE_WITH_TWO_CHILDREN(tag, n1, c1, n2, c2)			\
	AST::Node* node = ASTFACTORY.NewNode(tag);				\
	DASSERT(c1 && c2);										\
	DPTR(node)->PushBack(c1, n1);							\
	DPTR(node)->PushBack(c2, n2);							\
	return node

#define	MAKE_WITH_ONE_OPTIONAL_CHILD(tag, name, child)		\
	AST::Node* node = ASTFACTORY.NewNode(tag);				\
	if (child)												\
		DPTR(node)->PushBack(child, name);					\
	return node

#define	MAKE_WITH_ONE_ATTR(tag, attr, val)					\
	AST::Node* node = ASTFACTORY.NewNode(tag);				\
	DASSERT(val);											\
	DPTR(node)->SetAttribute(attr, val);					\
	return node

#define	EXTEND_ARG(arg, tag, childId)						\
	arg = AST::Node::Extend(arg, tag, childId);

#define	MAKE_BINARY_OPERATION(op, lname, left, rname, right)\
	AST::Node* node = ASTFACTORY.NewNode(op);				\
	DASSERT(left && right);									\
	DPTR(node)->PushBack(left, lname);						\
	DPTR(node)->PushBack(right, rname);						\
	return node

#define	MAKE_UNARY_OPERATION(op, name, expr)				\
	AST::Node* node = ASTFACTORY.NewNode(op);				\
	DASSERT(expr);											\
	DPTR(node)->PushBack(expr, name);						\
	return node

#define	MAKE_NODELIST(l, i)									\
	if (!l)													\
		l = ASTFACTORY.NewNodeList();						\
	if (i)													\
		l->Add(i);											\
	return l

#define	MAKE_LEAF(tag)										\
	return ASTFACTORY.NewNode(tag)

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.