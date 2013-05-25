using wx;

dll = std::dllimportdeltalib(wx::DLL);

app = app_construct();

function oninitfunc() {
	frame = frame_construct(nil, "ID_ANY", "test");
	frame.show(true);
	boxsizer = boxsizer_construct(VERTICAL);
	frame.setsizer(boxsizer);
	sizer = gridbagsizer_construct();
	boxsizer.add(sizer, 1, EXPAND, 5);
//	frame.setsizer(sizer);
	gbspan = gbspan_construct(1, 1);
	row = 0;
	column = 0;
	rows = 5;
	columns = 3;
	sizer.setrows(rows);
	sizer.setcols(columns);
	for (i = 0; i < rows; ++i)
		sizer.addgrowablerow(i, 1.0);
	for (i = 0; i < columns; ++i)
		sizer.addgrowablecol(i, 1.0);
	for (i = 0; i < rows * columns; ++i) {
		button = button_construct(frame, ID_ANY, "Button " + i);
		button.setdefault();
		gbposition = gbposition_construct(row, column);
		sizer.add(button, gbposition, gbspan, EXPAND);
		if (++column == columns) {
			column = 0;
			++row;
		}
	}
	sizer.calcmin();
	sizer.recalcsizes();

	frame.fit();
	frame.refresh();
	frame.centre();
}

app_oninitadd(app, oninitfunc);
app_start(app);

std::dllunimportdeltalib(dll);

