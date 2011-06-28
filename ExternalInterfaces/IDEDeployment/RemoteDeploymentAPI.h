/**
 *	RemoteDeploymentAPI.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EXT_DEPLOYMENTAPI_H
#define EXT_DEPLOYMENTAPI_H

#include "IDEDeploymentCommon.h"
#include "RemoteValue.h"

namespace ext {

////////////////////////////////////////////////////////////////////////

class _DEPLOY_API RemoteDeploymentAPI {
public:
	static void Initialize (const std::string& host, unsigned short port);
	static void CleanUp (void);

	struct InvalidArgumentsException {};
	struct InvalidReturnValueException {};
	struct CallFailedException {};

	static void Call (
		const std::string& classId,
		const std::string& function,
		const RemoteValueList& arguments = RemoteValueList(),
		RemoteValue *retval = (RemoteValue *) 0
	);

	static void Call (
		const RemoteHandle& handle,
		const std::string& function,
		const RemoteValueList& arguments = RemoteValueList(),
		RemoteValue *retval = (RemoteValue *) 0
	);

private:

	//******************************************************************
	// The following functions shall not be implemented
	//
	RemoteDeploymentAPI (void);
	~RemoteDeploymentAPI (void);

	RemoteDeploymentAPI (const RemoteDeploymentAPI&);
	RemoteDeploymentAPI& operator= (const RemoteDeploymentAPI&);
};

////////////////////////////////////////////////////////////////////////

} // namespace ext

#endif // EXT_DEPLOYMENTAPI_H
