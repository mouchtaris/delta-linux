/**
 *	QuickWatch.dsc
 *
 *	-- Sparrow Quick Watche --
 *
 *	Quick watch view of a Delta expression during debugging
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2008
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "QuickWatch";

inBreakpoint = false;
quickWatches = [];

//-----------------------------------------------------------------------

function QuickWatchWindow(window, expr) {
	window = spw.decorate(window);
	local treeview = spw.decorate(window.GetTreeViewWindow());
	local treeviewBase = spw.decorate(spw::mostbasecomponent(treeview));
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

	return [
		@window : window,
		@treeview : treeview,
		@treeviewBase : treeviewBase,
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
			if (action.type == "expand")
				type = "collapse";
			else if (action.type == "collapse")
				type = "expand";
			else
				assert false;
			return [{.type : type}, {.id : action.id}];
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
			@window.SetExpression(expr);
			if (@watchId)
				@treeview.RemoveExpression(@watchId);
			if (expr != "")
				@watchId = @treeview.InsertExpression(0, expr);
			
			@undoStack.clear();
			@redoStack.clear();
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
		
		method onExpanding(id) {
			if (not @isPerformingAction)
				@undoStack.push_front([{.type : "collapse"}, {.id : id}]);
		},
		method onCollapsing(id) {
			if (not @isPerformingAction)
				@undoStack.push_front([{.type : "expand"}, {.id : id}]);
		},
		
		method onClose {
			@undoStack.clear();
			@redoStack.clear();
			quickWatches[@window.serial] = nil;
			local shell = spw.components.Shell;
			if (shell.serial != 0)
				shell.RemoveComponent(@window);
		}
	];
}

//-----------------------------------------------------------------------

function MatchWindow(invoker) {
	if (local quickWatch = quickWatches[invoker.serial]) {
		local window = quickWatch.window;
		assert window;
		if (invoker.class_id == window.class_id and invoker.serial == window.serial)
			return quickWatch;
	}
	return nil;
}

function MatchTreeview(invoker) {
	foreach(local quickWatch, quickWatches) {
		local treeviewBase = quickWatch.treeviewBase;
		assert treeviewBase;
		if (invoker.class_id == treeviewBase.class_id and invoker.serial == treeviewBase.serial)
			return quickWatch;
	}
	return nil;
}

function CloseAllQuickWatchWindows {
	foreach(local quickWatch, quickWatches)
		quickWatch.onClose();	//Use this instead of quickWatch.window.CloseDialog() to instantly
								//close the quickWatch (no signals involved). This is usefull when
								//sparrow closes during debugging and no pending signals are sent.
	assert tablength(quickWatches) == 0;
}

//-------------------------------------------------------//
//---- Exported API -------------------------------------//

onevent onQuickWatchEvaluateExpression(invoker, expr) {
	if (inBreakpoint and local quickWatch = MatchWindow(invoker))
		quickWatch.EvaluateExpression(expr);
}

//-----------------------------------------------------------------------

onevent onQuickWatchAddWatch(invoker, expr)
{
	if (spw.components.ExpressionWatches)
		spw.components.ExpressionWatches.AddWatch(expr);
}

//-----------------------------------------------------------------------

onevent onQuickWatchBack(invoker) {
	if (local quickWatch = MatchWindow(invoker))
		quickWatch.undo();
}

//-----------------------------------------------------------------------

onevent onQuickWatchForward(invoker) {
	if (local quickWatch = MatchWindow(invoker))
		quickWatch.redo();
}

//-----------------------------------------------------------------------

onevent onQuickWatchTreeviewWidthChanged(invoker, treeviewWidth) {
	if (local quickWatch = MatchWindow(invoker))
		quickWatch.ExpandValueColumn(treeviewWidth);
}

//-----------------------------------------------------------------------

onevent onQuickWatchClosed(invoker) {
	if (local quickWatch = MatchWindow(invoker))
		quickWatch.onClose();
}

//-----------------------------------------------------------------------

onevent onTreeListItemSelected(invoker, id) {
	if (local quickWatch = MatchTreeview(invoker))
		quickWatch.window.SetDisplayedExpression(quickWatch.treeview.GetFullExpression(id));
}

//-----------------------------------------------------------------------

onevent onTreeListItemExpanding(invoker, id) {
	if (local quickWatch = MatchTreeview(invoker))
		quickWatch.onExpanding(id);
}

//-----------------------------------------------------------------------

onevent onTreeListItemCollapsing(invoker, id) {
	if (local quickWatch = MatchTreeview(invoker))
		quickWatch.onCollapsing(id);
}

//-----------------------------------------------------------------------

onevent onTreeListWidthChanged(invoker, treeviewWidth) {
	if (local quickWatch = MatchTreeview(invoker))
		quickWatch.ExpandValueColumn(treeviewWidth);
}

//-----------------------------------------------------------------------

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
	if (not inBreakpoint or spw.components.EditorManager.serial == 0)
		return;
	local editor = spw.components.EditorManager.GetFocusedEditor();
	if (not editor or editor.serial == 0)
		return;
	local expr = editor.GetSelectedText();

	local window = spw.components.Shell.AddComponent("DeltaQuickWatch", 0);
	local quickWatch = QuickWatchWindow(window, expr);
	quickWatches[quickWatch.window.serial] = quickWatch;
	quickWatch.window.ShowDialog(false);
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
		
	spw::class_decl_required_member_handler(classId, "QuickWatchReevaluate");
	spw::class_decl_required_member_handler(classId, "QuickWatchAddWatch");
	spw::class_decl_required_member_handler(classId, "QuickWatchBack");
	spw::class_decl_required_member_handler(classId, "QuickWatchForward");
	spw::class_decl_required_member_handler(classId, "QuickWatchExpressionChanged");
	spw::class_decl_required_member_handler(classId, "QuickWatchTreeviewWidthChanged");
	spw::class_decl_required_member_handler(classId, "QuickWatchClosed");
	spw::class_decl_required_member_handler(classId, "TreeListItemSelected");
	spw::class_decl_required_member_handler(classId, "TreeListItemExpanding");
	spw::class_decl_required_member_handler(classId, "TreeListItemCollapsing");
	spw::class_decl_required_member_handler(classId, "TreeListWidthChanged");
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
		
	spw::inst_impl_required_member_handler(classId, "QuickWatchReevaluate", onQuickWatchEvaluateExpression);
	spw::inst_impl_required_member_handler(classId, "QuickWatchAddWatch", onQuickWatchAddWatch);
	spw::inst_impl_required_member_handler(classId, "QuickWatchBack", onQuickWatchBack);
	spw::inst_impl_required_member_handler(classId, "QuickWatchForward", onQuickWatchForward);
	spw::inst_impl_required_member_handler(classId, "QuickWatchExpressionChanged", onQuickWatchEvaluateExpression);
	spw::inst_impl_required_member_handler(classId, "QuickWatchTreeviewWidthChanged", onQuickWatchTreeviewWidthChanged);
	spw::inst_impl_required_member_handler(classId, "QuickWatchClosed", onQuickWatchClosed);
	spw::inst_impl_required_member_handler(classId, "TreeListItemSelected", onTreeListItemSelected);
	spw::inst_impl_required_member_handler(classId, "TreeListItemExpanding", onTreeListItemExpanding);
	spw::inst_impl_required_member_handler(classId, "TreeListItemCollapsing", onTreeListItemCollapsing);
	spw::inst_impl_required_member_handler(classId, "TreeListWidthChanged", onTreeListWidthChanged);
	spw::inst_impl_required_member_handler(classId, "BreakpointHit", onBreakpointHit);
	spw::inst_impl_required_member_handler(classId, "DebugResumed", onDebugResumed);
	spw::inst_impl_required_member_handler(classId, "DebugStopped", onDebugStopped);
}

//-----------------------------------------------------------------------

onevent Destructor { CloseAllQuickWatchWindows(); }

//-----------------------------------------------------------------------