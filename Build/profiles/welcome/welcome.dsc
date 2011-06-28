using std;
using #main;
spw  = main::Sparrow();

const SHELL_LEFT_TOP		= 0; 
const SHELL_LEFT_BOTTOM 	= 1; 
const SHELL_RIGHT_TOP		= 2; 
const SHELL_RIGHT_BOTTOM	= 3;
const SHELL_BOTTOM_LEFT		= 4; 
const SHELL_BOTTOM_RIGHT	= 5; 
const SHELL_CENTER			= 6;
const SHELL_NOWHERE			= -1;

spw::print(spw);
spw::print(spw.components);

spw.components.Shell.AddComponent("AdaptationManager", SHELL_LEFT_TOP);
spw.components.Shell.AddComponent("ComponentSpy", SHELL_CENTER);
