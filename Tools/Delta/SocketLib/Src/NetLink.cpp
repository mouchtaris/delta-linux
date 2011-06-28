// NetLink.cpp
// Implementation of super-class members for net link.
// 2WEAR Project.
// A. Savidis, January 2003.
//

#include "DDebug.h"
#include "NetLink.h"
#include "uvalidator.h"
#include "ustlhelpers.h"
#include "usystem.h"

#include <list>
#include <algorithm>

//////////////////////////////////////////////////////////

uinstancevalidator<NetLink>* NetLink::netLinks;

void NetLink::Initialise (void) {
	DASSERT(!netLinks);
	netLinks = DNEW(uinstancevalidator<NetLink>);
}

void NetLink::CleanUp (void) {
	DASSERT(netLinks);
	DDELETE(netLinks);
	unullify(netLinks);
}

//////////////////////////////////////////////////////////

NetLink::NetLink (void) {
	buffer = (void*) 0;
	size = 0;
	DPTR(netLinks)->add(this);
}

NetLink::~NetLink () {
	Validate();
	ClearAllQueues();
	DPTR(netLinks)->remove(this);
}

/////////////////////////////////////////////////////////

void NetLink::ClearMsg (void) { 

	Validate();

	if (buffer && size) {
		DestroyDataBuffer(buffer);
		buffer	= (void*) 0;
		size	= 0;
	}
}

//////////////////////////////////////////////////////////

void NetLink::ReadMsg (void** _buffer, unsigned int* _size) { 

	Validate();

	DASSERT(buffer && size);

	*_buffer = buffer;
	*_size = size;
}

//////////////////////////////////////////////////////////

void NetLink::ClearAllQueues (void) { 

	Validate();

	ClearMsg();

	std::for_each(
		data.begin(),
		data.end(),
		DeleteDataFunctor()
	);

	data.clear();
}

//////////////////////////////////////////////////////////

bool NetLink::WaitAnyMessage (util_ui32 timeout) {
	util_ui32 ts = ugettime();
	while (1) {
		if (timeout && ugettime() - ts >= timeout)
			return false;
		if (IsConnectionBroken())
			return false;
		if (IsMsgPending()) 
			{ ReceiveMsg(); return true; }
	}
}

//////////////////////////////////////////////////////////
