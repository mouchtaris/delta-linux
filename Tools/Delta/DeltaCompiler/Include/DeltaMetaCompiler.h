// DeltaMetaCompiler.h
// Meta compiler implementation.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//

#ifndef	DELTAMETACOMPILER_H
#define	DELTAMETACOMPILER_H

#include "CompilerAPI.h"
#include "ucallbacks.h"
#include "ASTUnparseVisitor.h"
#include "ASTChainOfSourceLineOriginInfo.h"

///////////////////////////////////////////////////////////////

class ASTInjector;
class DeltaVirtualMachine;
class DeltaValue;

///////////////////////////////////////////////////////////////

class DCOMPLIB_CLASS DeltaMetaCompiler : public DeltaCompiler {
	public:
	typedef std::list<AST::ChainOfSourceLineOriginInfo>					InlineReferences;
	typedef AST::UnparseVisitor::LineMappings							LineMappings;
	typedef util_ui32 NodeSerialNo;
	typedef std::map<NodeSerialNo, AST::ChainOfSourceLineOriginInfo>	NodeToChainOfSourceLineOriginInfo;

	private:
	typedef std::map<DeltaVirtualMachine*, DeltaMetaCompiler*> VMCompilerMap;
	static VMCompilerMap metaCompilerMappings;

	AST::Node*				stage;
	InlineReferences		inlineReferences;
	ASTInjector*			astInjector;
	AST::ValidationVisitor* validator;
	std::string				mainSource;

	typedef ucallbackwithclosure<DeltaVirtualMachine* (*)(
		const std::string&,
		const std::string&,
		const LineMappings&,
		const NodeToChainOfSourceLineOriginInfo&,
		void*
	)> StageCallback;
	StageCallback			stageCallback;

	typedef ucallbackwithclosure<bool (*)(
		const std::string&,
		const std::string&,
		const LineMappings&,
		const NodeToChainOfSourceLineOriginInfo&,
		bool,
		void*
	)> StageResultCallback;
	StageResultCallback		stageResultCallback;

	virtual bool			IntermediateCode			(void);
	bool					ToAST						(DeltaValue* value, AST::Node** ast);

	AST::Node*				AssembleStage				(AST::Node* ast, unsigned depth);
	bool					ExecuteStageVM				(DeltaVirtualMachine* vm);
	unsigned				GenerateStageResult			(AST::Node* ast, const std::string& originalSource, const std::string& mainSource, util_ui32 stage);

public:
	InlineReferences&			GetInlineReferences(void) { return inlineReferences; }

	bool						StagedCompilation (AST::Node* ast);
	
	void						Inline (DeltaValue* value);
	TreeNode*					GetInlineContext (const std::string& tag = "");

	void						SetStageCallback (StageCallback callback) { stageCallback = callback; }
	StageCallback				GetStageCallback (void) const { return stageCallback; }

	void						SetStageResultCallback (StageResultCallback callback) { stageResultCallback = callback; }
	StageResultCallback			GetStageResultCallback (void) const { return stageResultCallback; }

	static DeltaMetaCompiler*	GetCompilerForVM (DeltaVirtualMachine* vm);
	static void					DumpSource (const std::string& source, const std::string& text);

	DeltaMetaCompiler (void);
	~DeltaMetaCompiler ();
};

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
