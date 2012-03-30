using #Utilities;


/****************************************************/
/*  ShapeRAD functions:                             */
/*		shapeHit (pt, currProject, lookforshape)    */
/*		shapefirstHit (pt, currProject)             */
/*		shapeListHit (rect, currProject)            */
/*		redraw (currProject)                        */
/*		drawPatterns(currProject)                   */
/*		patternHit (pt, currProject)                */
/****************************************************/

/**
*  Description: Look if there is constituent in point pt.
*  Arguments:
*  		pt          , the point which mouse clicked
*  		currProject , the project which raiden loaded
*  		lookforshape, true if don't look for segment
*  Returns: The constituent which is in point pt, if don't find, return nil.
**/
function shapeHit (pt, currProject, lookforshape) {
	assert(pt!=nil and currProject!=nil);
	//
	constituents = currProject.currentPattern.constituents;
	//	check all others in current constituents
	// check line segments
	if (not lookforshape) {
		segments = [ "Address", "Use", "Offer", "PartOF" ];
		foreach (type, segments) {
			elements = constituents[type];
			foreach (element, elements)
				if (element.onCollision(pt))
					return element;
		}
	}
	//	check priority stack
	priorityStack = constituents.priorityStack;
	foreach (pstack, priorityStack) {
		if (constituents[pstack.type][pstack.id].onCollision(pt)) {
			if (lookforshape) {
				if (constituents[pstack.type][pstack.id].superclass=="Shape")
					return constituents[pstack.type][pstack.id];
			}
			else
				return constituents[pstack.type][pstack.id];
		}
	}
	// check shapes
	shapes = [ "Requirement", "Operation", "Role", "Input", "Output" ];
	foreach (type, shapes) {
		elements = constituents[type];
		foreach (element, elements) {
			if (element.onCollision(pt)) {
				return element;
			}
		}
	}
	return nil;
}

/**
*  Description: Check with priority first for shape and then for segment
*  Arguments:
*  		pt          , the point which mouse clicked
*		currProject , the project which raiden loaded
*  Returns: The constituent which is in point pt, if don't find, return nil.
**/
function shapefirstHit (pt, currProject) {
	assert(pt!=nil and currProject!=nil);
	//
	constituents = currProject.currentPattern.constituents;
	//	check all others in current constituents
	// check shapes
	priorityStack = constituents.priorityStack;
	foreach (pstack, priorityStack) {
		if (constituents[pstack.type][pstack.id].superclass=="Shape" and constituents[pstack.type][pstack.id].onCollision(pt))
			return constituents[pstack.type][pstack.id];
	}
	shapes = [ "Requirement", "Operation", "Role", "Input", "Output" ];
	foreach (type, shapes) {
		elements = constituents[type];
		foreach (element, elements) {
			if (element.onCollision(pt)) {
				return element;
			}
		}
	}
	// check line segments
	priorityStack = constituents.priorityStack;
	foreach (pstack, priorityStack) {
		if (constituents[pstack.type][pstack.id].superclass=="LineSegment" and constituents[pstack.type][pstack.id].onCollision(pt))
			return constituents[pstack.type][pstack.id];
	}
	segments = [ "Address", "Use", "Offer", "PartOF" ];
	foreach (type, segments) {
		elements = constituents[type];
		foreach (element, elements)
			if (element.onCollision(pt))
				return element;
	}
	return nil;
}

/**
*  Description: this function do not used..
*  Finds all the constituents which rect contains.
*  ShapeList : list which contains Obj [{"type":type},{"id":id}]
**/
function shapeListHit (rect, currProject) {
	assert(rect!=nil and currProject!=nil);
	shapeList = std::list_new();
	constituents = currProject.currentPattern.constituents;
	//	check priority stack
	priorityStack = constituents.priorityStack;
	foreach (pstack, priorityStack) {
		currConstituent = constituents[pstack.type][pstack.id];
		if (currConstituent.superclass=="Shape" and currConstituent.onCollisionRect(rect))
			shapeList.push_back([{"type":pstack.type},{"id":pstack.id}]);
	}
	//	check all others in current constituent
	// check shapes
	shapes = [ "Requirement", "Operation", "Role", "Input", "Output" ];
	foreach (type, shapes) {
		elements = constituents[type];
		foreach (element, elements)
			if (element.onCollisionRect(rect))
				shapeList.push_back([{"type":element.class},{"id":element.id}]);
	}
	return shapeList;
}

/**
*  Description: Redraw action area of current pattern
*  Arguments:	2 args when not used paint event because of a lot of events per sec!
*  		currProject, the project which raiden loaded
*  Returns: nope
**/
function redraw (currProject...) {
	if (std::isnil(currProject))
		return;
	pattern = currProject.currentPattern;
	constituents = pattern.constituents;
	frame  = pattern.window;
	//
	dc = nil;
	if (arguments.total()==1)
		dc = wx::paintdc_construct(frame);
	else
		dc = wx::clientdc_construct(frame);
	buffer = wx::buffereddc_construct(dc, pattern.virtualsize);
	frame.preparedc(dc);
	dc.setuserscale(frame.scale, frame.scale);
	
	// draw background
	local pen = wx::pen_construct(colour = wx::colour_construct(212,220,237));
	buffer.setpen(pen);
	pen.destruct();
	local brush = wx::brush_construct(colour);
	buffer.setbrush(brush);
	brush.destruct();
	colour.destruct();
	buffer.drawrectangle(wx::point_construct(0, 0), pattern.virtualsize);
	// draw line segments
	segments = [ "Address", "Use", "Offer", "PartOF" ];
	foreach (type, segments) {
		elements = constituents[type];
		foreach (element, elements) {
			element.drawElement(buffer);
		}
	}
	// draw shapes
	// draw them by priority stack
	priorityStack = constituents.priorityStack;
	it = std::listiter_new();
	it.setend(priorityStack);
	while(it.checkbegin(priorityStack)==false) {
		it.bwd();
		value = it.getval();
		constituents[value.type][value.id].drawElement(buffer);
	}
	// draw if there is line and extra...
	drawline = frame.drawline;
	if (drawline!=nil) {
		local pen	= wx::pen_construct(colour = wx::colour_construct(0,0,0),2,wx::DOT);
		buffer.setpen(pen);
		pen.destruct();
		colour.destruct();
		buffer.drawline(drawline.point1, drawline.point2);
		drawline.constituent.drawElement(buffer);
		enable = drawline.enable;
		if (enable!=nil)
			enable.drawEnable(buffer);
		// unpin data for extra draw line and enable shape
		frame.drawline	= nil;
	}
	// fire paint the screen and in paint event stop th recall of event
	buffer.destruct();
	dc.destruct();
}

/**
 *  Description: create bitmap for the pattern
 *
 **/
 function createBitmapPattern (pattern) {
	if (std::isnil(pattern))
		return;
	constituents = pattern.constituents;
	dc     = wx::clientdc_construct();
	// the bitmap of pattern
	bitmap = wx::bitmap_construct(pattern.virtualsize.getwidth(), pattern.virtualsize.getheight());
	buffer = wx::buffereddc_construct(dc, bitmap);
	// draw background
	local pen = wx::pen_construct(colour = wx::colour_construct(212,220,237));
	buffer.setpen(pen);
	pen.destruct();
	local brush = wx::brush_construct(colour);
	buffer.setbrush(brush);
	brush.destruct();
	colour.destruct();
	buffer.drawrectangle(wx::point_construct(0,0), pattern.virtualsize);
	// draw line segments
	segments = [ "Address", "Use", "Offer", "PartOF" ];
	foreach (type, segments) {
		elements = constituents[type];
		foreach (element, elements) {
			element.drawElement(buffer);
		}
	}
	//  priority stack has all the shapes
	//	draw priority stack shapes
	priorityStack = constituents.priorityStack;
	it = std::listiter_new();
	it.setend(priorityStack);
	while(it.checkbegin(priorityStack)==false) {
		it.bwd();
		value = it.getval();
		constituents[value.type][value.id].drawElement(buffer);
	}
	//
	buffer.destruct();
	return bitmap;
}


/**
*  Description: Redraw All patterns in main project page
*  Arguments:
*		currProject, The project which raiden loaded
*  Returns: nope
**/
function drawPatterns(currProject) {
	if (std::isnil(currProject))
		return;
	patterns = currProject.patterns;
	frame = currProject.window;
	width  = frame.getsize().getwidth();
	height = frame.getsize().getheight();
	dc = frame.dc;
	buffer = wx::buffereddc_construct(dc, wx::size_construct(width,height));
	local pen = wx::pen_construct(colour = wx::colour_construct(214,240,245));
	buffer.setpen(pen);
	pen.destruct();
	local brush = wx::brush_construct(colour);
	buffer.setbrush(brush);
	brush.destruct();
	buffer.drawrectangle(wx::point_construct(0,0), wx::size_construct(width,height));
	//
	it = std::listiter_new();
	it.setend(patterns);
	while(it.checkbegin(patterns)==false) {
		it.bwd();
		pattern = it.getval();
		//
		local pen = wx::pen_construct(colour = wx::colour_construct(0,0,0), 2);
		buffer.setpen(pen);
		pen.destruct();
		colour.destruct();
		local brush = wx::brush_construct(colour = wx::colour_construct(255,249,130));
		buffer.setbrush(brush);
		brush.destruct();
		colour.destruct();
		buffer.drawrectangle(pattern.rect);
		strlist = std::strtokenise(pattern.name," ");
		strlength = 0;
		line = 1;
		strname = "";
		previousname = "";
		previousstr = "";
		// fix line draw for pattern name if it is long.
		foreach (str, strlist) {
			strlength += std::strlen(str) + 1;
			previousstr = str;
			previousname = strname;
			strname += str + " ";
			if (strlength>=24) {
				if (strlength>=30) {
					buffer.drawtext(previousname, pattern.rect.getx()+15, pattern.rect.gety()+(15*line));
					strname = previousstr+" ";
					strlength = std::strlen(strname);
				}
				else {
					buffer.drawtext(strname, pattern.rect.getx()+15, pattern.rect.gety()+(15*line));
					strname = "";
					strlength = 0;
				}
				++line;
			}
		}
		buffer.drawtext(strname, pattern.rect.getx()+15, pattern.rect.gety()+(15*line));
	}
	buffer.destruct();
}

/**
*  Description: Look if there is pattern in point pt.
*  Arguments:
*  		pt          , the point which mouse clicked
*  		currProject , the project which raiden loaded
*  Returns: The pattern which is in point pt, if don't find, return nil.
**/
function patternHit (pt, currProject) {
	assert(pt!=nil and currProject!=nil);
	patterns = currProject.patterns;
	foreach (pattern, patterns)
		if (pattern.rect.contains(pt))
			return pattern;
	return nil;
}


/**
 * Description: paint events for Raiden
 *
 **/
function onPaint (frame, ev) {
	::redraw(frame.currentProject);
	ev.skip();
}

/**
 *  Description: This empty body function needs to intercept the EVT_ERASE_BACKGROUND event and 
 *               this way fixing the flicker-free drawing.
 **/
function onEraseBackground (frame, ev) {
}

