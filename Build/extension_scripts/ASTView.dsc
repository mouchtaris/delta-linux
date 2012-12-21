/**
 *	ASTView.dsc
 *
 *	-- Sparrow IDE AST View extension --
 *
 *	Custom visualizer for viewing ast values.
 *  (Requires graphviz dot executable).
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	November 2011
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "ASTView";

inBreakpoint = false;
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

onevent onBreakpointHit(invoker, uri, line) { inBreakpoint = true; }

onevent onDebugResumed(invoker) { inBreakpoint = false; }

//-----------------------------------------------------------------------

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

	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"	},
			{.class_id		: classId			},
			{.function_name	: "ShowASTCmd"		},
			{.flags			: 7					}
		],
		"/{110}Debug/{150}AST View\tShift+F8--",
		"Show a visual representation of an AST value"
	);
	
	spw::class_decl_required_member_handler(classId, "BreakpointHit");
	spw::class_decl_required_member_handler(classId, "DebugResumed");	
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_command(classId, "ShowASTCmd", GenerateAndShowAST);	
	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
}

//-----------------------------------------------------------------------

onevent Destructor {}

//-----------------------------------------------------------------------
