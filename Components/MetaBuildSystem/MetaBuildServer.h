// MetaBuildServer.h
// Meta build server and thread implementation.
// Y.Lilis, February 2012.
//

#ifndef	METABUILDSERVER_H
#define	METABUILDSERVER_H

#include "Common.h"
#include "MetaBuildCommands.h"
#include "utypes.h"
#include "ufunctors.h"
#include <map>
#include <set>
#include <functional>
#include <string>

#define	BUILD_SERVER_PORT	0	// Auto assign.

/////////////////////////////////////////////////////////////

class SocketPacketNetLink;

namespace ide
{

class Component;
class Client;
class MetaBuildServer {

	/////////////////////////////////////////////////////////////

	public:

	typedef std::map< util_ui32, std::set<util_ui32> > LineMappings;
	typedef std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > > NodeToChainOfSourceLineOriginInfo;

	private:

	//************************
	// Dispatch (handler) functions.
	//
	typedef void (*RequestHandler) (Client* client);
	typedef std::map<ClientToServerRequest, RequestHandler> HandlerMap;

	static HandlerMap* handlers;
	static void InstallRequestHandlers (void);
	static void	DispatchRequest (Client* client);
	static ClientToServerRequest GetRequestType (void* data);

	static void HandleAttachSource (ClientToServerRequest request, Client* client);

	static void HandleAttachStageSource (Client* client);
	static void HandleAttachStageResult (Client* client);
	static void HandleAttachAspectResult (Client* client);
	static void HandleBuildStageSource (Client* client);
	static void HandleGetAspectResults (Client* client);
	static void HandleGetSourceLineMappings (Client* client);
	static void HandleGetNodeToChainOfSourceLineOriginInfo (Client* client);

	//************************
	// Request processing functions.
	//
	static bool		GetAttachSourceData (
						ClientToServerRequest				request,
						void*								data,
						char**								origin,
						char**								target,						
						LineMappings*						lineMappings,
						NodeToChainOfSourceLineOriginInfo*	info,
						char**								classId,
						bool*								isFinal
					);

	static bool		GetString (ClientToServerRequest request, void* data, char **str);

	static bool		GetSourceFromBuildStageSourceRequest (void* data, char** source);
	static bool		GetSourceFromGetAspectResultsRequest (void* data, char** source);
	static bool		GetSourceFromGetSourceLineMappingsRequest (void* data, char** source);
	static bool		GetSourceFromNodeToChainOfSourceLineOriginInfoRequest (void* data, char** source);

	//************************
	// Response functions.
	//
	static void		DoSourceIsAttachedResponse (Client* client);
	static void		DoBuildStageSourceResponse (Client* client, const std::string& binary, const std::string& bytecodePath, const std::string& dllimportPath);
	static void		DoAspectResultsResponse (Client* client, const StdStringList& transformations);
	static void		DoSourceLineMappingsResponse (Client* client, const LineMappings& lineMappings);
	static void		DoNodeToChainOfSourceLineOriginInfoResponse (Client* client, const NodeToChainOfSourceLineOriginInfo& info);

	//************************
	// Mischellaneous functions.
	//
	static void		ServiceLoop (void);
	static void		TryEstablishClientConnection (void* unused = (void*) 0);

	static void		TryAcceptNewClients(void);
	static bool		HandleIncomingMessages(void);
	static void		CleanUpClients (void);

	/////////////////////////////////////////////////////////////

	static void		AttachSource(
						Client*				client,
						Component*			script,
						const std::string&	source,
						const StringList&	lineMappings,
						const StringList&	sourceRefs,
						const std::string&	type,
						bool				isFinal
					);
	static Client*	GetClientFromStageSource(Component* script);

	public:
	static bool			Initialise (util_ui32 port);
	static void			CleanUp (void);
	static util_ui32	GetPort (void);

	static void			OnCompileFinished(Component* script, const UIntList& buildId);
};

}
#endif	// Do not add stuff beyond this point.
