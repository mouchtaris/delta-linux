// MetaBuildCommands.h
// Meta build server and client commands.
// Y.Lilis, September 2013.
//
#ifndef META_BUILD_COMMANDS_H_
#define META_BUILD_COMMANDS_H_

///////////////////////////////////////////////////////////////////

enum ClientToServerRequest {
	MetaBuildSystem_AttachStageSource						= 0,
	MetaBuildSystem_AttachStageResult						= 1,
	MetaBuildSystem_AttachAspectResult						= 2,
	MetaBuildSystem_BuildStageSource						= 3,
	MetaBuildSystem_GetAspectResults						= 4,
	MetaBuildSystem_GetSourceLineMappings					= 5,
	MetaBuildSystem_GetNodeToChainOfSourceLineOriginInfo	= 6,
	MetaBuildSystem_RequestInvalid							= 7
};

///////////////////////////////////////////////////////////////////

enum ServerToClientResponse {
	MetaBuildSystem_SourceIsAttached						= 0,
	MetaBuildSystem_StageBinaryInfo							= 1,
	MetaBuildSystem_AspectResults							= 2,
	MetaBuildSystem_SourceLineMappings						= 3,
	MetaBuildSystem_NodeToChainOfSourceLineOriginInfo		= 4,
	MetaBuildSystem_ResponseInvalid							= 5
};

///////////////////////////////////////////////////////////////////

#define	STAGE_SOURCE_CLASS_ID	"StageSource"
#define	STAGE_RESULT_CLASS_ID	"StageResult"
#define	ASPECT_RESULT_CLASS_ID	"AspectResult"

///////////////////////////////////////////////////////////////////

#endif