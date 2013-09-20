// StagedCompilationHandler.cpp
// Handler for stage compilation callbacks.
// Y.Lilis, February 2012.
//

#include "StagedCompilationHandler.h"
#include "BuildClient.h"
#include "DeltaVirtualMachine.h"
#include "ASTUtilVisitors.h"
#include "DeltaStdLib.h"

///////////////////////////////////////////////////////////////

std::string	StagedCompilationHandler::host;
util_ui32	StagedCompilationHandler::port;
std::string	StagedCompilationHandler::originalSource;
std::string	StagedCompilationHandler::outputPath;

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::AttachSource (
	const std::string&								source,
	const std::string&								text,
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	info,
	bool											isStage,
	bool											isFinal
) {
	DeltaMetaCompiler::DumpSource(outputPath + source, text);
	if (isStage)
		BuildClient::DoAttachStageSource(originalSource, source, lineMappings, info);
	else
		BuildClient::DoAttachStageResult(originalSource, source, lineMappings, info, isFinal);
	BuildClient::WaitAnyMessage();
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::BuildSource (const std::string& source, std::string* binary, std::string* bytecodePath, std::string* dllimportPath) {
	BuildClient::DoBuildStageSource(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == MetaBuildSystem_StageBinaryInfo)	//ignore unrecognized messages
		BuildClient::GetStageBinaryInfo(binary, bytecodePath, dllimportPath);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
}

///////////////////////////////////////////////////////////////

const StagedCompilationHandler::StringList StagedCompilationHandler::GetAspectResultsForSource (const std::string& source) {
	StringList aspectResults;
	BuildClient::DoGetAspectResults(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == MetaBuildSystem_AspectResults)
		BuildClient::GetAspectResults(aspectResults);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
	return aspectResults;
}

///////////////////////////////////////////////////////////////

const StagedCompilationHandler::LineMappings StagedCompilationHandler::GetSourceLineMappingsForSource (const std::string& source) {
	LineMappings lineMappings;
	BuildClient::DoGetSourceLineMappings(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == MetaBuildSystem_SourceLineMappings)
		BuildClient::GetSourceLineMappings(lineMappings);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
	return lineMappings;
}

///////////////////////////////////////////////////////////////

const AST::NodeToChainOfSourceLineOriginInfo StagedCompilationHandler::GetNodeToChainOfSourceLineOriginInfoForSource (const std::string& source) {
	AST::NodeToChainOfSourceLineOriginInfo info;
	BuildClient::DoGetNodeToChainOfSourceLineOriginInfo(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == MetaBuildSystem_NodeToChainOfSourceLineOriginInfo)
		BuildClient::GetNodeToChainOfSourceLineOriginInfo(info);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
	return info;
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::UpdateInlineReferences (const std::string& source, DeltaMetaCompiler* compiler) {
	DeltaMetaCompiler::InlineReferences& inlineRefs = compiler->GetInlineReferences();
	if (!inlineRefs.empty()) {
		// Check if any aspect transformations were involved so as to update the
		// line mappings from the original source across all aspect result sources.
		const StringList aspectResults = GetAspectResultsForSource(source);
		for (StringList::const_iterator i = aspectResults.begin(); i != aspectResults.end(); ++i) {
			const LineMappings lineMappings = GetSourceLineMappingsForSource(*i);				
			for (DeltaMetaCompiler::InlineReferences::iterator j = inlineRefs.begin(); j != inlineRefs.end(); ++j) {
				DeltaMetaCompiler::LineMappings::const_iterator iter = lineMappings.find(j->back().line);
				DASSERT(iter != lineMappings.end());
				j->push_back(AST::SourceLineOriginInfo(*i, *iter->second.begin()));
			}
		}
	}
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::OnParse (AST::Node* ast, void* closure) {
	AST::NodeToChainOfSourceLineOriginInfo info = GetNodeToChainOfSourceLineOriginInfoForSource(originalSource);
	(AST::ChainOfSourceLineOriginInfoSetter(info))(ast);
}

///////////////////////////////////////////////////////////////

DeltaVirtualMachine* StagedCompilationHandler::OnStageSource (
	const std::string&								source,
	const std::string&								text,
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	sourceRefs,
	void*											closure
) {
	AttachSource(source, text, lineMappings, sourceRefs, true, false);

	DeltaVirtualMachine* vm = (DeltaVirtualMachine*) 0;
	std::string error;
	DeltaMetaCompiler* compiler = (DeltaMetaCompiler*) closure;

	if (BuildClient::IsConnectionBroken())
		error = "Could not build the stage source because the connection to the MetaBuildServer was broken";
	else {
		std::string stageBinary, bytecodePath, dllimportPath;
		BuildSource(source, &stageBinary, &bytecodePath, &dllimportPath);

		if (stageBinary.empty())
			error = "There were build errors in the stage compilation";
		else {
			UpdateInlineReferences(source, compiler);
			SetByteCodeLoadingPath_StdLib(bytecodePath, false);
			SetDllImportPath_StdLib(dllimportPath, false);
			const std::string vmId = source;	//uconstructstr("%s%s", compilerSerial, ucstringarg(source));
			vm = DNEWCLASS(DeltaVirtualMachine, (vmId.c_str()));
			if (!DPTR(vm)->Load(stageBinary.c_str())) {
				unullify(vm);
				error = UERROR_GETREPORT();
				UERROR_CLEAR();
			}
		}
	}
	if (!error.empty()) {
		DeltaCompilerMessenger& messenger = COMPMESSENGER_EX(compiler->GET_COMPONENT_DIRECTORY());
		messenger.SetChainOfSourceLineOriginInfo();
		messenger.Error("Stage execution failed: %s", error.c_str());
	}
	return vm;
}

///////////////////////////////////////////////////////////////

bool StagedCompilationHandler::OnStageResult(
	const std::string&								source,
	const std::string&								text,
	const LineMappings&								lineMappings,
	const AST::NodeToChainOfSourceLineOriginInfo&	sourceRefs,
	bool											final,
	void*											closure
) {
	AttachSource(source, text, lineMappings, sourceRefs, false, final);
	if (final) {
		std::string error;
		if (BuildClient::IsConnectionBroken())
			error = "Could not build the final program because the connection to the MetaBuildServer was broken";
		else {
			std::string binary, unused;
			BuildSource(source, &binary, &unused, &unused);
			if (binary.empty())
				error = "There were build errors in the final program compilation";
		}
		if (!error.empty()) {
			DeltaCompilerMessenger& messenger = COMPMESSENGER_EX(((DeltaMetaCompiler*) closure)->GET_COMPONENT_DIRECTORY());
			messenger.SetChainOfSourceLineOriginInfo();
			messenger.Error(error.c_str());
		}
		return false;
	}
	else
		return true;
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::RegisterSource (const std::string& source, const std::string& stageOutputPath, DeltaMetaCompiler* compiler) {
	BuildClient::Connect(host, port);
	originalSource = source;
	outputPath = stageOutputPath;
	DPTR(compiler)->SetParseCallback(umakecallback(&OnParse, compiler));
	DPTR(compiler)->SetStageCallback(umakecallback(&OnStageSource, compiler));
	DPTR(compiler)->SetStageResultCallback(umakecallback(&OnStageResult, compiler));
}

void StagedCompilationHandler::Initialise (const std::string& host, util_ui32 port) {
	StagedCompilationHandler::host = host;
	StagedCompilationHandler::port = port;
	BuildClient::Initialise();
}

void StagedCompilationHandler::CleanUp (void) {
	BuildClient::CleanUp();
}
