// NetLink.h
// An abstraction of an already established communication
// link (or channel) for sending, receiving data, polling, and testing
// link validity. For each communicating partner requires its own link. 
// 2WEAR Project.
// A. Savidis, July 2002.
//

#ifndef	NETLINK_H
#define	NETLINK_H

#include <algorithm>
#include <functional>
#include <list>
#include "uvalidator.h"
#include "DDebug.h"
#include "SocketLibDefs.h"

class SOCKETLIB_CLASS NetLink {

	protected:
	struct Data {
		void*			buffer;
		unsigned int	size;

		Data (const Data& data) :
			buffer(data.buffer),
			size(data.size) {}

		Data (void* _buffer, unsigned int _size) :
			buffer(_buffer),
			size(_size) {}

		~Data(){}
	};

	struct DeleteDataFunctor : public std::unary_function<Data, void> {
		void operator() (const Data& data) {
			DDELARR((unsigned char*) data.buffer);
		}
	};

	void*						buffer;				// Currently received message.
	unsigned int				size;				// Size of the message.
	std::list<Data>				data;				// All data that are waiting to be received.
	static uinstancevalidator<NetLink>*	netLinks;	// For NetLink instance validation.

	void						Validate (void) 
									{ DPTR(netLinks)->validate(this); }							
	
	//**************************
	// Those are the two functions to be mainly used in derived class for
	// creating and destroying data buffers, if the original ClearMsg() is to be
	// used. Else, the latter function has to be overriden.
	//
	void*			CreateDataBuffer (unsigned int size) 
						{ return DNEWARR(char, size); }
	
	void			DestroyDataBuffer (void* data)
						{ DDELARR((char*) data);  }

	public:

	/////////////////////////////////////////////////////////////////////

	virtual bool		IsMsgPending (void) = 0;
	virtual void		ReceiveMsg (void) = 0;
	virtual void		BlockReceiveMsg (void) = 0;
	virtual void		SendMsg (void* data, unsigned int size) = 0;
	virtual bool		IsConnectionBroken (void) = 0;

	bool				WaitAnyMessage (util_ui32 timeout = 0);	// Template method relying on virtual functions.

	// Those are already implemented in the super-class.

	virtual void		ReadMsg (void** data, unsigned int* size);
	virtual void		ClearMsg (void);
	virtual void		ClearAllQueues (void);

	static void			Initialise (void);
	static void			CleanUp (void);

	NetLink (void);
	virtual ~NetLink();
};

#endif	// Do not add stuff beyond this point.
