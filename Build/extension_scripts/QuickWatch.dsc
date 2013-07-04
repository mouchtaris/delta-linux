/**
 *	QuickWatch.dsc
 *
 *	-- Sparrow Quick Watch --
 *
 *	Quick watch view of a Delta expression during debugging
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2008
 */

using std;
using #sparrowlib;
spw = sparrowlib::sparrow();

const classId = "QuickWatch";

inBreakpoint = false;
quickWatches = [];

//-----------------------------------------------------------------------

function QuickWatchWindow(window, expr) {
	window = spw.decorate(window);
	window.AddNavigationButtons();
	
	local treeview = spw.decorate(window.CreateContainedComponent("ExpressionTreeListView"));
	treeview.SetColumns(list_new("Expression:150", "Value:400"));
	treeview.Append(0, list_new("", ""));
	local result = treeview.SetSingleSelectionMode(true);
	assert result;	// We should always have the root before setting 
					// the selection mode and it should always succeed
	local watchId = 0;
	if (expr != "") {
		window.SetExpression(expr);
		watchId = treeview.InsertExpression(0, expr);
	}

	local watch = [
		@window : window,
		@treeview : treeview,
		@watchId : watchId,
		@undoStack : list_new(),
		@redoStack : list_new(),
		@isPerformingAction : false,
		
		{.isValidAction : (function(action) {
			return	action.type != nil and typeof(action.type) == "String" and
					action.id != nil and typeof(action.id) == "Number";
		})},
		
		method opposite (action){
			assert @isValidAction(action);
			if (std::isundefined(static opposites))
				opposites = [{"expand" : "collapse"}, {"collapse" : "expand"}];
			return [{.type : opposites[action.type]}, {.id : action.id}];
		},
		
		method perform(action)
		{
			@isPerformingAction = true;
			assert @isValidAction(action);
			if (action.type == "expand")
				@treeview.Expand(action.id);
			else if (action.type == "collapse")
				@treeview.Collapse(action.id);
			else
				assert false;
			@treeview.SetSingleSelection(action.id);
			@isPerformingAction = false;
		},
		
		method undo {
			if (@undoStack.total() > 0) {
				local action = @undoStack.pop_front();
				@redoStack.push_front(@opposite(action));
				@perform(action);
			}
		},
		
		method redo {
			if (@redoStack.total() > 0) {
				local action = @redoStack.pop_front();
				@undoStack.push_front(@opposite(action));
				@perform(action);
			}
		},
		
		method ExpandValueColumn(width) {
			local exprWidth = @treeview.GetColumnWidth(0);
			local valueWidth = @treeview.GetColumnWidth(1);
			if (exprWidth + valueWidth < width)
				@treeview.SetColumnWidth(1, width - exprWidth - 6);
		},

		method EvaluateExpression(expr) {
			if (inBreakpoint) {
				@window.SetExpression(expr);
				if (@watchId)
					@treeview.RemoveExpression(@watchId);
				if (expr != "")
					@watchId = @treeview.InsertExpression(0, expr);
				
				@undoStack.clear();
				@redoStack.clear();
			}
		},
		method Refresh {
			local children = @treeview.GetChildren(@treeview.GetRoot(), false);
			assert typeof(children) == TYPEOF_EXTERNID and externidtype(children) == "std::list";
			assert children.total() <= 1;
			if (children.total())
				@watchId = strtonum(@treeview.UpdateExpression(children.front()).first);
		},
		
		method Disable {
			if (isundefined(static INVALID)) {
				function CreateColor(r, g, b)
					{ return [ {.class : "wxColour"}, {.r : r}, {.g : g}, {.b : b} ]; }
				INVALID = CreateColor(190, 190, 190);	//GRAY
			}
			foreach(local watch, @treeview.GetChildren(@treeview.GetRoot(), true))
				@treeview.SetTextColor(watch, INVALID);
		},
		
		method onItemSelected(id) {
			spw::basecall(@treeview, "OnItemSelected", id);
			@window.SetDisplayedExpression(@treeview.GetFullExpression(id));
		},
		method onItemExpanding(id) {
			spw::basecall(@treeview, "OnItemExpanding", id);
			if (not @isPerformingAction)
				@undoStack.push_front([{.type : "collapse"}, {.id : id}]);
		},
		method onItemCollapsing(id) {
			spw::basecall(@treeview, "OnItemCollapsing", id);
			if (not @isPerformingAction)
				@undoStack.push_front([{.type : "expand"}, {.id : id}]);
		},
		
		method onClose {
			if (not @closing) {
				@closing = true;
				@undoStack.clear();
				@redoStack.clear();
				quickWatches[@window.serial] = nil;
				local shell = spw.components.Shell;
				if (shell.serial != 0 and @window)
					shell.RemoveComponent(@window);
			}
		},
		
		method onAddWatch(expr) {
			local watches = spw.components.ExpressionWatches;
			if (watches.serial != 0)
				watches.AddWatch(expr);
		}
	];

	spw::inst_impl_dynamic_member_function(window, "OnBack", watch.undo, "void (void)");
	spw::inst_impl_dynamic_member_function(window, "OnForward", watch.redo, "void (void)");
	spw::inst_impl_dynamic_member_function(window, "OnReevaluate", watch.EvaluateExpression, "void (const String& expression)");
	spw::inst_impl_dynamic_member_function(window, "OnAddWatch", watch.onAddWatch, "void (const String& expression)");
	spw::inst_impl_dynamic_member_function(window, "OnExpressionChanged", watch.EvaluateExpression, "void (const String& expression)");
	spw::inst_impl_dynamic_member_function(window, "OnViewerWidthChanged", watch.ExpandValueColumn, "void (uint width)");
	spw::inst_impl_dynamic_member_function(window, "OnClosed", watch.onClose, "void (void)");

	spw::inst_impl_dynamic_member_function(treeview, "OnItemSelected", watch.onItemSelected, "void (uint serial)");
	spw::inst_impl_dynamic_member_function(treeview, "OnItemExpanding", watch.onItemExpanding, "void (uint serial)");
	spw::inst_impl_dynamic_member_function(treeview, "OnCollapsing", watch.onItemCollapsing, "void (uint serial)");
	spw::inst_impl_dynamic_member_function(treeview, "OnWidthChanged", watch.ExpandValueColumn, "void (uint width)");
	
	return watch;
}

//-----------------------------------------------------------------------

function CloseAllQuickWatchWindows {
	foreach(local quickWatch, quickWatches)
		quickWatch.onClose();	//Use this instead of quickWatch.window.CloseDialog() to instantly
								//close the quickWatch (no signals involved). This is usefull when
								//sparrow closes during debugging and no pending signals are sent.
	assert tablength(quickWatches) == 0;
}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

onevent onBreakpointHit(invoker, uri, line) {
	foreach(local quickWatch, quickWatches)
		quickWatch.Refresh();
	inBreakpoint = true;
}

onevent onDebugResumed(invoker) {
	foreach(local quickWatch, quickWatches)
		quickWatch.Disable();
	inBreakpoint = false;
}

onevent onDebugStopped(classId, uri) { CloseAllQuickWatchWindows(); }

//-----------------------------------------------------------------------

function ShowDialog
{	
	if (inBreakpoint) {
		local expr = "";
		if (spw.components.EditorManager.serial != 0							and
			(local editor = spw.components.EditorManager.GetFocusedEditor())	and
			editor.serial != 0
		)
			expr = editor.GetSelectedText();

		local window = spw.components.Shell.AddComponent("DeltaQuickWatch", 0);
		local quickWatch = QuickWatchWindow(window, expr);
		quickWatches[quickWatch.window.serial] = quickWatch;
		quickWatch.window.ShowDialog(false);
	}
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId);
	spw::setcomponentmetadata(
		classId, "Quick Watch",
		"Quick watch view of a Delta expression during debugging",
		"Ioannis Lilis <lilis@ics.forth.gr>",
		"alpha"
	);

	spw::registerimage("watch", spw::installationdir() + "/resources/watch.png");

	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"	},
			{.class_id		: classId			},
			{.function_name	: "ShowDialogCmd"	},
			{.flags			: 7					},
			{.image			: "watch"		}
		],
		"/{110}Debug/{140}Quick Watch\tShift+F9--",
		"Show the Quick Watch dialog"
	);

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
	spw::inst_impl_required_member_command(classId, "ShowDialogCmd", ShowDialog);

	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onDebugStopped);
}

//-----------------------------------------------------------------------

onevent Destructor { CloseAllQuickWatchWindows(); }

//-----------------------------------------------------------------------