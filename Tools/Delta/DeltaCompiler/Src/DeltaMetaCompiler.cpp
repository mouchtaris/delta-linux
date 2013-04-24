// DeltaMetaCompiler.cpp
// Meta compiler implementation.
// ScriptFighter Project.
// Y. Lilis, July 2011.
//

#include "DeltaMetaCompiler.h"
#include "ASTMetaTags.h"
#include "ASTStageDepthCalculator.h"
#include "ASTStageAssembler.h"
#include "ASTSanitiseVisitor.h"
#include "ASTValidationVisitor.h"
#include "ASTTranslationVisitor.h"
#include "ASTCreationActions.h"
#include "ASTAlphaRenamer.h"
#include "ASTUtilVisitors.h"
#include "ASTUnparseVisitor.h"
#include "ASTInjector.h"

#include "CompileOptions.h"
#include "DeltaCompErrorMsg.h"

#include "ASTLib.h"
#include "MetaCompilerLib.h"
#include "DeltaVirtualMachine.h"

DeltaMetaCompiler::VMCompilerMap DeltaMetaCompiler::metaCompilerMappings;

/////////////////////////////////////////////////////////

struct InlineLocator : public TreeVisitor {
	AST::Node* node;
	static void Handle_Inline (AST_VISITOR_ARGS) {
		if (!node->GetTotalChildren()) {
			((InlineLocator*) closure)->node = (AST::Node*) node;
			((InlineLocator*) closure)->Stop();
		}
	}
	AST::Node* operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this); return node; }
	InlineLocator (void) : node((AST::Node*) 0) { SetHandler(AST_TAG_INLINE, &Handle_Inline, this); }
};

/////////////////////////////////////////////////////////

bool DeltaMetaCompiler::IntermediateCode (void) {
	if (!COMPMESSENGER.ErrorsExist() && StagedCompilation(GetSyntaxTree())) {
		PARSEPARMS.SetLine(1);
		return DeltaCompiler::IntermediateCode();
	}
	else
		return false;
}

/////////////////////////////////////////////////////////

bool DeltaMetaCompiler::ToAST(DeltaValue* value, AST::Node** node) {
	AST::Node* n = (AST::Node*) 0;
	switch(value->Type()) {
		case DeltaValue_Nil:		*node = (AST::Node*) 0;	return true;
		case DeltaValue_Number:		n = ASTCREATOR.MakeNode_ConstValue(value->ToNumber());			break;
		case DeltaValue_Bool:		n = ASTCREATOR.MakeNode_ConstValue(value->ToBool());				break;
		case DeltaValue_String: 	n = ASTCREATOR.MakeNode_StringConst(value->ToString().c_str());	break;
		case DeltaValue_Table:		return false;
		
		case DeltaValue_ExternId:	{
			if (TreeNode* ast = DeltaAST_Get(*value)) {
				*node = (AST::Node*) ast->Clone(ubind1st(umemberfunctionpointer(&AST::Factory::NewNode), &ASTFACTORY));
				return true;
			}
			else
				return false;
		}

		default:					return false;
	}
	DASSERT(n);
	*node = ASTCREATOR.MakeNode_PrimaryExpression(DPTR(n)); 
	return true;
}

/////////////////////////////////////////////////////////

AST::Node* DeltaMetaCompiler::AssembleStage (AST::Node* ast, unsigned depth) {
	AST::StageAssembler assembler(GET_COMPONENT_DIRECTORY());
	AST::Node* stage = assembler(ast, depth);
	if (COMPMESSENGER.ErrorsExist())
		return (AST::Node*) 0;
	else
	if (!(*validator)(stage)) {
		COMPMESSENGER.Error("AST for stage %d is invalid: %s", depth, DPTR(validator)->GetValidationError().c_str());
		return (AST::Node*) 0;
	}
	else {
		PARSEPARMS.SetLine(1);
		AST::SanitiseVisitor()(ast);
		inlineReferences = assembler.GetInlineReferences();
		return stage;
	}
}

/////////////////////////////////////////////////////////

static void UpdateSourceLocationFromUnparsed (AST::Node* original, AST::Node* unparsed, const std::string& currentSource, bool addSourceReference = true) {
	DASSERT(original && unparsed);
	std::list<AST::Node*> originalNodes = AST::Linearizer()(original);
	std::list<AST::Node*> unparsedNodes = AST::Linearizer()(unparsed);
	DASSERT(originalNodes.size() == unparsedNodes.size());
	std::list<AST::Node*>::iterator i, j;
	for (i = originalNodes.begin(), j = unparsedNodes.begin(); i != originalNodes.end(); ++i, ++j) {
		DASSERT((*i)->GetTag() == (*j)->GetTag());
		AST::Node* node = *i;
		std::string source;
		if (addSourceReference && !(source = DPTR(node)->GetSource()).empty() && source != currentSource)
			DPTR(node)->AddSourceReference(AST::Node::SourceInfo(source, DPTR(node)->GetStartLine()));
		DPTR(node)->SetLocation(DPTR(*j)->GetLocation());
		DPTR(node)->SetSource(DPTR(*j)->GetSource());
		if (unsigned line = DPTR(*j)->GetLine())
			DPTR(node)->SetLine(line);
	}
}

/////////////////////////////////////////////////////////

static const std::string GenerateSource (AST::Node* ast, const std::string& source, DeltaMetaCompiler::LineMappings* lineMappings, bool addSourceReference = true) {
	AST::UnparseVisitor unparser;
	const std::string text = unparser(ast);
	if (lineMappings)
		*lineMappings = unparser.GetLineMappings();

	DeltaMetaCompiler* compiler = DNEW(DeltaMetaCompiler);
	AST::Node* unparsedAst = DPTR(compiler)->ParseText(text.c_str());
	
	// Ideally we should have assert(unparsedAst);
	// However, upon AST problems this causes hard to debug errors since we typically run in release mode.
	// So use the if, allowing the stage source to be generated and look there to resolve the errors.
	if (unparsedAst) {
		(AST::SourceSetter(source))(unparsedAst);
		UpdateSourceLocationFromUnparsed(ast, unparsedAst, source, addSourceReference);
	}
	DDELETE(compiler);

	return text;
}

/////////////////////////////////////////////////////////

static void PatchInlineSourceLocations (
	DeltaMetaCompiler::InlineReferences&	inlineReferences,
	const std::string&						file,
	const DeltaMetaCompiler::LineMappings&	lineMappings
) {
	for (DeltaMetaCompiler::InlineReferences::iterator i = inlineReferences.begin(); i != inlineReferences.end(); ++i) {
		i->back().first = file;
		DeltaMetaCompiler::LineMappings::const_iterator iter = lineMappings.find(i->back().second);
		DASSERT(iter != lineMappings.end());
		i->back().second = *iter->second.begin();
	}
}

/////////////////////////////////////////////////////////

bool DeltaMetaCompiler::ExecuteStageVM (DeltaVirtualMachine* vm) {
	util_ui32 serialNo = DPTR(vm)->GetSerialNo();
			
	metaCompilerMappings[vm] = this;
	DPTR(vm)->Run();
	metaCompilerMappings.erase(vm);

	if (ValidatableHandler::Validate(vm, serialNo))
		DDELETE(vm);

	if (DeltaRunTimeErrorsExist()) {
		COMPMESSENGER.Error("Stage execution error: %s", UERROR_GETREPORT().c_str());
		return false;
	}

	return !COMPMESSENGER.ErrorsExist();
}

/////////////////////////////////////////////////////////

static const std::string BaseName (const std::string& name) {
	const std::string noSuffix = name.substr(0, name.find_last_of('.'));
	std::string::size_type pos = noSuffix.find_last_of('/');
	return pos == std::string::npos ? noSuffix : noSuffix.substr(pos + 1);
}

bool DeltaMetaCompiler::StagedCompilation (AST::Node* ast) {
	const std::string originalSource = COMPOPTIONS.GetSourceFile();
	(AST::SourceSetter(originalSource))(ast);
	DPTR(ast)->SetAttribute(AST_ATTRIBUTE_REF_COUNTER, (util_ui32) 1);	//internal ast

	util_ui32 currentStage = 0;
	unsigned depth = AST::StageDepthCalculator()(ast);
	while (depth) {
		++currentStage;

		if (!(stage = AssembleStage(ast, depth)))
			return false;

		std::string basename;
		if (COMPOPTIONS.IsDynamicCode())
			basename = uconstructstr("stage_%d", currentStage);
		else
			basename = uconstructstr("%s_stage_%d", BaseName(originalSource).c_str(), currentStage);
		const std::string stageSource = basename + ".dsc";

		DeltaMetaCompiler::LineMappings lineMappings;
		const std::string stageText = GenerateSource(stage, stageSource, &lineMappings);
		PatchInlineSourceLocations(inlineReferences, stageSource, lineMappings);
		AST::SerialProducer()(stage);
		SourceReferences sourceRefs = AST::SourceReferenceGetter()(stage);

		DASSERT(stageCallback);
		DeltaVirtualMachine* vm = stageCallback(stageSource, stageText, currentStage, lineMappings, sourceRefs);
		TreeNode::Delete((TreeNode*&) stage);
		if (!vm)
			return false;

		mainSource = basename + "_result.dsc";	// set the next main source for the inlines
		if (!ExecuteStageVM(vm))
			return false;

		if (!(*validator)(ast)) {
			COMPMESSENGER.Error("After executing stage %d, resulting AST is invalid: %s", depth, DPTR(validator)->GetValidationError().c_str());
			return false;
		}
		PARSEPARMS.SetLine(1);
		depth = AST::StageDepthCalculator()(ast);
		lineMappings.clear();
		sourceRefs.clear();
		const std::string mainText = GenerateSource(ast, mainSource, &lineMappings, depth > 0);
		AST::SerialProducer()(ast);	// node get new serials after stage execution
		sourceRefs = AST::SourceReferenceGetter()(ast);

		DASSERT(stageResultCallback);
		if (!stageResultCallback(mainSource, mainText, currentStage, lineMappings, sourceRefs, !depth))
			return false;
		//COMPMESSENGER.SetCurrentFile(mainSource);
	}
	return true;
}

/////////////////////////////////////////////////////////

void DeltaMetaCompiler::Inline (DeltaValue* value) {
	AST::Node* node;
	if (!ToAST(value, &node)) {
		COMPMESSENGER.Error("Invalid inline value: no AST or AST convertible value given");
		return;
	}

	AST::Node* ast = GetSyntaxTree();
	AST::Node* target = InlineLocator()(ast);

	std::string error;
	
	// Upon simple meta-compilation the number of inlines should match the number of std::inline calls.
	// However, through std::context it is possible to remove inline nodes from the original AST.
	// Additionally, through AOP it is possible to introduce additional std::inline calls.
	// Overall, it is possible that a given std::inline call does not correspond to a valid inline node, so check here.
	if (target) {
		DASSERT(!inlineReferences.empty());
		AST::Node::SourceInfoReferences refs = inlineReferences.front();
		inlineReferences.pop_front();

		AST::ValidationVisitor* validator = NewValidator(true);
		DPTR(validator)->SetAllowRenames();

		if (node && !(*validator)(node))
			error = uconstructstr("Invalid inline value: invalid AST given: %s", DPTR(validator)->GetValidationError().c_str());
		else {
			if (node)
				AST::LocationSetter(target->GetLocation(), target->GetLine(), mainSource, refs)(DPTR(node));
			(*astInjector)(target, node);

			AST::SanitiseVisitor()(ast);
			if ((*validator)(ast))
				AST::AlphaRenamer()(ast);
			else
				error = uconstructstr("Invalid inline result: %s", DPTR(validator)->GetValidationError().c_str());
		}
		DDELETE(validator);
	}
	else
		error = "Invalid inline: program AST contains no inline tags (maybe removed via std::context access?)";

	if (!error.empty())
		COMPMESSENGER.Error(error.c_str());
}

/////////////////////////////////////////////////////////

TreeNode* DeltaMetaCompiler::GetInlineContext (const std::string& tag) {
	TreeNode* target = InlineLocator()(GetSyntaxTree());
	if (target)
		target = DPTR(target)->GetParent();
	if (!tag.empty())
		while (target && DPTR(target)->GetTag() != tag)
			target = DPTR(target)->GetParent();
	return target;
}

/////////////////////////////////////////////////////////

DeltaMetaCompiler* DeltaMetaCompiler::GetCompilerForVM (DeltaVirtualMachine* vm) {
	VMCompilerMap::const_iterator i = metaCompilerMappings.find(vm);
	return i == metaCompilerMappings.end() ? (DeltaMetaCompiler*) 0 : i->second;
}

void DeltaMetaCompiler::DumpSource(const std::string& source, const std::string& text) {
	if (FILE* fp = fopen(source.c_str(), "w")) {
		fputs(text.c_str(), fp);
		fflush(fp);
		fclose(fp);
	}
}

/////////////////////////////////////////////////////////

static TreeNode* CreateASTNode(const std::string& tag, void* closure)
	{ return ASTFACTORY_EX(((DeltaMetaCompiler*)closure)->GET_COMPONENT_DIRECTORY()).NewNode(tag); }

static bool DefaultStageResultCallback(
	const std::string&							source,
	const std::string&							text,
	util_ui32									stage,
	const DeltaMetaCompiler::LineMappings&		lineMappings,
	const DeltaMetaCompiler::SourceReferences&	sourceRefs, 
	bool										final,
	void*										closure
) {
	DeltaMetaCompiler* compiler = (DeltaMetaCompiler*) closure;
	if (!COMPOPTIONS_EX(DPTR(compiler)->GET_COMPONENT_DIRECTORY()).IsDynamicCode())
		DeltaMetaCompiler::DumpSource(source, text);
	else if (final)	// For the final version of a dynamic source replace the original text with the final
		COMPOPTIONS_EX(DPTR(compiler)->GET_COMPONENT_DIRECTORY()).InitialiseForText(text.c_str());
	return true;
}

static DeltaVirtualMachine* DefaultStageCallback(
	const std::string&							stageSource,
	const std::string&							stageText,
	util_ui32									stage,
	const DeltaMetaCompiler::LineMappings&		lineMappings,
	const DeltaMetaCompiler::SourceReferences&	sourceRefs, 
	void*										closure
) {
	DeltaMetaCompiler::DumpSource(stageSource, stageText);
	ucomponentdirectory* directory = DPTR((DeltaMetaCompiler*) closure)->GET_COMPONENT_DIRECTORY();

	DeltaCompiler* compiler = DNEW(DeltaCompiler);	//stage source has no meta tags, so no need for a metacompiler
	DPTR(compiler)->SetErrorCallback(DPTR((DeltaMetaCompiler*) closure)->GetErrorCallback());
	if (DeltaCompiler::FuncList* externFuncs = DPTR((DeltaMetaCompiler*) closure)->GetExternFuncs())
		for (DeltaCompiler::FuncList::const_iterator i = externFuncs->begin(); i != externFuncs->end(); ++i)
			DPTR(compiler)->AddExternFuncs(*i);
	if (COMPOPTIONS_EX(directory).IsDynamicCode())
		DPTR(compiler)->CompileText(stageText.c_str());
	else
		DPTR(compiler)->Compile(stageSource.c_str());

	DeltaVirtualMachine* vm = (DeltaVirtualMachine*) 0;
	std::string error;
	if (DPTR(compiler)->ErrorsExist())
		error = "There were build errors in the stage compilation";
	else {
		const std::string vmId = stageSource;	//TODO: uconstructstr("%s%s", compilerSerial, ucstringarg(stageSource));
		if (COMPOPTIONS_EX(directory).IsDynamicCode()) {
			ubinaryio::OutputBuffer* ob = DNEW(ubinaryio::OutputBuffer);
			DPTR(compiler)->DumpBinaryCode(utempobj(PortableBufferWriter(*DPTR(ob))));

			ubinaryio::InputBuffer* ib = DNEWCLASS(ubinaryio::InputBuffer, (*DPTR(ob)));
			vm = DNEWCLASS(DeltaVirtualMachine, (vmId.c_str()));

			bool loaded = DPTR(vm)->Load(utempobj(PortableBufferReader(DPTR(ib))));
			DASSERT(loaded);

			DDELETE(ob);
			DDELETE(ib);
		}
		else {
			const std::string stageBinary = BaseName(stageSource) + ".dbc";
			DPTR(compiler)->DumpBinaryCode(stageBinary.c_str());
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
		messenger.SetSourceReferences();
		messenger.Error("Stage execution failed: %s", error.c_str());
	}

	DDELETE(compiler);
	return vm;
}

DeltaMetaCompiler::DeltaMetaCompiler (void) {
	astInjector = DNEWCLASS(ASTInjector, (umakecallback(&CreateASTNode, this)));
	validator = NewValidator();
	stageCallback = umakecallback(&DefaultStageCallback, this);
	stageResultCallback = umakecallback(&DefaultStageResultCallback, this);
}

DeltaMetaCompiler::~DeltaMetaCompiler () {
	DDELETE(astInjector);
	DDELETE(validator);
}

/////////////////////////////////////////////////////////
