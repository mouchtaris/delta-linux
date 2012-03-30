using #ImageHolder;


toolchoices = [
	#none,
	
	#shape_Requirement,
	#shape_Operation,
	#shape_Output,
	#shape_Input,
	
	#segment_Address,
	#segment_Use,
	#segment_Offer,
	#segment_PartOF
];


//
toolSelected = nil;
toolHovered  = nil;
//
toolChoice = #none;

/**
 *  Description: construct tools - toolbar and draw them
 **/
function Tools (...) {
	static tools;
	if (not tools) {
		h = 50;
		tools = [];
		tools.window = arguments[0];
		tools.width  = arguments[1];
		tools.height = arguments[2];
		tools.data = [];
		tools.data.requirement = [];
		tools.data.operation   = [];
		tools.data.role        = [];
		tools.data.input       = [];
		tools.data.output      = [];
		// Element Relationships
		tools.data.address = [];
		tools.data.use	   = [];
		tools.data.offer   = [];
		tools.data.partof = [];
		
		rectWidth = tools.width -5;
		
		tools.data.requirement.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.requirement.name = "shape_Requirement";
		tools.data.requirement.txt = "Requirement";
		tools.data.requirement.bitmap = ImageHolder::images().toolRequirement;
		h += 21;
		tools.data.operation.rect   = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.operation.name = "shape_Operation";
		tools.data.operation.txt = "Operation";
		tools.data.operation.bitmap = ImageHolder::images().toolOperation;
		h += 21;
		tools.data.role.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.role.name = "shape_Role";
		tools.data.role.txt = "Role";
		tools.data.role.bitmap = ImageHolder::images().toolRole;
		h += 21;
		tools.data.input.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.input.name = "shape_Input";
		tools.data.input.txt = "Input";
		tools.data.input.bitmap = ImageHolder::images().toolInput;
		h += 21;
		tools.data.output.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.output.name = "shape_Output";
		tools.data.output.txt = "Output";
		tools.data.output.bitmap = ImageHolder::images().toolOutput;
		h = 194;
		tools.data.address.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.address.name = "segment_Address";
		tools.data.address.txt = "Address";
		tools.data.address.bitmap = ImageHolder::images().toolAddress;
		h += 21;
		tools.data.use.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.use.name = "segment_Use";
		tools.data.use.txt = "Use";
		tools.data.use.bitmap = ImageHolder::images().toolUse;
		h += 21;
		tools.data.offer.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.offer.name = "segment_Offer";
		tools.data.offer.txt = "Offer";
		tools.data.offer.bitmap = ImageHolder::images().toolOffer;
		h +=21;
		tools.data.partof.rect = wx::rect_construct(wx::point_construct(2,h),wx::size_construct(rectWidth,20));
		tools.data.partof.name = "segment_PartOF";
		tools.data.partof.txt = "PartOF";
		tools.data.partof.bitmap = ImageHolder::images().toolPartOF;
//  bitmap for background toolbar colour
		tools.backgroundBitmap  = wx::bitmap_construct(rectWidth,20);
		tools.backgroundToolBar = wx::bitmap_construct(tools.width+20,20);
		tmp = [
			method (width, height) {
				
				self.height = height;
				self.width  = width;
				width -= 5;
				foreach (tool, self.data)
					tool.rect.setwidth(width);
				self.backgroundBitmap.destruct();
				self.backgroundBitmap = wx::bitmap_construct(width,20);
				memDC = wx::memorydc_construct();
				memDC.selectobject(self.backgroundBitmap);
				colNum = 241;
				for(i=0;i<width;++i) {
					local pen = wx::pen_construct(colour = wx::colour_construct(colNum, colNum, colNum), 1);
					memDC.setpen(pen);
					pen.destruct();
					colour.destruct();
					memDC.drawline(i, 0, i, 20);
					if(i%10==0)
						colNum = colNum - 1;
				}
				memDC.selectobject(wx::bitmap_construct());
				memDC.destruct();
				//
				self.backgroundToolBar.destruct();
				self.backgroundToolBar = wx::bitmap_construct(self.width,self.height);
				memDC = wx::memorydc_construct();
				memDC.selectobject(self.backgroundToolBar);
				colNum = 241;
				for(i=0;i<self.width;++i) {
					local pen = wx::pen_construct(colour = wx::colour_construct(colNum, colNum, colNum), 1);
					memDC.setpen(pen);
					pen.destruct();
					colour.destruct();
					memDC.drawline(i, 0, i, height);
					if(i%10==0)
						colNum = colNum - 1;
				}
				memDC.selectobject(wx::bitmap_construct());
				
			},
			method (pt) {
				foreach (tool, self.data)
					if (tool.rect.contains(pt))
						return tool;
				return nil;
			}
		];
		std::tabsetmethod(tools, tmp, "setwidth",0);
		std::tabsetmethod(tools, tmp, "onCollision", 1);
		tools.setwidth(tools.width, tools.height);
	}
	return tools;
}

/**
 *  Description: draw the toolbox
 **/
function drawToolBox () {
	window = ::Tools(...).window;
	width  = window.getsize().getwidth();
	height = window.getsize().getheight();
	buffer = wx::buffereddc_construct(window.dc, wx::size_construct(width, height));
	// draw background
	wx::dc_drawbitmap(buffer,::Tools().backgroundToolBar,0,0);
/*	local brush = wx::brush_construct(colour = wx::colour_construct(167,167,167));
	buffer.setbrush(brush);
	brush.destruct();
	colour.destruct();
	local pen = wx::pen_construct(colour = wx::colour_construct(167,167,167), 2);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	buffer.drawrectangle(0, 0, width, height);*/
	local pen = wx::pen_construct(colour = wx::colour_construct(160,160,160), 2);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	local brush = wx::brush_construct(colour = wx::colour_construct(167,167,167));
	buffer.setbrush(brush);
	brush.destruct();
	colour.destruct();
	buffer.drawrectangle(2, 2, width-3, 18);
	buffer.settextforeground(colour = wx::colour_construct(255,255,255));
	colour.destruct();
	buffer.drawtext("Toolbox",5,2);
	local pen = wx::pen_construct(colour = wx::colour_construct(190,190,190),2);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	local brush = wx::brush_construct(colour = wx::colour_construct(255,255,255));
	buffer.setbrush(brush);
	brush.destruct();
	colour.destruct();
	buffer.drawrectangle(2, 22, width-3, 17);
	colNum = 221;
	w = width-3;
	x = 22;
	for(i=1;i<=15;++i) {
		local pen = wx::pen_construct(colour = wx::colour_construct(colNum, colNum, colNum), 1);
		buffer.setpen(pen);
		pen.destruct();
		colour.destruct();
		buffer.drawline(2, x, w, x);
		colNum -= 2;
		++x;
	}
	local bitmap = ImageHolder::images().toolbarArrow;
	buffer.drawbitmap(bitmap, wx::point_construct(10,28), true);
	buffer.settextforeground(colour = wx::colour_construct(0,0,0));
	colour.destruct();
	buffer.drawtext("Element", 25, 24);
	local pen = wx::pen_construct(colour = wx::colour_construct(190,190,190),2);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	buffer.drawline(1, 42, width-1, 42);
	// Element Links
	l = 171;
	d = 165;
	local pen = wx::pen_construct(colour = wx::colour_construct(190,190,190),2);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	buffer.drawrectangle(2, d, w, 20);
	local pen = wx::pen_construct(colour = wx::colour_construct(190,190,190),2);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	local brush = wx::brush_construct(colour = wx::colour_construct(255,255,255));
	buffer.setbrush(brush);
	buffer.drawrectangle(2, d, w, 17);
	colNum = 221;
	++d;
	for(i=1;i<=15;++i) {
		local pen = wx::pen_construct(colour = wx::colour_construct(colNum, colNum, colNum), 1);
		buffer.setpen(pen);
		pen.destruct();
		colour.destruct();
		buffer.drawline(2, d, w, d);
		colNum -= 2;
		++d;
	}
	local bitmap = ImageHolder::images().toolbarArrow;
	buffer.drawbitmap(bitmap,wx::point_construct(10,l),true);
	buffer.settextforeground(colour = wx::colour_construct(0,0,0));
	colour.destruct();
	buffer.drawtext("Element Relationships", 25, l-5);
	tools = ::Tools(width,height).data;
	local font = wx::font_construct(9,wx::FONTBTN_DEFAULT_STYLE,wx::FONTBTN_DEFAULT_STYLE,10);
	buffer.setfont(font);
	font.destruct();
	local pen = wx::pen_construct(colour = wx::colour_construct(51,153,255), 1);
	buffer.setpen(pen);
	pen.destruct();
	colour.destruct();
	foreach(tool, tools) {
		if (::toolSelected==tool) {
			local brush = wx::brush_construct(colour = wx::colour_construct(218,235,252));
			buffer.setbrush(brush);
			brush.destruct();
			colour.destruct();
			buffer.drawrectangle(tool.rect); }
		else
		if (::toolHovered==tool) {
			local brush = wx::brush_construct(colour = wx::colour_construct(194,226,255));
			buffer.setbrush(brush);
			brush.destruct();
			colour.destruct();
			buffer.drawrectangle(tool.rect);
		}
		buffer.drawbitmap(tool.bitmap, tool.rect.getx()+6, tool.rect.gety()+3, true);
		buffer.drawtext(tool.txt, tool.rect.getx()+40, tool.rect.gety()+2);
	}
	buffer.destruct();
}


/////
//   Events of Tools
////

/**
 *  Description: set size of tool bar and redraw the window
 **/
function onResizeEvent (frame, ev) {
	::Tools().setwidth(frame.getsize().getwidth(), frame.getsize().getheight());
	::drawToolBox();
}
/**
 *  Description: redraw the window without hovered draw because user is out of window now
 **/
function onLeaveWindow (frame, ev) {
	::toolHovered = nil;
	::drawToolBox();
}
/**
 *  Description: redraw the toolbox when user is back to the window
 **/
function onEnterWindow (frame, ev) {
	::drawToolBox();
}

/**
 *  Description: user mouse over the toolbar and have to redraw the toolbar as moused overed up to tool
 **/
function onMotionEvent (frame, ev) {
	if ( (tool=::Tools().onCollision(ev.getposition())) != nil) {
		if( tool!=::toolHovered and tool!=toolSelected ) {
			buffer = wx::buffereddc_construct(frame.dc, frame.getsize());
			local font = wx::font_construct(9,wx::FONTBTN_DEFAULT_STYLE,wx::FONTBTN_DEFAULT_STYLE,10);
			buffer.setfont(font);
			font.destruct();
			if (::toolHovered!=nil) {
				buffer.drawbitmap(::Tools().backgroundBitmap, ::toolHovered.rect.getposition());
				buffer.drawbitmap(::toolHovered.bitmap, ::toolHovered.rect.getx()+6, ::toolHovered.rect.gety()+3, true);
				buffer.drawtext(::toolHovered.txt, ::toolHovered.rect.getx()+40, ::toolHovered.rect.gety()+2);
			}
			::toolHovered = tool;
			local pen = wx::pen_construct(colour = wx::colour_construct(51,153,255), 1);
			buffer.setpen(pen);
			pen.destruct();
			colour.destruct();
			local brush = wx::brush_construct(colour = wx::colour_construct(194,226,255));
			buffer.setbrush(brush);
			brush.destruct();
			colour.destruct();
			buffer.drawrectangle(::toolHovered.rect);
			buffer.drawbitmap(::toolHovered.bitmap, ::toolHovered.rect.getx()+6, ::toolHovered.rect.gety()+3, true);
			buffer.drawtext(::toolHovered.txt, ::toolHovered.rect.getx()+40, ::toolHovered.rect.gety()+2);
			buffer.destruct();
		}
		else
		if (::toolHovered!=nil and tool==toolSelected) {
			buffer = wx::buffereddc_construct(frame.dc, frame.getsize());
			local font = wx::font_construct(9,wx::FONTBTN_DEFAULT_STYLE,wx::FONTBTN_DEFAULT_STYLE,10);
			buffer.setfont(font);
			font.destruct();
			buffer.drawbitmap(::Tools().backgroundBitmap, ::toolHovered.rect.getposition());
			buffer.drawbitmap(::toolHovered.bitmap, ::toolHovered.rect.getx()+6, ::toolHovered.rect.gety()+3, true);
			buffer.drawtext(::toolHovered.txt, ::toolHovered.rect.getx()+40, ::toolHovered.rect.gety()+2);			
			::toolHovered = nil;
			buffer.destruct();
		}
	}
	else
	if (::toolHovered!=nil) {
		buffer = wx::buffereddc_construct(frame.dc, frame.getsize());
		local font = wx::font_construct(9,wx::FONTBTN_DEFAULT_STYLE,wx::FONTBTN_DEFAULT_STYLE,10);
		buffer.setfont(font);
		font.destruct();
		buffer.drawbitmap(::Tools().backgroundBitmap, ::toolHovered.rect.getposition());
		buffer.drawbitmap(::toolHovered.bitmap, ::toolHovered.rect.getx()+6, ::toolHovered.rect.gety()+3, true);
		buffer.drawtext(::toolHovered.txt, ::toolHovered.rect.getx()+40, ::toolHovered.rect.gety()+2);
		::toolHovered = nil;
		buffer.destruct();
	}
}

/**
 *  Description: user maybe tried to choose a tool, set the choice and fix redraw
 **/
function onLeftDownEvent (frame, ev) {
	if ( (tool=::Tools().onCollision(ev.getposition())) != nil) {
		buffer = wx::buffereddc_construct(frame.dc, frame.getsize());
		if( tool!=::toolSelected ) {
			local font = wx::font_construct(9,wx::FONTBTN_DEFAULT_STYLE,wx::FONTBTN_DEFAULT_STYLE,10);
			buffer.setfont(font);
			font.destruct();
			if (::toolSelected!=nil) {
				buffer.drawbitmap(::Tools().backgroundBitmap, ::toolSelected.rect.getposition());
				buffer.drawbitmap(::toolSelected.bitmap, ::toolSelected.rect.getx()+6, ::toolSelected.rect.gety()+3, true);
				buffer.drawtext(::toolSelected.txt, ::toolSelected.rect.getx()+40, ::toolSelected.rect.gety()+2);
			}
			::toolSelected = tool;
			::toolChoice = tool.name;
			::toolHovered  = nil;
			local pen = wx::pen_construct(colour = wx::colour_construct(51,153,255), 1);
			buffer.setpen(pen);
			pen.destruct();
			colour.destruct();
			local brush = wx::brush_construct(colour = wx::colour_construct(218,235,252));
			buffer.setbrush(brush);
			brush.destruct();
			colour.destruct();
			buffer.drawrectangle(::toolSelected.rect);
			buffer.drawbitmap(::toolSelected.bitmap, ::toolSelected.rect.getx()+6, ::toolSelected.rect.gety()+3, true);
			buffer.drawtext(::toolSelected.txt, ::toolSelected.rect.getx()+40, ::toolSelected.rect.gety()+2);
		}
		else {
			::toolSelected = nil;
			::toolChoice = #none;
			::toolHovered = tool;
			local font = wx::font_construct(9,wx::FONTBTN_DEFAULT_STYLE,wx::FONTBTN_DEFAULT_STYLE,10);
			buffer.setfont(font);
			font.destruct();
			local pen = wx::pen_construct(colour = wx::colour_construct(51,153,255), 1);
			buffer.setpen(pen);
			pen.destruct();
			colour.destruct();
			local brush = wx::brush_construct(colour = wx::colour_construct(194,226,255));
			buffer.setbrush(brush);
			brush.destruct();
			colour.destruct();
			buffer.drawrectangle(::toolHovered.rect);
			buffer.drawbitmap(::toolHovered.bitmap, ::toolHovered.rect.getx()+6, ::toolHovered.rect.gety()+3, true);
			buffer.drawtext(::toolHovered.txt, ::toolHovered.rect.getx()+40, ::toolHovered.rect.gety()+2);
		}
		buffer.destruct();
	}
}


/////

function connectToolBar (window) {
	window.connect(wx::EVT_LEFT_DOWN, ::onLeftDownEvent);
	window.connect(wx::EVT_MOTION   , ::onMotionEvent);	
}

function disconnectToolBar (window) {
	window.disconnect(wx::EVT_LEFT_DOWN, ::onLeftDownEvent);
	window.disconnect(wx::EVT_MOTION   , ::onMotionEvent);
}

function loadMainToolBar (window) {
	window.dc = wx::clientdc_construct(window);
	width  = window.getsize().getwidth();
	height = window.getsize().getheight();
// initialize tools	
	::Tools(window, width, height);
// draw toolbox rect with buttons
	::drawToolBox();
// connections of toolbar
	window.connect(wx::EVT_LEAVE_WINDOW, ::onLeaveWindow);
	window.connect(wx::EVT_ENTER_WINDOW, ::onEnterWindow);
	window.connect(wx::EVT_SIZE     , ::onResizeEvent);
	return window;
}


/**
 *  Description: get the current user choice
 *  Arguments: nope
 *  Returns: The tool choice of user. values must be from table toolchoices
 **/
function GetToolChoice () {
	return ::toolChoice;
}
/**
 *  Description: sets the tool choice of program(ConstituentsMouseEvents) and updates redraw the toolbox
 **/
function SetToolChoice (tool) {
	::toolChoice = tool;
	::toolSelected = nil;
	::toolHovered  = nil;
	::drawToolBox();
}

