/**
 *	ASTView.dsc
 *
 *	-- Sparrow IDE AST View extension --
 *
 *	Custom visualizer for viewing ast values.
 *  (requires graphviz dot executable)
 *	Also supports a textual AST representation
 *	(not requiring any external tools)
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	November 2011
 */

using std;
using #sparrowlib;
spw = sparrowlib::sparrow();

const classId = "ASTView";

inBreakpoint = false;
editorWindows = [];
id = 0;

//-------------------------------------------------------//
//---- Constant Definitions -----------------------------//

const AST_TAG_NAME					= "Name";
const AST_TAG_FUNCTION				= "Function";

const AST_TAG_NUM_CONST 			= "NumConst";
const AST_TAG_BOOL_CONST 			= "BoolConst";

const AST_TAG_DOTINDEX_IDENT 		= "DotIndexIdent";
const AST_TAG_DOTINDEX_OPSTRING 	= "DotIndexOpString";
const AST_TAG_BRACKETINDEX_OPMETHOD = "BracketIndexOpMethod";

const AST_TAG_TABLE_DOTTED_IDENT 	= "DottedIdent";
const AST_TAG_TABLE_OPERATOR_INDEX 	= "OperatorIndex";

const AST_ATTRIBUTE_NAME 			= "name";
const AST_ATTRIBUTE_RENAME 			= "rename";
const AST_ATTRIBUTE_CLASS 			= "class";
const AST_ATTRIBUTE_LINKAGE 		= "linkage";
const AST_ATTRIBUTE_CONSTVALUE 		= "constvalue";
const AST_ATTRIBUTE_VALUE 			= "value";

nodeAttributes = [
	{ AST_TAG_NAME : list_new(AST_ATTRIBUTE_NAME, AST_ATTRIBUTE_RENAME) },
	{ AST_TAG_FUNCTION : list_new(AST_ATTRIBUTE_CLASS, AST_ATTRIBUTE_LINKAGE) },
	{ AST_TAG_NUM_CONST : list_new(AST_ATTRIBUTE_CONSTVALUE) },
	{ AST_TAG_BOOL_CONST : list_new(AST_ATTRIBUTE_CONSTVALUE) },
	{ 	AST_TAG_DOTINDEX_IDENT,
		AST_TAG_DOTINDEX_OPSTRING,
		AST_TAG_BRACKETINDEX_OPMETHOD,
		AST_TAG_TABLE_OPERATOR_INDEX	: list_new(AST_ATTRIBUTE_VALUE)
	},
	{ AST_TAG_TABLE_DOTTED_IDENT : list_new(AST_ATTRIBUTE_NAME) }
];

//TODO: string lists:
//AST_TAG_STRING_CONST
//AST_TAG_STRINGIFY_DOTTED_IDENTS

//-------------------------------------------------------//
//---- Properties ---------------------------------------//

function SetProperties {}

//-----------------------------------------------------------------------

function RemoveQuotes(str) {
	assert typeof(str) == TYPEOF_STRING;
	return strslice(str, 1, strlen(str) - 2);
}

//-----------------------------------------------------------------------

function EvalExpr(expr) {
	local table = spw.components.DeltaVM.EvalExpr(expr);
	return (table.second ? table.first : nil);
}

function EvalStringExpr(expr) {	
	local val = EvalExpr(expr);
	return (val ? RemoveQuotes(val) : "");
}

//-----------------------------------------------------------------------

function IsValidAST(expr) { return EvalStringExpr(expr + "._type_") == "std::ast"; }

//-----------------------------------------------------------------------

function GenerateAST(expr)
{
	function ToBool(val) { return val == "true"; }
	function ToNum(val) { return strtonum(val); }
	function ToString(val) { return RemoveQuotes(val); }
	
	local ast = [];
	ast.id = id++;
	ast.tag = EvalStringExpr(expr + ".tag");
	assert ast.tag;
	
	if (local attrs = nodeAttributes[ast.tag]) {
		ast.attributes = [];
		foreach(local attr, attrs) {
			local val = EvalExpr(expr + ".attributes." + attr);
			if (ast.tag == AST_TAG_BOOL_CONST or attr == AST_ATTRIBUTE_RENAME)
				ast.attributes[attr] = ToBool(val);
			else if (ast.tag == AST_TAG_NUM_CONST)
				ast.attributes[attr] = ToNum(val);
			else 
				ast.attributes[attr] = ToString(val);
		}
	}

	local table = spw.components.DeltaVM.EvalExpr(expr + ".children.size");
	if (table.second) {
		ast.children = [];
		local totalChildren = strtonum(table.first);
		for (local i = 0; i < totalChildren; ++i)
			ast.children[i] = GenerateAST(expr + ".children[" + i + "]");
	}
	return ast;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

function GetASTExpressionCode(expr) {
	local debugger = spw::getclassproperty("DeltaVM", "debugger");
	assert debugger;
	local oldMax = debugger.properties.value.tostring_maxlength.value;
	debugger.properties.value.tostring_maxlength.value = 0;	//get full message for this evaluation
	spw::addclassproperty("DeltaVM", "debugger", debugger);
	
	local str = EvalExpr(expr);
	
	debugger.properties.value.tostring_maxlength.value = oldMax; //restore message limit
	spw::addclassproperty("DeltaVM", "debugger", debugger);

	if (str) {
		local start = 0, end = strlen(str) - 1;
		while(strchar(str, start) != "\"") ++start;
		while(strchar(str, end) != "\"") --end;
		return strslice(str, start + 1, end - 1);
	}
	else
		return nil;
}

//-----------------------------------------------------------------------

function EditorWindow(window, expr) {
	window = spw.decorate(window);
	local editor = spw.decorate(window.CreateContainedComponent("Editor"));
	const uri = "AST.dsc";
	editor.SetURI(uri);
	editor.SetLanguageSettings(uri);

	local editorWindow = [
		@window : window,
		@editor : editor,
		
		method EvaluateExpression(expr) {
			@window.SetExpression(expr);
			@editor.SetReadOnly(false);
			local text = (IsValidAST(expr) ? GetASTExpressionCode(expr) : "<Not a valid AST expression>");
			@editor.SetText(text);
			@editor.SetReadOnly(true);
			@editor.SetModified(false);
			@editor.ClearIndicators();
		},
		method Refresh { @EvaluateExpression(@window.GetExpression()); },
				
		method onClose {
			editorWindows[@window.serial] = nil;
			local shell = spw.components.Shell;
			if (shell.serial != 0 and @window)
				shell.RemoveComponent(@window);
		}
	];
	editorWindow.EvaluateExpression(expr);
	return editorWindow;
}

//-----------------------------------------------------------------------

function MatchWindow(invoker) {
	if (local editorWindow = editorWindows[invoker.serial]) {
		local window = editorWindow.window;
		assert window;
		if (invoker.class_id == window.class_id and invoker.serial == window.serial)
			return editorWindow;
	}
	return nil;
}

//-----------------------------------------------------------------------

function CloseAllEditorWindows {
	foreach(local editorWindow, editorWindows)
		editorWindow.onClose();	//Use this instead of editorWindow.window.CloseDialog() to instantly
								//close the editorWindow (no signals involved). This is usefull when
								//sparrow closes during debugging and no pending signals are sent.
	assert tablength(editorWindows) == 0;
}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

function GenerateAndShowAST
{	
	if (not inBreakpoint or spw.components.EditorManager.serial == 0)
		return;
	local editor = spw.components.EditorManager.GetFocusedEditor();
	if (not editor or editor.serial == 0)
		return;
	local expr = editor.GetSelectedText();
	if (expr == "")
		return;
	if (IsValidAST(expr)) {
		id = 0;
		local ast = GenerateAST(expr);

		function WriteNodes(nodes, node) {
			local label;
			if (node.tag == AST_TAG_NAME)
				label = node.attributes[AST_ATTRIBUTE_NAME];
			else if (node.tag == AST_TAG_NUM_CONST or node.tag == AST_TAG_BOOL_CONST)
				label = node.attributes[AST_ATTRIBUTE_CONSTVALUE];
			else
				label = node.tag;
			
			nodes.push_back("n" + node.id + "[label = \""+ label +"\"];");
			local totalChildren = tablength(node.children);
			for(local i = 0; i < totalChildren; ++i)
				WriteNodes(nodes, node.children[i]);
		}
		function WriteEdges(edges, node) {
			local totalChildren = tablength(node.children);
			for(local i = 0; i < totalChildren; ++i) {
				edges.push_back("n" + node.id + " -> n" + node.children[i].id);
				WriteEdges(edges, node.children[i]);
			}
		}
		local nodes = list_new();
		local edges = list_new();
		WriteNodes(nodes, ast);
		WriteEdges(edges, ast);
		
		const options = 
			"ordering=out;
			ranksep=.4;
			node [
				shape=box,
				fixedsize=false,
				fontsize=11,
				fontname=\"Courier\",
				width=.25,
				height=.25
			];
			edge [arrowsize=.5]";
		
		local file = fileopen("tmp.dot", "wt");
		file.write("digraph {\n");
		file.write(options);
		file.write("\n");
		foreach(local str, nodes)
			file.write(str, "\n");
		file.write("\n");
		foreach(local str, edges)
			file.write(str, "\n");
		file.write("\n}");
		file.close();
		fileexecute("dot tmp.dot -Tpng -o ast.png");
		fileexecute("ast.png");
	}
	else
		spw.components.Shell.ShowMessage(
			"Error",
			"Expression '" + expr + "' does not evaluate to an AST value."
		);
}

//-----------------------------------------------------------------------

function ShowTextualAST
{
	if (inBreakpoint) {
		local expr = "";
		if (spw.components.EditorManager.serial != 0							and
			(local editor = spw.components.EditorManager.GetFocusedEditor())	and
			editor.serial != 0
		)
			expr = editor.GetSelectedText();

		local window = spw.components.Shell.AddComponent("DeltaQuickWatch", 0);
		local editorWindow = EditorWindow(window, expr);
		editorWindows[editorWindow.window.serial] = editorWindow;
		editorWindow.window.ShowDialog(false);
	}
}

//-----------------------------------------------------------------------

onevent onQuickWatchEvaluateExpression(invoker, expr) {
	if (inBreakpoint and local editorWindow = MatchWindow(invoker))
		editorWindow.EvaluateExpression(expr);
}

//-----------------------------------------------------------------------

onevent onQuickWatchAddWatch(invoker, expr)
{
	if (spw.components.ExpressionWatches and MatchWindow(invoker))
		spw.components.ExpressionWatches.AddWatch(expr);
}

//-----------------------------------------------------------------------

onevent onQuickWatchClosed(invoker) {
	if (local editorWindow = MatchWindow(invoker))
		editorWindow.onClose();
}

//-----------------------------------------------------------------------

onevent onBreakpointHit(invoker, uri, line) {
	foreach(local editorWindow, editorWindows)
		editorWindow.Refresh();
	inBreakpoint = true;
}

//-----------------------------------------------------------------------

onevent onDebugResumed(invoker) { inBreakpoint = false; }

//-----------------------------------------------------------------------

onevent onDebugStopped(classId, uri) { CloseAllEditorWindows(); }

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId);
	spw::setcomponentmetadata(
		classId, "ASTView",
		"Custom visualizer for viewing ast values",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);
	
	local dir = spw::installationdir() + "/resources/";
	spw::registerimage("ast", dir + "ast.png");
	spw::registerimage("ast_text", dir + "ast_text.png");

	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"	},
			{.class_id		: classId			},
			{.function_name	: "ShowVisualAST"	},
			{.flags			: 7					},
			{.image			: "ast"				}
		],
		"/{110}Debug/{150}Visual AST View\tShift+F8--",
		"Show a visual representation of an AST value"
	);

	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"	},
			{.class_id		: classId			},
			{.function_name	: "ShowTextualAST"	},
			{.flags			: 7					},
			{.image			: "ast_text"		}
		],
		"/{110}Debug/{160}Source Code AST View\tShift+F10--",
		"Show the source code representation of an AST value"
	);

	spw::class_decl_required_member_handler(classId, "QuickWatchReevaluate");
	spw::class_decl_required_member_handler(classId, "QuickWatchAddWatch");
	spw::class_decl_required_member_handler(classId, "QuickWatchExpressionChanged");
	spw::class_decl_required_member_handler(classId, "QuickWatchClosed");
	spw::class_decl_required_member_handler(classId, "BreakpointHit");
	spw::class_decl_required_member_handler(classId, "DebugResumed");
	spw::class_decl_required_member_handler(classId, "DebugStopped");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_command(classId, "ShowVisualAST", GenerateAndShowAST);
	spw::inst_impl_required_member_command(classId, "ShowTextualAST", ShowTextualAST);	
	spw::inst_impl_required_member_handler(classId, "QuickWatchReevaluate", onQuickWatchEvaluateExpression);
	spw::inst_impl_required_member_handler(classId, "QuickWatchAddWatch", onQuickWatchAddWatch);
	spw::inst_impl_required_member_handler(classId, "QuickWatchExpressionChanged", onQuickWatchEvaluateExpression);
	spw::inst_impl_required_member_handler(classId, "QuickWatchClosed", onQuickWatchClosed);	
	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onDebugStopped);
}

//-----------------------------------------------------------------------

onevent Destructor { CloseAllEditorWindows(); }

//-----------------------------------------------------------------------
