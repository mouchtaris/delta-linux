// BuildServer.h
// Build server and thread implementation.
// Y.Lilis, February 2012.
//

#ifndef	BUILDSERVER_H
#define	BUILDSERVER_H

#include "Common.h"
#include "utypes.h"
#include "ufunctors.h"
#include <map>
#include <set>
#include <functional>
#include <string>

#define	BUILD_SERVER_PORT	0	// Auto assign.

/////////////////////////////////////////////////////////////

enum ClientToServerRequest {
	Build_AddSource				= 0,
	Build_BuildSource			= 1,
	Build_GetTransformations	= 2,
	Build_GetLineMappings		= 3,
	Build_GetSourceReferences	= 4,
	Build_RequestInvalid		= 5
};

enum ServerToClientResponse {
	Build_SourceAdded			= 0,	
	Build_StageBinary			= 1,	// FIXME: is it
	Build_Transformations		= 2,
	Build_LineMappings			= 3,
	Build_SourceReferences		= 4,
	Build_ResponseInvalid		= 5
};

/////////////////////////////////////////////////////////////

class SocketPacketNetLink;

namespace ide
{

class Component;
class Client;
class BuildServer {

	/////////////////////////////////////////////////////////////

	public:

	typedef std::map< util_ui32, std::set<util_ui32> > LineMappings;
	typedef std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > > SourceReferences;

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

	static void HandleAddSource (Client* client);
	static void HandleBuildSource (Client* client);
	static void HandleGetTransformations (Client* client);
	static void HandleGetLineMappings (Client* client);
	static void HandleGetSourceReferences (Client* client);

	//************************
	// Request processing functions.
	//
	static bool		GetAddSource (
						void*				data,
						char**				origin,
						char**				target,						
						LineMappings*		lineMappings,
						SourceReferences*	sourceRefs,
						char**				type,
						util_ui32*			index,
						bool*				isFinal
					);

	static bool		GetString (ClientToServerRequest request, void* data, char **str);

	static bool		GetBuildSource (void* data, char** source);
	static bool		GetTransformationsSource (void* data, char** source);
	static bool		GetLineMappingsSource (void* data, char** source);
	static bool		GetSourceReferencesSource (void* data, char** source);

	//************************
	// Response functions.
	//
	static void		DoStageSourceResponse (Client* client);
	static void		DoBuildSourceResponse (Client* client, const std::string& binary, const std::string& bytecodePath, const std::string& dllimportPath);
	static void		DoTransformationsResponse (Client* client, const StdStringList& transformations);
	static void		DoLineMappingsResponse (Client* client, const LineMappings& lineMappings);
	static void		DoSourceReferencesResponse (Client* client, const SourceReferences& sourceRefs);

	//************************
	// Mischellaneous functions.
	//
	static void		ServiceLoop (void);
	static void		TryEstablishClientConnection (void* unused = (void*) 0);

	static void		TryAcceptNewClients(void);
	static bool		HandleIncomingMessages(void);
	static void		CleanUpClients (void);

	/////////////////////////////////////////////////////////////

	static void		AddSource(
						Client*				client,
						Component*			script,
						const std::string&	source,
						const StringList&	lineMappings,
						const StringList&	sourceRefs,
						const std::string&	type,
						util_ui32			index,
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
