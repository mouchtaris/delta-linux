// utreenode.cpp
// Tree node implementation.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#include "DDebug.h"
#include "utreenode.h"
#include "utypes.h"
#include "ufunctors.h"
#include <algorithm>

//---------------------------------------------------------------

utreenode::utreenode (utreenode* _parent) :

		data((void*) 0),
		destructor((utreenodedestructor) 0),
		copier((utreenodecopier) 0),
		parent(_parent),
		next((utreenode*) 0),
		prev((utreenode*) 0) { 

		if (parent)
			parent->children.push_back(this);
}

/////////////////////////////////////////////////////////////////

utreenode::~utreenode() {

	// Firstly diconnect from its parent list and
	// other collegues.
	//
	if (parent)
		parent->children.remove(this);
	if (next)
		DPTR(next)->prev = prev;
	if (prev)
		DPTR(prev)->next = next;

	std::for_each(
		children.begin(),
		children.end(),
		uptrdestructorfunctor<utreenode*>()
	);
}

/////////////////////////////////////////////////////////////////

void utreenode::setdata (void* _data) {

	if (data)
		if (destructor)
			(*destructor)(data);
	
	if (_data)
		if (copier)
			data = (*copier)(_data);
		else
			data = _data;
	else
		data = (void*) 0;
}

/////////////////////////////////////////////////////////////////

void utreenode::cleardata (void) {
	if (data && destructor)
		(*destructor)(data);
}

//---------------------------------------------------------------
