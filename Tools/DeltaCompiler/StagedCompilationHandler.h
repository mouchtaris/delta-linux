// StagedCompilationHandler.h
// Handler for stage compilation callbacks.
// Y.Lilis, February 2012.
//

#ifndef	STAGEDCOMPILATIONHANDLER_H
#define	STAGEDCOMPILATIONHANDLER_H

#include "DeltaMetaCompiler.h"

class StagedCompilationHandler {
public:
	typedef DeltaMetaCompiler::LineMappings	LineMappings;
	typedef std::list<std::string> StringList;
private:
	static std::string	host;
	static util_ui32	port;
	static std::string	originalSource;
	static std::string	outputPath;

	static void						AttachSource(
										const std::string&								source,
										const std::string&								text,
										const LineMappings&								lineMappings,
										const AST::NodeToChainOfSourceLineOriginInfo&	info,
										bool											isStage,
										bool											isFinal
									);

	static void						BuildSource (
										const std::string&	source,
										std::string*		binary,
										std::string*		bytecodePath,
										std::string*		dllimportPath
									);

	static const StringList								GetAspectResultsForSource (const std::string& source);
	static const LineMappings							GetSourceLineMappingsForSource (const std::string& source);
	static const AST::NodeToChainOfSourceLineOriginInfo	GetNodeToChainOfSourceLineOriginInfoForSource (const std::string& source);

	static void						UpdateInlineReferences (const std::string& source, DeltaMetaCompiler* compiler);
public:
	static void						OnParse (AST::Node* ast, void* closure);
	static DeltaVirtualMachine*		OnStageSource(
										const std::string&								source,
										const std::string&								text,
										const LineMappings&								lineMappings,
										const AST::NodeToChainOfSourceLineOriginInfo&	sourceRefs,
										void*											closure
									);

	static bool						OnStageResult(
										const std::string&								source,
										const std::string&								text,
										const LineMappings&								lineMappings,
										const AST::NodeToChainOfSourceLineOriginInfo&	sourceRefs,
										bool											final,
										void*											closure
									);

	static void RegisterSource (const std::string& soure, const std::string& stageOutputPath, DeltaMetaCompiler* compiler);

	static void Initialise (const std::string& host, util_ui32 port);
	static void CleanUp (void);
};

#endif	// Do not add stuff beyond this point.