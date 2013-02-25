// AspectCompiler.h
// Aspects for Delta scripts based on AST Transformations
// Y.Lilis, July 2012.
//

#include "AspectCompiler.h"
#include "DeltaMetaCompiler.h"
#include "ASTLib.h"
#include "ASTUtilVisitors.h"
#include "ASTValidationVisitor.h"
#include "ASTAlphaRenamer.h"

///////////////////////////////////////////////////////////////////////////

void AspectCompiler::Error(const std::string& msg) {
	if (onError)
		onError(uconstructstr((msg + ".\n").c_str(), UERROR_GETREPORT().c_str()).c_str());
	UERROR_CLEAR();
}

///////////////////////////////////////////////////////////////////////////

static const std::string BaseName (const std::string& name)
	{ return name.substr(0, name.find_last_of('.')); }

bool AspectCompiler::ParseFile(const std::string& path, const std::string& symbolic) {
	DeltaMetaCompiler* compiler = DNEW(DeltaMetaCompiler);
	DPTR(compiler)->SetErrorCallback(onError);
	if (AST::Node* ast = DPTR(compiler)->Parse(path.c_str())) {
		(AST::SourceSetter(symbolic))(ast);
		if (onParse)
			onParse(ast);
		tree = ast->TreeNode::Clone(TreeNode::DefaultCreator());
		name = BaseName(symbolic);
	}
	DDELETE(compiler);
	return tree;
}

///////////////////////////////////////////////////////////////////////////

static void UpdateSourceLocationFromUnparsed (AST::Node* original, AST::Node* unparsed, const std::string& currentSource) {
	DASSERT(original && unparsed);
	std::list<AST::Node*> originalNodes = AST::Linearizer()(original);
	std::list<AST::Node*> unparsedNodes = AST::Linearizer()(unparsed);
	DASSERT(originalNodes.size() == unparsedNodes.size());
	std::list<AST::Node*>::iterator i, j;
	for (i = originalNodes.begin(), j = unparsedNodes.begin(); i != originalNodes.end(); ++i, ++j) {
		DASSERT((*i)->GetTag() == (*j)->GetTag());
		AST::Node* node = *i;
		std::string source;
		if (!(source = DPTR(node)->GetSource()).empty() && source != currentSource)
			DPTR(node)->AddSourceReference(AST::Node::SourceInfo(source, DPTR(node)->GetStartLine()));
		DPTR(node)->SetLocation(DPTR(*j)->GetLocation());
		DPTR(node)->SetSource(currentSource);
	}
}

static const std::string GenerateSource (AST::Node* ast, const std::string& source, AspectCompiler::LineMappings* lineMappings) {
	AST::UnparseVisitor unparser;
	const std::string text = unparser(ast);
	if (lineMappings)
		*lineMappings = unparser.GetLineMappings();

	DeltaMetaCompiler* compiler = DNEW(DeltaMetaCompiler);
	AST::Node* unparsedAst = DPTR(compiler)->ParseText(text.c_str());
	DASSERT(unparsedAst);
	UpdateSourceLocationFromUnparsed(ast, unparsedAst, source);
	DDELETE(compiler);

	return text;
}

bool AspectCompiler::ApplyTransformations(const StringList& aspects) {
	DASSERT(tree);

	AutoCollector* autoCollector = DNEW(AutoCollector);
	AST::Factory* factory = DNEWCLASS(AST::Factory, (DPTR(autoCollector)));

	unsigned count = 0;
	for(StringList::const_iterator i = aspects.begin(); i != aspects.end(); ++i) {
		const std::string binary = *i;	//TODO: change this if aspects are sources instead of binaries
		
		const std::string vmId = uconstructstr("%s_aspect_%d", name.c_str(), ++count);
		DeltaVirtualMachine* vm = DNEWCLASS(DeltaVirtualMachine, (vmId.c_str()));
		if (!DPTR(vm)->Load(binary.c_str())) {
			udelete(vm);
			Error("Error loading aspect code: %s");
			break;
		}

		util_ui32 serialNo = DPTR(vm)->GetSerialNo();
		DPTR(vm)->Run();

		if (DeltaRunTimeErrorsExist()) {
			if (ValidatableHandler::Validate(vm, serialNo))
				DDELETE(vm);
			Error("Error running global aspect code: %s");
			break;
		}

		if (!DPTR(vm)->GlobalFuncExists(ASPECT_TRANSFORMATION_FUNCNAME)) {
			udelete(vm);
			if (onError)
				onError(uconstructstr("Error: No global function '%s' found.\n", ASPECT_TRANSFORMATION_FUNCNAME).c_str());
			break;
		}

		DeltaValue treeValue;
		DeltaAST_Make(&treeValue, tree);
		DPTR(vm)->PushActualArg(treeValue);
		DPTR(vm)->ExtCallGlobalFunc(ASPECT_TRANSFORMATION_FUNCNAME);
		
		if (DeltaRunTimeErrorsExist()) {
			if (ValidatableHandler::Validate(vm, serialNo))
				DDELETE(vm);
			Error("Error executing aspect transformation function: %s");
			unullify(tree);
			break;
		}

		tree = DeltaAST_Get(DPTR(vm)->GetReturnValue());		//previous tree is automatically garbage collected if needed
		if (ValidatableHandler::Validate(vm, serialNo))
			DDELETE(vm);
		if (!tree) {
			if (onError)
				onError("Invalid transformation: result is not a valid AST.\n");
			break;
		}

		AST::AlphaRenamer()(tree);
		AST::ValidationVisitor validator;
		if (!validator(tree)) {
			if (onError)
				onError(uconstructstr("Invalid transformation: %s.\n", validator.GetValidationError().c_str()).c_str());
			unullify(tree);
			break;
		}

		AST::Node* ast = (AST::Node*) tree->Clone(ubind1st(umemberfunctionpointer(&AST::Factory::NewNode), DPTR(factory)));
		RemoveRefCounters(ast);

		LineMappings lineMappings;

		const std::string source = vmId + ".dsc";
		const std::string text = GenerateSource(ast, source, &lineMappings);
		
		tree = ast->TreeNode::Clone(TreeNode::DefaultCreator());
		
		if (onTransformation) {
			AST::SerialProducer()(ast);
			const SourceReferences sourceRefs = AST::SourceReferenceGetter()(ast);
			onTransformation(source, text, lineMappings, sourceRefs, count, count == aspects.size());
		}

		DPTR(autoCollector)->CleanUp();
	}

	udelete(factory);
	udelete(autoCollector);

	return count == aspects.size();
}

///////////////////////////////////////////////////////////////////////////

static void DefaultTransformationCallback(
	const std::string&						source,
	const std::string&						text,
	const AspectCompiler::LineMappings&		lineMappings,
	const AspectCompiler::SourceReferences&	sourceRefs,
	util_ui32								index,
	bool									final,
	void*									closure
) {
	DeltaMetaCompiler::DumpSource(source, text);
}

AspectCompiler::AspectCompiler(void) : tree(0) {
	onTransformation = umakecallback(&DefaultTransformationCallback, this);
}
AspectCompiler::~AspectCompiler() { if (tree) TreeNode::Delete(tree); }