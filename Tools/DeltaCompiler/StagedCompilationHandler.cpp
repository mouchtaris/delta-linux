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

void StagedCompilationHandler::AddSource(
	const std::string&		source,
	const std::string&		text,
	unsigned				stage,
	const LineMappings&		lineMappings,
	const SourceReferences&	sourceRefs,
	bool					isStage,
	bool					isFinal
) {
	DeltaMetaCompiler::DumpSource(outputPath + source, text);
	BuildClient::DoAddSource(originalSource, source, lineMappings, sourceRefs, isStage ? "stage" : "result", stage, isFinal);
	BuildClient::WaitAnyMessage();
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::BuildSource(const std::string& source, std::string* binary, std::string* bytecodePath, std::string* dllimportPath) {
	BuildClient::DoBuildSource(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == Build_StageBinary)	//ignore unrecognized messages
		BuildClient::GetStageBinaryData(binary, bytecodePath, dllimportPath);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
}

///////////////////////////////////////////////////////////////

const StagedCompilationHandler::StringList StagedCompilationHandler::GetTransformations(const std::string& source) {
	StringList transformations;
	BuildClient::DoGetTransformations(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == Build_Transformations)
		BuildClient::GetTransformations(transformations);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
	return transformations;
}

///////////////////////////////////////////////////////////////

const StagedCompilationHandler::LineMappings StagedCompilationHandler::GetLineMappings(const std::string& source) {
	LineMappings lineMappings;
	BuildClient::DoGetLineMappings(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == Build_LineMappings)
		BuildClient::GetLineMappings(lineMappings);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
	return lineMappings;
}

///////////////////////////////////////////////////////////////

const StagedCompilationHandler::SourceReferences StagedCompilationHandler::GetSourceReferences(const std::string& source) {
	SourceReferences sourceRefs;
	BuildClient::DoGetSourceReferences(source);
	BuildClient::WaitAnyMessage();
	if (BuildClient::GetResponseType() == Build_SourceReferences)
		BuildClient::GetSourceReferences(sourceRefs);
	if (!BuildClient::IsConnectionBroken())
		BuildClient::ResponseProcessed();
	return sourceRefs;
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::UpdateInlineReferences(const std::string& source, DeltaMetaCompiler* compiler) {
	DeltaMetaCompiler::InlineReferences& inlineRefs = compiler->GetInlineReferences();
	if (!inlineRefs.empty()) {
		const StringList transformations = GetTransformations(source);
		for (StringList::const_iterator i = transformations.begin(); i != transformations.end(); ++i) {
			const LineMappings lineMappings = GetLineMappings(*i);				
			for (DeltaMetaCompiler::InlineReferences::iterator j = inlineRefs.begin(); j != inlineRefs.end(); ++j) {
				DeltaMetaCompiler::LineMappings::const_iterator iter = lineMappings.find(j->back().second);
				DASSERT(iter != lineMappings.end());
				j->push_back(AST::Node::SourceInfo(*i, *iter->second.begin()));
			}
		}
	}
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::OnParse (AST::Node* ast, void* closure) {
	SourceReferences sourceRefs = GetSourceReferences(originalSource);
	(AST::SourceReferenceSetter(sourceRefs))(ast);
}

///////////////////////////////////////////////////////////////

DeltaVirtualMachine* StagedCompilationHandler::OnStageSource (
	const std::string&		source,
	const std::string&		text,
	util_ui32				stage,
	const LineMappings&		lineMappings,
	const SourceReferences&	sourceRefs,
	void*					closure
) {
	AddSource(source, text, stage, lineMappings, sourceRefs, true, false);

	DeltaVirtualMachine* vm = (DeltaVirtualMachine*) 0;
	std::string error;
	DeltaMetaCompiler* compiler = (DeltaMetaCompiler*) closure;

	if (BuildClient::IsConnectionBroken())
		error = "Could not build the stage source because the connection to the BuildServer was broken";
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
		DeltaCompilerMessenger& messenger = COMPMESSENGER_EX(compiler->COMPONENT_DIRECTORY());
		messenger.SetSourceReferences();
		messenger.Error("Stage execution failed: %s", error.c_str());
	}
	return vm;
}

///////////////////////////////////////////////////////////////

bool StagedCompilationHandler::OnStageResult(
	const std::string&		source,
	const std::string&		text,
	util_ui32				stage,
	const LineMappings&		lineMappings,
	const SourceReferences&	sourceRefs,
	bool					final,
	void*					closure
) {
	AddSource(source, text, stage, lineMappings, sourceRefs, false, final);
	if (final) {
		std::string error;
		if (BuildClient::IsConnectionBroken())
			error = "Could not build the final program because the connection to the BuildServer was broken";
		else {
			std::string binary, unused;
			BuildSource(source, &binary, &unused, &unused);
			if (binary.empty())
				error = "There were build errors in the final program compilation";
		}
		if (!error.empty()) {
			DeltaCompilerMessenger& messenger = COMPMESSENGER_EX(((DeltaMetaCompiler*) closure)->COMPONENT_DIRECTORY());
			messenger.SetSourceReferences();
			messenger.Error(error.c_str());
		}
		return false;
	}
	else
		return true;
}

///////////////////////////////////////////////////////////////

void StagedCompilationHandler::HandleSource (const std::string& source, const std::string& stageOutputPath, DeltaMetaCompiler* compiler) {
	BuildClient::Connect(host, port);
	originalSource = source;
	outputPath = stageOutputPath;
	DPTR(compiler)->SetParseCallback(umakecallback(&OnParse, compiler));
	DPTR(compiler)->SetStageCallback(umakecallback(&OnStageSource, compiler));
	DPTR(compiler)->SetStageResultCallback(umakecallback(&OnStageResult, compiler));
}

void StagedCompilationHandler::Initialise(const std::string& host, util_ui32 port) {
	StagedCompilationHandler::host = host;
	StagedCompilationHandler::port = port;
	BuildClient::Initialise();
}

void StagedCompilationHandler::CleanUp() {
	BuildClient::CleanUp();
}
