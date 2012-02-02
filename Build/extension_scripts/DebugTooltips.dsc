/**
 *	DebugTooltips.dsc
 *
 *	-- Sparrow Debug Watch Tooltips --
 *
 *	Watches of variables during embedded as
 *	tooltips in the source editor.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	September 2007
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "DebugTooltips";

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

function onMouseOnSymbol(editor, startPos, endPos, symbolType, desc)
{
	editor = spw.decorate(editor);
	local selection = editor.GetSelectedText();
	local pos = editor.GetCurrentPos();
	if (strlen(selection) > 0 and pos >= startPos and pos <= endPos)	//mouse on selection
	{
		local table = spw.components.DeltaVM.EvalExpr(selection);
		if (table.second)
		{
			editor.ShowTooltip(startPos, table.first);
			return;
		}
	}
	if(symbolType >= 16 and symbolType <= 19 or symbolType == 21)
	{
		local variable = editor.GetTextRange(startPos, endPos);
		local table = spw.components.DeltaVM.EvalExpr(variable);
		//table: [{.first : value}, {.second : status}]
		if (table.second)
			editor.ShowTooltip(startPos, table.first);
	}
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId);
	spw::setcomponentmetadata(
		classId, "Debugging Watch Tooltips",
		"Tooltips for watching variables during debugging",
		"Themistoklis Bourdenas <themis@ics.forth.gr>",
		"alpha"
	);

	spw::class_decl_required_member_handler(classId, "EditMouseOnSymbol");
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_handler(classId, "EditMouseOnSymbol", onMouseOnSymbol);
}

//-----------------------------------------------------------------------

onevent Destructor {}

//-----------------------------------------------------------------------