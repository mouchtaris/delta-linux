/**
 *	RemoteDynamicCall.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EXT_REMOTEDYNAMICCALL_H
#define EXT_REMOTEDYNAMICCALL_H

#include "DynamicCall.h"
#include "RemoteFunctionCaller.h"

namespace ext {

////////////////////////////////////////////////////////////////////////

template<class R>
class RDCall : public ide::DCall<R, RemoteFunctionCaller> {
public:
	RDCall (void) {}
	RDCall (const std::string& callerId, const std::string& classId, const std::string& function) :
		ide::DCall<R, RemoteFunctionCaller>(callerId, classId, function) {}
};

//////////////////////////////////////////////////////////////////////

template<class R>
class RMDCall : public ide::MDCall<R, RemoteFunctionCaller> {
public:
	RMDCall (void) {}
	RMDCall (const std::string& callerId, const std::string& classId, const std::string& function) :
		ide::MDCall<R, RemoteFunctionCaller>(callerId, classId, function) {}
};

////////////////////////////////////////////////////////////////////////

} // namespace ext

#endif // EXT_REMOTEDYNAMICCALL_H
