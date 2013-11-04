using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const SHELL_LEFT_TOP		= 0; 
const SHELL_LEFT_BOTTOM 	= 1; 
const SHELL_RIGHT_TOP		= 2; 
const SHELL_RIGHT_BOTTOM	= 3;
const SHELL_BOTTOM_LEFT		= 4; 
const SHELL_BOTTOM_RIGHT	= 5; 
const SHELL_CENTER			= 6;
const SHELL_NOWHERE			= -1;

//-- just place the framework of the editor manager, for Sparrow not to seem empty
spw.components.Shell.AddComponent("EditorManager", SHELL_CENTER);

//-- register the component directory, through which I load every delta component
spw.components.Shell.AddComponent("DeltaComponentDirectory", SHELL_RIGHT_BOTTOM);

//-- deploy debugging facilities
spw.components.DeltaComponentDirectory.CreateComponent("AutoAdjustDebugTools");

(function BreakpointsView {
	local result = spw.components.DeltaComponentDirectory.LoadComponent("BreakpointsView");
	assert result;
	spw.components.Shell.AddComponent("BreakpointsView", SHELL_BOTTOM_RIGHT);
})();

function DependencyGraph {
    local result = spw.components.DeltaComponentDirectory.LoadComponent("DependencyGraph");
    assert result;
    spw.components.Shell.AddComponent("DependencyGraph", SHELL_CENTER);
}

//-- load several utilities
(function LineCounter { spw.components.DeltaComponentDirectory.CreateComponent("LineCounter"); })();

//-- restore last workspace
spw.components.DeltaComponentDirectory.LoadComponent("RecentWorkspaces");
spw.components.RecentWorkspaces.RestoreLastWorkspace();