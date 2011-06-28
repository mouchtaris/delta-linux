/////////////////////////////////////////////
// Remake of the classic Snake game via ALADIN gfx library.
// A. Savidis, July 2008, updated August 2010.
//
using std;
using aladin;
using keys;
using shiftkeys; 

aladin_dll = dllimportdeltalib(aladin::DLL);
assert aladin_dll;
dllinvoke(aladin_dll, "ShowConsole");

const VERSION = "
Remake of the snake classic game. Uses the
ALADIN direct access library, while being
implemented in the Delta language.
July, 2008 original version.
";

/////////////////////////////////////////////
// Configuration support with the resource loader.

if (not (config = std::rcload("Config.txt")))
	config = [	// Std configuration.
		@gfx : [
				@mode	:	SCREEN_MODE_OVERLAYED,
				@depth 	:	SCREEN_DEPTH_DETECT
		],
		@Data : [
			@bg		: "SnakeBg.bmp",
			@tiles	: "SnakeTiles.bmp"
		],
		@consts : [
				@TILE_W			: 27,
				@TILE_H			: 32,
				@TILES_NHORIZ	: 6,
				@TILES_NVERT	: 4,
				@TERRAIN_W		: 567,
				@TERRAIN_H		: 544,				
				@START_ROW		: 8,
				@START_COL		: 10,				
				@TERRAIN_X1		: 28,
				@TERRAIN_Y1		: 36,
				@STAGE_MENU		: "[F1: Next stage ESC: Quit]",
				@TERRAIN_MENU	: "[F1: New game ESC: Quit]",
				@RESUME_MESSAGE	: "[Paused: Space to resume]",
				@AUTOMOVE_TIME	: 200, 
				@AUTOMOVE_DEC	: 3,				
				@SCORE_X		: 676,
				@SCORE_Y		: 175,
				@STAGE_X		: 676,
				@STATE_Y		: 220,
				@HIGH_SCORE_X	: 676,
				@HIGH_SCORE_Y	: 280,
				@NUM_FLIES		: 5, 
				@NUM_APPLES		: 5,
				@INC_FLIES		: 5,
				@INC_APLPES		: 5,
				@BONUS_FLY		: 150,
				@BONUS_APPLE	: 100,
				@TEXT_COLOR		: [255, 255, 0],
				@INPUT_PACE		: 50
			]
	];

const RESOLUTION	= "800x600";
const BONUS_EXPIRE	= 3000;

consts = config.consts;
consts.TILES_BMPW	= consts.TILE_W		* consts.TILES_NHORIZ;
consts.TILES_BMPH	= consts.TILE_H		* consts.TILES_NVERT;
consts.TSIZE_HORIZ	= consts.TERRAIN_W	/ consts.TILE_W;
consts.TSIZE_VERT	= consts.TERRAIN_H	/ consts.TILE_H;
consts.TERRAIN_X2	= consts.TERRAIN_X1 + consts.TERRAIN_W - 1;
consts.TERRAIN_Y2	= consts.TERRAIN_Y1 + consts.TERRAIN_H - 1;

/////////////////////////////////////////////

Data = [
	method create {
		@back_buffer =	bitmap_construct(@sw, @sh, false);
		if (not (@bg = bitmap_load(config.Data.bg, false)))
			return false;
		if (drawable_width(@bg) != @sw or drawable_height(@bg) != @sh)
			return false;
		
		@textcolor =	 drawable_makecolor(
								consts.TEXT_COLOR[0], consts.TEXT_COLOR[1], consts.TEXT_COLOR[2]
							);															
		if (not (@tiles = bitmap_load(config.Data.tiles, false)))
			return false;
		if (drawable_width(@tiles) != consts.TILES_BMPW or drawable_height(@tiles) != consts.TILES_BMPH)
			return false; 
		return true;
	},
	
	method clear { 
		if (@bg)			bitmap_destroy(@bg); 
		if (@tiles)			bitmap_destroy(@tiles); 
		if (@back_buffer)	bitmap_destroy(@back_buffer); 
		bitmapmanager_cleanup();					
		fontmanager_cleanup();
	},
				 
	{ #fps, #lps : 0 },
	
	method fpscalculation {
		local t = std::currenttime();
		if (not @ts) 
			@ts = t;
		else {
			++@lps;								
			if (t - @ts >= 1000)
				@fps = @lps, @lps = 0, @ts = t;
		}
	}											
];

/////////////////////////////////////////////
// Tile positions for snake parts. Direction for all characterizes where
// the snake is supposed to be going.

function xy(x, y) { return [ {.x : x}, {.y : y} ]; }
	
tails 	 = [ @R : xy(0,0), @T : xy(1,0), @L : xy(2,0), @B : xy(3,0) ];
heads	 = [ @R : xy(0,1), @T : xy(1,1), @L : xy(2,1), @B : xy(3,1) ];

bodies = [	
	{ #BR, #LT: xy(2,2)}, { #TR, #LB : xy(3,2)},
	{ #RT, #BL: xy(0,2)}, { #RB, #TL : xy(1,2)},
	{ #LR, #RL: xy(4,2)}, { #BT, #TB : xy(5,2)}	
];
			
displayOffsets = [
	@LT: [1,0], @TL: [0,1],  @BR: [0,-1], @RB: [-1,0],
	@LB: [1,0], @BL: [0,-1], @TR: [0,1],  @RT: [-1,0], 
	
	@LR: [-1,0], @RL: [1,0], 
	@TB: [0,-1], @BT: [0,1],
	
	@L : [1,0], @R : [-1, 0], @T : [0,1], @B : [0,-1]
];

// B: body, H: head, O: offset.
moveChanges = [		// B	H   O(dx, dy)	
	@L:	[ @T: [ "LT", "T",	0, -1], @B: [ "LB", "B",  0, 1], @L: [ "RL", "L",-1, 0] ],
	@R:	[ @T: [ "RT", "T", 0, -1], 	@B: [ "RB", "B",  0, 1], @R: [ "LR", "R", 1, 0] ],
	@T:	[ @L: [ "TL", "L", -1, 0], 	@R: [ "TR", "R",  1, 0], @T: [ "BT", "T", 0,-1] ],
	@B:	[ @L: [ "BL", "L", -1, 0], 	@R: [ "BR", "R",  1, 0], @B: [ "TB", "B", 0, 1] ]
];

tailSubstitutions = [
	@T: [ @BT : "T", @TL: "L", @TR : "R" ],
	@B: [ @TB : "B", @BL: "L", @BR : "R" ],
	@L: [ @RL : "L", @LT: "T", @LB : "B" ],
	@R: [ @LR : "R", @RT: "T", @RB : "B" ]
];

inverseDirection = [ @L : "R", @R : "L", @T : "B", @B : "T" ];

apple = xy(0,3);
fly	  = xy(1,3);

//*******************************

Snake = [
	@initialised 	: false,
	@highscore		: 0,
	@has_won		: false,
	@grid_total		: consts.TSIZE_VERT * consts.TSIZE_HORIZ,
	@bonus_sprites 	: list_new(), 
	@version 		: VERSION,

	method reset 
		{ @score = 0, @stage = ((local n = config.Data.stage) ? n : 1); },
	 
	method start {
		@head 			= @tail = @dir = "T";
		@body 			= std::list_new("BT");
		@gameover 		= @paused = false;
		@initialised	= true;
		@x  			= consts.START_COL * consts.TILE_W;
		@y  			= consts.START_ROW * consts.TILE_H;
		@ts 			= std::currenttime();
		@time			= consts.AUTOMOVE_TIME;
		
		if (not @has_won)	// Reset only when has previously lost.
			@reset();
		else
			++@stage;
			
		@num_apples = consts.NUM_APPLES + (@stage - 1) * consts.INC_APPLES;
		@num_flies  = consts.NUM_FLIES  + (@stage - 1) * consts.INC_FLIES;

		if (@num_apples + @num_flies > @grid_total)
			@num_flies = consts.NUM_FLIES, @num_apples = consts.NUM_APPLES;
		
		@items	= [ @bag : std::list_new(), @grid: [] ];
		@makeitems(@num_flies, consts.BONUS_FLY, fly);
		@makeitems(@num_apples, consts.BONUS_APPLE, apple);
	},
	
	method pauseresume 
		{ @paused = not @paused; },
	
	method makeitems (n, bonus, type) {
		for (local i = 0, local grid = @items.grid, local bag = @items.bag; i < n; ++i) {
			if (not grid[local r = random(consts.TSIZE_VERT - 2) + 1])
				grid[r] = [];
			if (not grid[r][local c = random(consts.TSIZE_HORIZ - 2) + 1]) 
				bag.push_back(
					grid[r][c] = [
						@type 		: type, 
						@bonus 		: bonus, 
						@visible	: true,
						@x 			: c * consts.TILE_W, 
						@y 			: r * consts.TILE_H
					]
				);	
		}
		@total_bonus_items = bag.total();
	},
	
	method drawitems {
		local j = listiter_new(), (local i = listiter_new()).setbegin(local bag = @items.bag);
		while (not i.checkend(bag))
			if (not (local item = i.getval()).visible)
				j.assign(i), i.fwd(), bag.erase(j);
			else
				drawable_maskedblit(
					Data.back_buffer,
					Data.tiles,
					item.type.x * consts.TILE_W, item.type.y * consts.TILE_H, 
					consts.TERRAIN_X1 + item.x, consts.TERRAIN_Y1 + item.y,  
					consts.TILE_W, consts.TILE_H
				), 
				i.fwd();
	},
	 
	method setgameover (won)	
		{ @gameover = true; if (not (@has_won = won)) @clear_bonus(); },
		
	method accelerate			
		{ @time -= consts.AUTOMOVE_DEC; },
	
	method automove {
		if (not @paused and @initialised and not @gameover)
			if ((local t = std::currenttime()) - @ts >= @time) 
				{ @ts = t; @move(@dir, true); }
	},
				
	method incscore (d) {
		if ((@score+=d) > @highscore)
			@highscore = @score; 
		@accelerate();
	},
	
	method check_bonus_item_eaten (x,y) {
		local r = y / consts.TILE_H;
		local c = x / consts.TILE_W;
		if (@items.grid[r] and (local v = @items.grid[r][c]) and v.visible) {
			@incscore(v.bonus); @expand = true; v.visible = false; --@total_bonus_items; 
			@new_bonus_message(v.bonus, x, y);
		}
	},
	
	method all_bonus_items_eaten 
		{ return @total_bonus_items == 0; },
		
	method move (dir, auto) {
		if (@paused							or
			not @initialised				or
			@gameover						or
			inverseDirection[@dir] == dir	or
			(not auto and @dir == dir))
			return;					
		
		local plan	= moveChanges[@dir][dir];
		local x		= @x + plan[2] * consts.TILE_W;
		local y		= @y + plan[3] * consts.TILE_H;
		
		if (x < 0 or y < 0 or consts.TERRAIN_W < (x + consts.TILE_W) or consts.TERRAIN_H < (y + consts.TILE_H))
			{ @setgameover(false); return; } // Snake hits the walls.
		
		@check_bonus_item_eaten(x,y);
		
		@body.push_front(plan[0]);
		@dir 		= dir;
		@head 		= plan[1];
		@x 			= x;
		@y 			= y;
		
		if (not @expand)
			@tail = tailSubstitutions[@tail][@body.pop_back()];
		else
			@expand = false;		// Tail is retained as it is here.
			
		if (@all_bonus_items_eaten())
			@setgameover(true); 	// All insects gone.
			
		assert @tail;
	},
	
	method drawtile (tile, x, y){				
		drawable_maskedblit(
			Data.back_buffer,
			Data.tiles,
			tile.x * consts.TILE_W, tile.y * consts.TILE_H, 
			consts.TERRAIN_X1 + x, consts.TERRAIN_Y1 + y,  
			consts.TILE_W, consts.TILE_H
		);
		if (@gameover or @paused) 
			return;
		if (not @rows[local r = y / consts.TILE_H])
			@rows[r] = [];
		if (@rows[r][local c = x / consts.TILE_W])	// Snake hits its body.
			@setgameover(false);
		else
			@rows[r][c] = true;
	},

	method drawscore {
		if (@initialised and not @gameover) {
			@drawtext(@score + "Pts", consts.SCORE_X, consts.SCORE_Y, TEXT_ORIGIN_LEFT);
			@drawtext(
				"#" + @stage + ", " + @total_bonus_items + " items", 
				consts.STAGE_X, 
				consts.STAGE_Y, 
				TEXT_ORIGIN_LEFT
			);
		}
		@drawtext(@highscore + "Pts", consts.HIGH_SCORE_X, consts.HIGH_SCORE_Y, TEXT_ORIGIN_LEFT);
	},
	
	method drawtext (text, x, y, align) 
		{ drawable_textdraw(Data.back_buffer, font_getdefault(),x, y, text, align, Data.textcolor); },
	
	method message (msg) { 
		@drawtext(
			msg, 
			consts.TERRAIN_X1 + consts.TERRAIN_W / 2, consts.TERRAIN_Y1 + consts.TERRAIN_H / 2,
			TEXT_ORIGIN_CENTRE
		);
	},
		
	method new_bonus_message (bonus, x, y) {
		@bonus_sprites.push_back(
			[ 	
				@x 		: consts.TERRAIN_X1 + x, 
				@y 		: consts.TERRAIN_Y1 + y, 
				@text 	: "++" + bonus + "++", 
				@t 		: currenttime() + BONUS_EXPIRE
			]
		); 
	},
	
	method clear_bonus 
			{ @bonus_sprites.clear(); },
	
	method display_bonus_messages {
		if (not @bonus_sprites.total())
			return; 
		foreach (local b, @bonus_sprites)
			@drawtext(b.text, b.x++, b.y++, TEXT_ORIGIN_CENTRE);
		(local i = listiter_new()).setbegin(@bonus_sprites), local j = listiter_new(); 
		while (not i.checkend(@bonus_sprites))
			if (i.getval().t <= currenttime())	// Expired?
				j.assign(i), i.fwd(), @bonus_sprites.erase(j);
			else
				i.fwd();
	},
	
	method display (at) {
		@drawscore();
		if (not @initialised) 
			{ @message(consts.TERRAIN_MENU); return; }
									
		@drawitems();					
		if (not @gameover)
			{ @rows	= []; @columns = []; }
			
		@drawtile(heads[@head], local sx = @x, local sy = @y);
		
		local d = displayOffsets[@head];
		sx += d[0] * consts.TILE_W;
		sy += d[1] * consts.TILE_H;
		
		foreach (local part, @body) {
			@drawtile(bodies[part], sx, sy);
			local d = displayOffsets[part];
			sx += d[0] * consts.TILE_W, sy += d[1] * consts.TILE_H;
		}
		
		@drawtile(tails[@tail], sx, sy);
		@display_bonus_messages();
		
		if (@gameover)
			@message((@has_won ? "You Won!" + consts.STAGE_MENU :  "Game Over" + consts.TERRAIN_MENU));
		else
		if (@paused)
			@message(consts.RESUME_MESSAGE);
	}
];

/////////////////////////////////////////////
// Numbering of commands is arbitrary here, just like
// C enums.

const	COMMAND_LEFT			= 0;
const 	COMMAND_RIGHT			= 1;
const 	COMMAND_UP				= 2;
const 	COMMAND_DOWN			= 3;
const 	COMMAND_NEW				= 4;
const 	COMMAND_PAUSERESUME		= 5;
const 	COMMAND_QUIT			= 6;

const	TOTAL_KBD_COMMANDS		= 7;
const	TOTAL_JOY_COMMANDS		= 4;
const	KBD_COMMANDS_TUPLE_SIZE	= 3;
const	JOY_COMMANDS_TUPLE_SIZE	= 5;

input = [ // Input control stuff (channels, comands and bindings).
	@map : [
		@kbd : [
			// KBD_COMMANDS_TUPLE_SIZE = 3, means 3 elements per row
			COMMAND_LEFT, 			KEY_LEFT, 	"Move left",
			COMMAND_RIGHT,			KEY_RIGHT,	"Move right",
			COMMAND_UP,				KEY_UP,		"Move up",
			COMMAND_DOWN,			KEY_DOWN,	"Move down",
			COMMAND_NEW,			KEY_F1,		"New game",
			COMMAND_PAUSERESUME,	KEY_SPACE,	"Pause / resume",
			COMMAND_QUIT,			KEY_ESC,	"Exit"
		],
		@joy : [
			// JOY_COMMANDS_TUPLE_SIZE = 5, means 5 elements per row
			COMMAND_LEFT, 	JOY_XDIR_LEFT,		JOY_YDIR_CENTRE, 	JOY_ZDIR_CENTRE, "Move left",
			COMMAND_RIGHT,	JOY_XDIR_RIGHT,		JOY_YDIR_CENTRE, 	JOY_ZDIR_CENTRE, "Move right",
			COMMAND_UP,		JOY_XDIR_CENTRE,	JOY_YDIR_FRONT, 	JOY_ZDIR_CENTRE, "Move up",
			COMMAND_DOWN,	JOY_XDIR_CENTRE,	JOY_YDIR_BACK, 		JOY_ZDIR_CENTRE, "Move down"
		]
	],
		
	method initialise {
		ipon_initialise();
		ipon_newchannel(@channel = 0, "Input control");
		
		local b = kbdbinding_new();
		const N1 = KBD_COMMANDS_TUPLE_SIZE * TOTAL_KBD_COMMANDS;
		for (local i = 0, local kbd = @map.kbd; i < N1; i += KBD_COMMANDS_TUPLE_SIZE) {
			kbdbinding_reset(b);
			kbdbinding_addkey(b, kbd[i + 1]);
			ipon_setkbdcommand(@channel, kbd[i], kbd[i + KBD_COMMANDS_TUPLE_SIZE - 1], b);
		}
		
		b = joybinding_new();
		const N2 = JOY_COMMANDS_TUPLE_SIZE * TOTAL_JOY_COMMANDS;
		for (local i = 0, local joy = @map.joy; i < N2; i += JOY_COMMANDS_TUPLE_SIZE) {
			joybinding_reset(b);
			joybinding_setjoynum(b, 0);
			joybinding_setxdir(b,joy[i + 1]);
			joybinding_setzdir(b,joy[i + 3]);
			ipon_setjoycommand(@channel, joy[i], joy[i + JOY_COMMANDS_TUPLE_SIZE - 1], b);
		}
	},
	
	@ts : 0,
	
	method istimetopoll {
		if ((local t = std::currenttime()) - @ts >= consts.INPUT_PACE)
			{ @ts = t; return true; }
		return false;
	},
	
	method poll {				
		ipon_processinput();
		return ipon_gettriggeredcommands(@channel);
	},

	method cleanup { ipon_cleanup(); }
];

/////////////////////////////////////////////
	
function init_gfx {

	if (not initialise(config.gfx.mode, RESOLUTION, config.gfx.depth))
		return false;
	local buffermode = (config.gfx.mode == "fullscreen" ? SCREEN_PAGE_FLIPPING : SCREEN_DOUBLE_BUFFERING);
	if (not screen_instantiate(buffermode))
		return false;
	
	Data.screen = screen_get();
	Data.sw 	= drawable_width(Data.screen);
	Data.sh 	= drawable_height(Data.screen);
	
	kbd_install();
	joystick_install();
	if (joystick_gettotal())
		joystick_setanaloguemovethreshold(0, 100);
	mouse_install();
	mouse_setdefaultcursor();
	input.initialise();
	rcmap_initialise();
	return true;
}

/////////////////////////////////////////////

function close_gfx {
	if (Data.screen)
		screen_destroy();		
	kbd_uninstall();
	mouse_uninstall();
	input.cleanup();
	rcmap_cleanup();
	cleanup();
}

/////////////////////////////////////////////

function main_loop {
	
	if (not Data.create())
		{ Data.clear(); return; }
	
	function Display() { 
	
		drawable_cleartocolor(Data.back_buffer, drawable_black());		
		drawable_blit(
			Data.back_buffer,
			Data.bg,
			0, 0, 0, 0, 
			drawable_width(Data.bg),
			drawable_height(Data.bg)
		);
		
		Snake.display(Data.back_buffer);
		
		drawable_textdraw(
			Data.back_buffer,
			font_getdefault(),
			0, 
			0, 
			"FPS:" + Data.fps,
			"left",
			drawable_white()
		);
		
		drawable_blit(
			Data.screen,
			Data.back_buffer,
			0, 0, 0, 0,  
			Data.sw, Data.sh
		);
		screen_flip(); 		
	} 
	
	//*****************************
	
	actions = [
		 { COMMAND_QUIT 		: function { return false; } },
		 { COMMAND_LEFT 		: function { Snake.move("L", false);	return true;  }},
		 { COMMAND_RIGHT		: function { Snake.move("R", false);	return true;  }},
		 { COMMAND_UP			: function { Snake.move("T", false);	return true;  }},
		 { COMMAND_DOWN			: function { Snake.move("B", false);	return true;  }},
		 { COMMAND_PAUSERESUME	: function { Snake.pauseresume();		return true;  }},
		 { COMMAND_NEW			: function { Snake.start();				return true;  }},
		 
		method @operator()(l) {
				foreach (action, l)
					if (not self[action]())
						return false;					
			return true;
		}
	];

	//*****************************
	// Game loop.
	
	while (not iswindowcloserequested()) {
		Display();	
		if (input.istimetopoll() and not actions(input.poll()))
			break;
		Snake.automove(); 
		Data.fpscalculation();
	}
	
	Data.clear();
}

/////////////////////////////////////////////

if (init_gfx())  
	{ main_loop(); close_gfx(); }

/////////////////////////////////////////////
