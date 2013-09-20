// BuildClient.h
// Client for remote communication with the Build server.
// Y.Lilis, February 2012.
//

#ifndef	BUILDCLIENT_H
#define	BUILDCLIENT_H

#include "MetaBuildCommands.h"
#include "ASTChainOfSourceLineOriginInfo.h"
#include "utypes.h"
#include "ufunctors.h"
#include <string>
#include <map>
#include <set>
#include <functional>

///////////////////////////////////////////////////////////////////

class BuildClient {
	public:
	typedef std::list<std::string> StringList;
	typedef std::map< util_ui32, std::set<util_ui32> > LineMappings;
	struct BrokenConnection {};

	private:
	static void	SendString (ClientToServerRequest request, const std::string& str);
	static void	DoAttachSource (
					const std::string&								primarySymbolicURI,
					const std::string&								symbolicURI,
					const LineMappings&								lineMappings,
					const AST::NodeToChainOfSourceLineOriginInfo&	info,
					const std::string&								classId,
					bool											isResultFinal = false	// applicable only for result sources
				);

	public:

	// Start-up, connection and clean-up
	//
	static void			Initialise (void);
	static bool			Connect (const std::string& host, util_ui32 port);
	static void			CleanUp (void);

	// Client functions.
	static void			DoAttachStageSource (
							const std::string&								primarySymbolicURI,
							const std::string&								symbolicURI,
							const LineMappings&								lineMappings,
							const AST::NodeToChainOfSourceLineOriginInfo&	info
						);
	static void			DoAttachStageResult (
							const std::string&								primarySymbolicURI,
							const std::string&								symbolicURI,
							const LineMappings&								lineMappings,
							const AST::NodeToChainOfSourceLineOriginInfo&	info,
							bool											isFinal
						);
	static void			DoAttachAspectResult (
							const std::string&								primarSymbolicURI,
							const std::string&								symbolicURI,
							const LineMappings&								lineMappings,
							const AST::NodeToChainOfSourceLineOriginInfo&	info,
							bool											isFinal
						);

	static void			DoBuildStageSource (const std::string& symbolicURI);
	static void			DoGetAspectResults (const std::string& symbolicURI);
	static void			DoGetSourceLineMappings (const std::string& symbolicURI);
	static void			DoGetNodeToChainOfSourceLineOriginInfo (const std::string& symbolicURI);

	// Checking for response availability, doing local receipt and querying type.
	static bool						IsResponsePending (void);	// Check this first.
	static bool						ReceiveResponse (void);		// Then receive response locally.
	static ServerToClientResponse	GetResponseType (void);		// Query response always after receiving.
	static void						ResponseProcessed (void);	// Call this at the end, after GetInfo called.

	// Response specific receive functions.
	static bool			GetStageBinaryInfo (std::string* stageBinary, std::string* bytecodePath, std::string* dllimportPath);
	static bool			GetAspectResults (StringList& results);
	static bool			GetSourceLineMappings (LineMappings& lineMappings);
	static bool			GetNodeToChainOfSourceLineOriginInfo (AST::NodeToChainOfSourceLineOriginInfo& info);

	// Mischellaneous.
	static void			WaitAnyMessage (void);
	static bool			IsConnectionBroken (void);
};

#endif	// Do not add stuff beyond this point.
