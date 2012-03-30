
/*
*  Dynamic using of SaveLoad
*/
function SaveLoad (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#saveload,"SaveLoad.dbc");
		saveload = std::libs::import(#saveload);
		funcs = saveload.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of Dialogs
*/
function Dialogs (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#dialogs,"Dialogs.dbc");
		dialogs = std::libs::import(#dialogs);
		funcs = dialogs.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of ProjectEvent
*/
function ProjectEvent (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#projectevent,"ProjectEvent.dbc");
		projectevent = std::libs::import(#projectevent);
		funcs = projectevent.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of PatternsEvents
*/
function PatternsEvents (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#patternsevents,"PatternsEvents.dbc");
		patternsevents = std::libs::import(#patternsevents);
		funcs = patternsevents.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of ConstituentsMouseEvents
*/
function ConstituentsMouseEvents (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#constituentsmouseev,"ConstituentsMouseEvents.dbc");
		constituentsmouseev = std::libs::import(#constituentsmouseev);
		funcs = constituentsmouseev.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of ConstituentsMouseEvents
*/
function ConstituentsEvents (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#constituentsevents,"ConstituentsEvents.dbc");
		constituentsevents = std::libs::import(#constituentsevents);
		funcs = constituentsevents.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of TreeView
*/
function TreeView (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#treeview,"TreeView.dbc");
		treeview = std::libs::import(#treeview);
		funcs = treeview.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of UndoRedo
*/
function UndoRedo (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#undoredo,"UndoRedo.dbc");
		undoredo = std::libs::import(#undoredo);
		funcs = undoredo.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of RadEvents
*/
function RadEvents (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#radevents,"RadEvents.dbc");
		radevents = std::libs::import(#radevents);
		funcs = radevents.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of Filter
*/
function Filters (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#filters,"Filters.dbc");
		filters = std::libs::import(#filters);
		funcs = filters.funcs();
	}
	return funcs[methodName];
}

/*
*  Dynamic using of Zoom
*/
function Zoom (methodName) {
	if (std::isundefined(static funcs)) {
		std::libs::registercopied(#zoom,"Zoom.dbc");
		zoom = std::libs::import(#zoom);
		funcs = zoom.funcs();
	}
	return funcs[methodName];
}
