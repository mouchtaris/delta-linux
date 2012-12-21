&using wx;
&text = "";
&{
	dll = std::dllimportdeltalib("WxWidgets.dll");

	textctrl = nil;

	onevent ButtonClicked(frame, evt) {
		text = textctrl.getvalue();
		frame.close(true);
	}

	onevent Initialize {
		local frame = frame_construct(nil, "ID_ANY", "Compile time GUI", point_construct(-1, -1), size_construct(250, 100));

		local toolbar = frame.createtoolbar();
		toolbar.addtool("ID_EXIT", "Exit");
		toolbar.hide();

		local sizer = staticboxsizer_construct(staticbox_construct(frame, "ID_ANY", "Runtime message:"), "HORIZONTAL");
		frame.setsizer(sizer);

		textctrl = textctrl_construct(frame, 4, "", point_construct(-1, -1), size_construct(150, 25));
		sizer.add(textctrl, 0, "ALIGN_");

		local okButton = button_construct(frame, "ID_OK", "Ok");
		okButton.setdefault();
		sizer.add(okButton, 0, "ALIGN_CENTER");

		frame.connect("ID_OK", "EVT_COMMAND_BUTTON_CLICKED", ButtonClicked);
		frame.refresh();
		frame.centre();
		frame.show(true);
		frame.maximize(false);
	}
	app = app_construct();
	app_oninitadd(app, Initialize);
	app_start(app);

	std::dllunimport(dll);
}

std::print(!(text));
