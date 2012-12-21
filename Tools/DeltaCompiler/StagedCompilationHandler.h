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
	typedef DeltaMetaCompiler::SourceReferences	SourceReferences;
	typedef std::list<std::string> StringList;
private:
	static std::string	host;
	static util_ui32	port;
	static std::string	originalSource;
	static std::string	outputPath;

	static void						AddSource(
										const std::string&		source,
										const std::string&		text,
										unsigned				stage,
										const LineMappings&		lineMappings,
										const SourceReferences& sourceRefs,
										bool					isStage,
										bool					isFinal
									);

	static void						BuildSource (
										const std::string&	source,
										std::string*		binary,
										std::string*		bytecodePath,
										std::string*		dllimportPath
									);

	static const StringList			GetTransformations (const std::string& source);
	static const LineMappings		GetLineMappings (const std::string& source);
	static const SourceReferences	GetSourceReferences (const std::string& source);

	static void						UpdateInlineReferences (const std::string& source, DeltaMetaCompiler* compiler);
public:
	static void						OnParse (AST::Node* ast, void* closure);
	static DeltaVirtualMachine*		OnStageSource(
										const std::string&		source,
										const std::string&		text,
										util_ui32				stage,
										const LineMappings&		lineMappings,
										const SourceReferences& sourceRefs,
										void*					closure
									);

	static bool						OnStageResult(
										const std::string&		source,
										const std::string&		text,
										util_ui32				stage,
										const LineMappings&		lineMappings,
										const SourceReferences& sourceRefs,
										bool					final,
										void*					closure
									);

	static void HandleSource (const std::string& soure, const std::string& stageOutputPath, DeltaMetaCompiler* compiler);

	static void Initialise (const std::string& host, util_ui32 port);
	static void CleanUp (void);
};

#endif	// Do not add stuff beyond this point.