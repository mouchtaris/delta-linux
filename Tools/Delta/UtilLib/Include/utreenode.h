// utreenode.h
// A generic tree creation facility using a general purpose
// tree node class.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UTREENODE_H
#define	UTREENODE_H

#include "utypes.h"
#include <string>
#include <list>

//---------------------------------------------------------------

// This is a general purpose tree class, based on the model
// I use for trees: parent, next, prev, first child, last child.
// It requires optional string ids for each node, and supports
// void* data to be included per node, requiring a destructor
// and a copier function to be supplied.
//

/////////////////////////////////////////////////////////////////
// Destructor and copier function types.
//
typedef	void	(*utreenodedestructor) (void* data);
typedef void*	(*utreenodecopier) (void* data);

/////////////////////////////////////////////////////////////////
// The tree node type is visible to the developer.
// This is a conscious decision for maximum flexibility
// and efficiency. Traversal algorithms are to be 
// implemented easily on a need basis by the developers.
// Upon construction, a node can be empty, and can remain
// empty (i.e. no data or id) if this is needed. Such a node
// is perfectly legal.
//
struct UTILLIB_CLASS utreenode {

	std::string 			id;
	void*					data;

	utreenodedestructor		destructor;
	utreenodecopier			copier;

	utreenode*				parent;
	utreenode*				next;
	utreenode*				prev;
	std::list<utreenode*>	children;

	void				setid (const char* _id) { id = _id; }
	void				setdata (void* data);			// Old, if any and destructor, gets destroyed.
	void				cleardata (void);				// if destructor, gets destroyed.

	void				setdestructor (utreenodedestructor _destructor) { destructor = _destructor; }
	void				setcopier (utreenodecopier _copier) { copier = _copier; }
	
	utreenode (utreenode* _parent);
	virtual ~utreenode();	// The whole subtree gets deleted automatically.
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.

