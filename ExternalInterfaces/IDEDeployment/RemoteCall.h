/**
 *	RemoteCall.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EXT_REMOTECALL_H
#define EXT_REMOTECALL_H

#include "Call.h"
#include "RemoteFunctionCaller.h"

namespace ext {

////////////////////////////////////////////////////////////////////////

template<class Signature>
class RCall : public ide::Call<Signature, RemoteFunctionCaller> {
public:
	RCall (void) {}
	RCall (const std::string& RemoteCallerId, const ide::Handle& handle, const std::string& function) :
		ide::Call<Signature, RemoteFunctionCaller>(RemoteCallerId, handle, function) {}
	RCall (const std::string& RemoteCallerId, const std::string& classId, const std::string& function) :
		ide::Call<Signature, RemoteFunctionCaller>(RemoteCallerId, classId, function) {}

};

////////////////////////////////////////////////////////////////////////

template<class Signature>
class RMCall : public ide::MCall<Signature, RemoteFunctionCaller> {
public:
	RMCall (void) {}
	RMCall (const std::string& RemoteCallerId, const ide::Handle& handle, const std::string& function) :
		ide::MCall<Signature, RemoteFunctionCaller>(RemoteCallerId, handle, function) {}
	RMCall (const std::string& RemoteCallerId, const std::string& classId, const std::string& function) :
		ide::MCall<Signature, RemoteFunctionCaller>(RemoteCallerId, classId, function) {}
};

////////////////////////////////////////////////////////////////////////

} // namespace ext

#endif // EXT_REMOTECALL_H
