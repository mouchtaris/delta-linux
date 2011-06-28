/////////////////////////////////////////////
// Test for 'deltaaladin' library.
// A. Savidis, December 2007.
//

/////////////////////////////////////////////
// Setting the dynamic interface to ALADIN library (no compiler
// awareness).
//
aladin = [
	{ .initialise 		: "aladin_initialise" },
	{ .cleanup			: "aladin_cleanup" },
	
	{ .drawable	: [
		{ .width 				:	"drawable_width" },
		{ .height 				:	"drawable_height" },
		{ .maskedblitfrom 		:	"drawable_maskedblitfrom" },
		{ .white				:	"drawable_white" }
	] },

	{ .screen : [
		{ .instantiate 	: "screen_instantiate" },
		{ .destroy 		: "screen_destroy" },
		{ .flip 		: "screen_flip" },
		{ .get 			: "screen_get" }	
	]},

	{ .bmp : [
		{ .load 	: "bmp_load" },
		{ .unload 	: "bmp_unload" }
	]},
	
	{ .kbd : [
		{ .install 		: "kbd_install" },
		{ .uninstall 	: "kbd_uninstall" },
		{ .keypressed 	: "kbd_keypressed" },
		{ .readkey 		: "kbd_readkey" },
		{ .numtokey 	: "kbd_numtokey" }
	]}
];


/////////////////////////////////////////////

const MAX_OFFSET = 50;
const MIN_OFFSET = 2;

data = [	// Game data
	{.move : (method(dx, dy) {
				self.bx += dx;
				self.by += dy;
				if (self.bx < 0) self.bx = 0;
				if (self.by < 0) self.by = 0;
				if (self.bx + self.bw > self.sw)
					self.bx = self.sw - self.bw;
				if (self.by + self.bh > self.sh)
					self.by = self.sh - self.bh;
			})},
	
	{"offset" : MIN_OFFSET },

	{.inc : (method() { if (self.offset < MAX_OFFSET) ++self.offset; })},
	{.dec : (method() { if (self.offset > MIN_OFFSET) --self.offset; })},
	
	{.clear : 	(method() { 
					if (self.bmp) 
						aladin.bmp.unload(self.bmp); 
				})}
];


/////////////////////////////////////////////
	
function init_gfx() {
	if (not aladin.initialise("overlayed", "800x600", "24bits"))
		return false;
	if ( not aladin.screen.instantiate("doublebuffering"))
		return false;
	
	data.screen = aladin.screen.get();
	data.sw 	= aladin.drawable.width(data.screen);
	data.sh 	= aladin.drawable.height(data.screen);
	aladin.kbd.install();
}

function close_gfx() {
	if (data.screen)
		aladin.screen.destroy();
	aladin.kbd.uninstall();
	aladin.cleanup();
}

/////////////////////////////////////////////

function main_loop () {
	if (not (data.bmp = aladin.bmp.load("villain.bmp", true)))
		return;
	data.bx = data.by = 0;
	data.bw = aladin.drawable.width(data.bmp);
	data.bh = aladin.drawable.height(data.bmp);

	function display() { 
		aladin.drawable.clear(data.screen, aladin.drawable.white());
		aladin.drawable.maskedblitfrom(
			data.screen,
			data.bmp,
			data.bx, data.by, 0, 0, data.bw, data.bh
		);
		aladin.screen.flip();
	}
	
	local dispatcher = [
		{ "KEY_ESC" 		: (function() { return false; })},
		{ "KEY_LEFT" 		: (function() { data.move(-data.offset, 0); return true; } ) },
		{ "KEY_RIGHT"		: (function() { data.move(data.offset, 0); return true; } ) },
		{ "KEY_UP"			: (function() { data.move(0, -data.offset); return true; } ) },
		{ "KEY_DOWN"		: (function() { data.move(0, data.offset); return true; } ) },
		{ "KEY_MINUS_PAD" 	: (function() { data.dec(); return true; } ) },
		{ "KEY_PLUS_PAD" 	: (function() { data.inc(); return true; } ) }
	];
	
	display();
	while (true)
			if (aladin.kbd.keypressed()) {
				local f = dispatcher[aladin.kbd.numtokey(aladin.kbd.readkey())];
				if (f and not f())
					break;
			}
	data.clear();
}

/////////////////////////////////////////////

function main() {
	if (not init_gfx())
		return;
	main_loop();
	close_gfx();
}

main();

/////////////////////////////////////////////
