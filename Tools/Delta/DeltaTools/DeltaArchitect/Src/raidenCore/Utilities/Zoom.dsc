using #ShapeRAD;



function onZoomIn (frame, ev) {
	currProject = frame.currProject;
	if (std::isnil(currProject))
		return;
	currPattern = currProject.currentPattern;
	if (std::isnil(currPattern) or std::isnil(currPattern.window))
		return;
	//
	scrolledwindow			= currPattern.window;
	scrolledwindow.scale   += 0.05;
	scrolledwindow.refresh();
}


function onZoonOut (frame, ev) {
	currProject = frame.currProject;
	if (std::isnil(currProject))
		return;
	currPattern = currProject.currentPattern;
	if (std::isnil(currPattern) or std::isnil(currPattern.window))
		return;
	//
	scrolledwindow			= currPattern.window;
	scrolledwindow.scale   -= 0.05;
	scrolledwindow.refresh();
}


function onNormalizeZoom (frame, ev) {
	currProject = frame.currProject;
	if (std::isnil(currProject))
		return;
	currPattern = currProject.currentPattern;
	if (std::isnil(currPattern) or std::isnil(currPattern.window))
		return;
	// not already normal size
	if (currPattern.window.scale!=1) {
		scrolledwindow			= currPattern.window;
		scrolledwindow.scale	= 1;
		scrolledwindow.refresh();
	}
}

