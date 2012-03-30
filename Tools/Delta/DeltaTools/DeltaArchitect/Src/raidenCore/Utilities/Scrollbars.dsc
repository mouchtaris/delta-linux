



/**
 * Description: fixing  the scrollbars
 *
 **/

pixels = 10;
// GLB values for the scroll events
VERTICAL	= 8;
HORIZONTAL	= 4;
// keep positionV & positionH
posV = 0;
posH = 0;
//

function onScrollPageUp (frame, ev) {
	currProject = frame.currentProject;
	pattern = currProject.currentPattern;
	vpoint  = pattern.viewPoint;
	if (ev.getorientation()==::VERTICAL) {
		::posV -= 5;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
		// calc the start point of the viewable area
		//pattern.viewPoint = wx::point_construct(vpoint.getx(), vpoint.gety()-5*::pixels);
	}
	else
	if (ev.getorientation()==::HORIZONTAL) {
		::posH -= 5;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
		//pattern.viewPoint = wx::point_construct(vpoint.getx()-5*::pixels, vpoint.gety());
	}
	//ev.skip(true);
	frame.refresh();
}

function onScrollPageDown (frame, ev) {
	currProject = frame.currentProject;
	pattern = currProject.currentPattern;
	vpoint  = pattern.viewPoint;
	if (ev.getorientation()==::VERTICAL) {
		::posV += 5;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
		//pattern.viewPoint = wx::point_construct(vpoint.getx(), vpoint.gety()+5*::pixels);
	}
	else
	if (ev.getorientation()==::HORIZONTAL) {
		::posH += 5;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
		//pattern.viewPoint = wx::point_construct(vpoint.getx()+5*::pixels, vpoint.gety());
	}
	//ev.skip(true);
	frame.refresh();
}

function onScrollLineUp (frame, ev) {
	currProject = frame.currentProject;
	pattern = currProject.currentPattern;
	vpoint  = pattern.viewPoint;
	if (ev.getorientation()==::VERTICAL) {
		if (::posV > 0)
			--::posV;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
//		pattern.viewPoint = wx::point_construct(vpoint.getx(), vpoint.gety()-::pixels);
	}
	else
	if (ev.getorientation()==::HORIZONTAL) {
		if (::posH > 0)
			--::posH;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
//		pattern.viewPoint = wx::point_construct(vpoint.getx()-::pixels, vpoint.gety());
	}
//	ev.skip(true);
	frame.refresh();
}

function onScrollLineDown (frame, ev) {
	currProject = frame.currentProject;
	pattern = currProject.currentPattern;
	vpoint  = pattern.viewPoint;
	if (ev.getorientation()==::VERTICAL) {
		if (::posV < 100)
			++::posV;
			wx::scrolledwindow_scroll(frame, ::posH, ::posV);
//		pattern.viewPoint = wx::point_construct(vpoint.getx(), vpoint.gety()+::pixels);
	}
	else
	if (ev.getorientation()==::HORIZONTAL) {
		if (::posH < 100)
			++::posH;
		wx::scrolledwindow_scroll(frame, ::posH, ::posV);
//		pattern.viewPoint = wx::point_construct(vpoint.getx()+::pixels, vpoint.gety());
	}
//	ev.skip(true);
	frame.refresh();
}

function onScrollThumbrelease (frame, ev) {
	currProject = frame.currentProject;
	pattern = currProject.currentPattern;
	vpoint  = pattern.viewPoint;
	if (ev.getorientation()==::VERTICAL) {
		::posV = ev.getposition();
		//pattern.viewPoint = wx::point_construct(vpoint.getx(), ::posV*::pixels);
	}
	else
	if (ev.getorientation()==::HORIZONTAL) {
		::posH = ev.getposition();
		//pattern.viewPoint = wx::point_construct(::posH*::pixels, vpoint.gety());
	}
	//ev.skip(true);
	frame.refresh();
}

function onScrollThumbtrack (frame, ev) {
	currProject = frame.currentProject;
	pattern = currProject.currentPattern;
	vpoint  = pattern.viewPoint;
	if (ev.getorientation()==::VERTICAL) {
		::posV = ev.getposition();
		//pattern.viewPoint = wx::point_construct(vpoint.getx(), ::posV*::pixels);
	}
	else
	if (ev.getorientation()==::HORIZONTAL) {
		::posH = ev.getposition();
		//pattern.viewPoint = wx::point_construct(::posH*::pixels, vpoint.gety());
	}
	wx::scrolledwindow_scroll(frame, ::posH, ::posV);
	//ev.skip(true);
	frame.refresh();
}

function onScroll (frame, ev) {
	std::print("scroll\n");
}

