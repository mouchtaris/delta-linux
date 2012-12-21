// BuildClient.h
// Client for remote communication with the Build server.
// Y.Lilis, February 2012.
//

#ifndef	BUILDCLIENT_H
#define	BUILDCLIENT_H

#include "utypes.h"
#include "ufunctors.h"
#include <string>
#include <map>
#include <set>
#include <functional>

///////////////////////////////////////////////////////////////////

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
	Build_StageBinary			= 1,
	Build_Transformations		= 2,
	Build_LineMappings			= 3,
	Build_SourceReferences		= 4,
	Build_ResponseInvalid		= 5
};

///////////////////////////////////////////////////////////////////

class BuildClient {
	private:
	static void	SendString (ClientToServerRequest request, const std::string& str);

	public:
	typedef std::list<std::string> StringList;
	typedef std::map< util_ui32, std::set<util_ui32> > LineMappings;
	typedef std::map< util_ui32, std::list< std::pair<std::string, util_ui32> > > SourceReferences;
	struct BrokenConnection {};

	// Start-up, connection and clean-up
	//
	static void			Initialise (void);
	static bool			Connect (const std::string& host, util_ui32 port);
	static void			CleanUp (void);

	// Client functions.
	static void			DoAddSource (
							const std::string&		originalSource,
							const std::string&		stageSource,							
							const LineMappings&		lineMappings,
							const SourceReferences&	sourceRefs,
							const std::string&		type,
							util_ui32				index,
							bool					isFinal
						);

	static void			DoBuildSource (const std::string& source);
	static void			DoGetTransformations (const std::string& source);
	static void			DoGetLineMappings (const std::string& source);
	static void			DoGetSourceReferences (const std::string& source);

	// Checking for response availability, doing local receipt and querying type.
	static bool						IsResponsePending (void);	// Check this first.
	static bool						ReceiveResponse (void);		// Then receive response locally.
	static ServerToClientResponse	GetResponseType (void);		// Query response always after receiving.
	static void						ResponseProcessed (void);	// Call this at the end, after GetInfo called.

	// Response specific receive functions.
	static bool			GetStageBinaryData (std::string* stageBinary, std::string* bytecodePath, std::string* dllimportPath);
	static bool			GetTransformations (StringList& transformations);
	static bool			GetLineMappings (LineMappings& lineMappings);
	static bool			GetSourceReferences (SourceReferences& sourceRefs);

	// Mischellaneous.
	static void			WaitAnyMessage (void);
	static bool			IsConnectionBroken (void);
};

#endif	// Do not add stuff beyond this point.
