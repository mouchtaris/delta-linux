using wx;

dll = std::dllimportdeltalib(wx::DLL);

wxsize = size_construct(wx::DefaultSize);
wxpoint = point_construct(wx::DefaultPosition);
app = app_construct();

function myFunc(frame, ev) {
	msgdlg = messagedialog_construct(frame, "Are you sure you want to quit this exceptional program?",
	"Quit this program", "YES_NO");
	quit = msgdlg.showmodal();
	msgdlg.destroy();
	if (quit == flags("ID_YES")) {
		std::print("Bye bye cruel world.\n");
		frame.close(true);
	}
}

function myFunc2(frame, ev) {
	colour = frame.getbackgroundcolour();
	red = colour.red();
	green = colour.green();
	blue = colour.blue();
	colour.set((red+128)%256, (green+128)%256, (blue+128)%256);
	frame.setbackgroundcolour(colour);
	frame.refresh();
}

function create_dialog(frame)
{
	dialog = progressdialog_construct("Progress dialog example", "An informative message", 100, frame,
	flags("PD_CAN_ABORT", "PD_APP_MODAL", "PD_ELAPSED_TIME", "PD_ESTIMATED_TIME",
		"PD_REMAINING_TIME", "PD_SMOOTH")/*0x007F*/);
	time = std::currenttime();
	i = 0;
	cont = true;
	while(cont) {
		if ((std::currenttime() - time) < 50) { continue; }
		if (i < 99) {
			cont = dialog.update(i, i+"%");
		} else {
			cont = dialog.update(99, "99% (You'll wait for ever now...)");
		}
		++i;
		time = std::currenttime();
	}
	log_onlog("LOG_Status", "You must wait a little longer next time.", 0);
	dialog.destroy();
}

function create_filedialog(frame)
{
	filedlg = filedialog_construct(frame, "Testing open multiple file dialog", "", "",
	"All files (*.*)|*.*|C++ files (*.cpp;*.h)|*.cpp;*.h", flags("FD_OPEN", "FD_MULTIPLE"));
	if (filedlg.showmodal() == flags("ID_OK")) {
		paths = filedlg.getpaths();
		filenames = filedlg.getfilenames();
		count = std::tablength(paths);
		for (n = 0; n < count; ++n) {
			log_onlog("LOG_Status", "File " + n + ": " + paths[n] + " (" + filenames[n] + ")", 0);
		}
		log_onlog("LOG_Status", "Filter index: " + filedlg.getfilterindex(), 0);
	}
}

function OnWizardPageChanging(page, ev)
{
	sel = page.m_radio.getselection();
	if ( sel == 2 )
		return;
	if ( ev.getdirection() and sel == 0 )
		return;
	if ( not ev.getdirection() and sel == 1 )
		return;
	mesgdlg = messagedialog_construct(page, "You can't go there", "Not allowed",
		flags("ICON_WARNING", "OK"));
	mesgdlg.showmodal();
	ev.veto();
}

function OnWizardCancel(page, ev)
{
	mesgdlg = messagedialog_construct(page, "Do you really want to cancel?", "Question",
		flags("ICON_QUESTION", "YES_NO"));
	if (mesgdlg.showmodal() != flags("ID_YES")) {
		ev.veto();
	}
}

function radioboxpage(wizard)
{
	page = wizardpagesimple_construct(wizard);
	choices = ["forward", "backward", "both", "neither"];
	m_radio = radiobox_construct(page, "ID_ANY", "Allow to proceed:", point_construct(-1,-1),
		size_construct(-1, -1), choices, 1, "RA_SPECIFY_COLS");
	page.m_radio = m_radio;
	m_radio.setselection(2);
	mainSizer = boxsizer_construct("VERTICAL");
	mainSizer.add(m_radio, 0, "ALL", 5);
	page.setsizer(mainSizer);
	mainSizer.fit(page);
	page.connect("EVT_WIZARD_CANCEL", OnWizardCancel);
	page.connect("EVT_WIZARD_PAGE_CHANGING", OnWizardPageChanging);
	return page;
}

function oncreate_wizard(frame, ev)
{
	wizard = wizard_construct(frame, "ID_ANY", "Absolutely Useless Wizard",
		bitmap_construct("wiztest.bmp", "BITMAP_TYPE_BMP"), point_construct(-1,-1),
		flags("DEFAULT_DIALOG_STYLE", "RESIZE_BORDER"));
	page1 = wizardpagesimple_construct(wizard);
	
	page2 = radioboxpage(wizard);

	page1.setnext(page2);
	page2.setprev(page1);
	wizard.runwizard(page1);
}

function change_fontdialog(frame, ev)
{
	/*local font = font_construct(18, FONTFAMILY_DEFAULT, FONTSTYLE_NORMAL, FONTWEIGHT_NORMAL);
	local bitmap = bitmap_construct(100, 100);
	local dc = memorydc_construct(bitmap);
	dc.setbackground(brush_construct(colour_construct(255, 255, 255)));
	dc.setbrush(brush_construct(colour_construct(0, 0, 0)));
	dc.clear();
	dc.setfont(font);
	dc.drawtext("abcdefg", 0, 0);
	bitmap.savefile("fonttest.bmp", BITMAP_TYPE_BMP);
	*/
	fontdlg = fontdialog_construct(frame);
	if (fontdlg.showmodal() == flags("ID_OK")) {
		fontdata = fontdlg.getfontdata();
		font = fontdata.getchosenfont();
		textattr = textattr_construct();
		textattr.setfont(font);
		frame.textctrl.setstyle(0, frame.textctrl.getlastposition(), textattr);
		frame.textctrl.fit();
		frame.textctrl.layout();
		frame.refresh();
	}
}

function change_backgroundcolourdialog(frame, ev)
{
	coldlg = colourdialog_construct(frame);
	if (coldlg.showmodal() == flags("ID_OK")) {
		coldata = coldlg.getcolourdata();
		col = coldata.getcolour();
		textattr = textattr_construct();
		textattr.setbackgroundcolour(col);
		frame.textctrl.setstyle(0, frame.textctrl.getlastposition(), textattr);
	}
}

function change_textcolourdialog(frame, ev)
{
	coldlg = colourdialog_construct(frame);
	if (coldlg.showmodal() == flags("ID_OK")) {
		coldata = coldlg.getcolourdata();
		col = coldata.getcolour();
		textattr = textattr_construct();
		textattr.settextcolour(col);
		frame.textctrl.setstyle(0, frame.textctrl.getlastposition(), textattr);
	}
}

function onCheckboxClicked(frame, ev)
{
	log_onlog("LOG_Status", "Checkbox clicked: " + ev.ischecked(), 0);
	frame.settitle("test2");
}

function onScrollFunc(scrollbar, ev)
{	//Change the text under the scrollbar
	scrollbar.scrollbarPos.setlabel(std::tostring(ev.getposition()));
}

function create_treebook(frame, ev)
{
	frame2 = frame_construct(nil, "ID_ANY", "treebook_test");
	topSizer = boxsizer_construct("VERTICAL");
	opPanel = panel_construct(frame2);
	construct_func = treebook_construct;
	addpage_func = treebook_addpage;
	if (frame.choice.getselection() == 0) {
		construct_func = notebook_construct;
		addpage_func = notebook_addpage;
	}
	if (frame.choice.getselection() == 1) {
		construct_func = listbook_construct;
		addpage_func = listbook_addpage;
	}
	if (frame.choice.getselection() == 2) {
		construct_func = choicebook_construct;
		addpage_func = choicebook_addpage;
	}
	if (frame.choice.getselection() == 3) {
		construct_func = treebook_construct;
		addpage_func = treebook_addpage;
	}
	treebook = construct_func(opPanel, "ID_ANY");
	//Insert here
	checkBox1 = checkbox_construct(treebook, "ID_ANY", "sample_checkbox_1");
	checkBox1.setvalue(false);
	checkBox2 = checkbox_construct(treebook, "ID_ANY", "sample_checkbox_2");
	checkBox2.setvalue(false);
	
	addpage_func(treebook, checkBox1, "sample text 1");
	addpage_func(treebook, checkBox2, "sample text 2");
	//
	sizerflags = sizerflags_construct(5);
	topSizer.insert(0, treebook, sizerflags.expand().border());
	opPanel.setsizerandfit(topSizer);
	topSizer.setsizehints(frame2);
	frame2.centre();
	frame2.show(true);
}

function spinup_evtfunction(frame, ev) {
	m_ev = ev;
	m_frame = frame;
	std::print(m_ev, "\n");
}

function oninitfunc() {
	frame = frame_construct(nil, "ID_ANY", "test");
	frame.show(true);
	size = size_construct(-1,-1);
	
	frame.setscrollbar(wx::VERTICAL, 0, 10, 100, true);	//TOSEE why the look and feel of this scrollbar is correct?

	fileopen_bmp = bitmap_construct("fileopen.bmp", "BITMAP_TYPE_BMP");
	mask = mask_construct(fileopen_bmp, colour_construct(255, 255, 255));
	fileopen_bmp.setmask(mask);

//CREATE MENUS
	fileMenu = menu_construct();
	helpMenu = menu_construct();
	menuitem = menuitem_construct(helpMenu);
	menuitem.setid(ID_ABOUT);
	menuitem.setkind(ITEM_NORMAL);
	menuitem.settext("&Change Background...\tAlt-C");
	menuitem.sethelp("Change the frame background colour");
	menuitem.setbitmap(fileopen_bmp);
	helpMenu.append(menuitem);
	helpMenu.append(0, "Show Modal &Dialog\tAlt-D", "Show an empty modal dialog");
	fileMenu.append("ID_OPEN", "O&pen\tAlt-O", "Open a file dialog");
	fileMenu.append(11, "&Run wizard  modal...\tCtrl-R");
	fileMenu.append(16, "&Treebook...\tCtrl-N");
	fileMenu.append("ID_EXIT", "E&xit\tAlt-X", "Quit this program");
	menus = [fileMenu, helpMenu];
	titles = ["&File", "&Help"];
	menuBar = menubar_construct(2, menus, titles);
	
	frame.setmenubar(menuBar);

//CREATE STATUSBAR
	frame.createstatusbar();
	frame.setstatustext("test status text");
	
//CREATE TOOLBAR
	toolbar = frame.createtoolbar();
	toolbar.addtool("ID_OPEN", "Open", fileopen_bmp, "Open a file dialog");
	toolbar.addseparator();
	quit_bmp = bitmap_construct("quit.bmp", "BITMAP_TYPE_BMP");
	toolbar.addtool("ID_EXIT", "Exit", quit_bmp, "Quit this program");
	toolbar.realize();
	
//CREATE SIZER
	sizer = staticboxsizer_construct(staticbox_construct(frame, "ID_ANY", "sample static box sizer"), "VERTICAL");
	frame.setsizer(sizer);
	
//CREATE SCROLLBAR
	scrollbar = scrollbar_construct(frame, ID_ANY);
	scrollbar.setscrollbar(25, 50, 100, 5);
	sizer.add(scrollbar, 0, flags(ALIGN_CENTER, EXPAND));
	scrollbar.connect(EVT_SCROLL_CHANGED, onScrollFunc);
	
	thumbPos = scrollbar.getthumbposition();
	scrollbarPos = statictext_construct(frame, ID_ANY, std::tostring(thumbPos));
	sizer.add(scrollbarPos, 0, ALIGN_CENTER);
	scrollbar.scrollbarPos = scrollbarPos;
	
//CREATE BUTTON & ADD IT TO SIZER
	staticbitmap = staticbitmap_construct(frame, ID_ANY, fileopen_bmp);
	sizer.add(staticbitmap, 0, wx::ALIGN_CENTER);
	bitmapbutton = bitmapbutton_construct(frame, ID_ANY, fileopen_bmp);
	sizer.add(bitmapbutton, 0, wx::ALIGN_CENTER);
	okButton = button_construct(frame, "ID_OK", "Quit");
	okButton.setdefault();
	sizer.add(okButton, 0, "ALIGN_CENTER");
	fontdlgbtn = button_construct(frame, 12, "Change Font");
	fontdlgbtn.setdefault();
	sizer.add(fontdlgbtn, 0, "ALIGN_CENTER");
	bcoldlgbtn = button_construct(frame, 13, "Change Background Colour");
	bcoldlgbtn.setdefault();
	sizer.add(bcoldlgbtn, 0, "ALIGN_CENTER");
	tcoldlgbtn = button_construct(frame, 14, "Change Text Colour");
	tcoldlgbtn.setdefault();
	sizer.add(tcoldlgbtn, 0, "ALIGN_CENTER");
	sizer.add(statictext_construct(frame, "ID_ANY", "static text sample"), 0, "ALIGN_CENTER");

//CREATE CHECKBOX & ADD IT TO SIZER
	chkbox = checkbox_construct(frame, 15, "Sample checkbox");
	sizer.add(chkbox, 0, "ALIGN_CENTER");
	
//CREATE GAUGE & ADD IT TO SIZER
	gauge = gauge_construct(frame, "ID_ANY", 100);
	sizer.add(gauge, 0, "ALIGN_CENTER");
	gauge.setvalue(49);

//CREATE TEXTCTRL & ADD IT TO SIZER
	textctrl_id = 4;
	textctrl = textctrl_construct(frame, textctrl_id, "Test value", point_construct(-1,-1),
	size_construct(-1,-1), "TE_RICH2");
	frame.textctrl = textctrl;
	sizer.add(textctrl, 0, "ALIGN_CENTER");

//CREATE CHOICE AND CHECKLISTBOX & ADD IT TO SIZER
	choices = ["notebook", "listbook", "choicebook", "treebook"];
	choice = choice_construct(frame, "ID_ANY", point_construct(-1,-1), size_construct(-1,-1), choices);
	choice.setselection(1);
	frame.choice = choice;
	sizer.add(choice, 0, "ALIGN_CENTER");
	listbox = checklistbox_construct(frame, "ID_ANY", point_construct(-1,-1), size_construct(-1,-1), choices);
	choice.setselection(0);
	sizer.add(listbox, 0, "ALIGN_CENTER");
	
//CREATE CALENDARCTRL & ADD IT TO SIZER
	calctrl = calendarctrl_construct(frame, "ID_ANY", datetime_construct(), point_construct(-1,-1),
	size_construct(-1,-1), "CAL_SEQUENTIAL_MONTH_SELECTION");
	sizer.add(calctrl, 0, "ALIGN_CENTER");
	
//CREATE DATEPICKERCTRL & ADD IT TO SIZER
	datectrl = datepickerctrl_construct(frame, "ID_ANY");
	sizer.add(datectrl, 0, "ALIGN_CENTER");
	
//CREATE GENERICDIRCTRL & ADD IT TO SIZER
	dirctrl = genericdirctrl_construct(frame);
	sizer.add(dirctrl, 0, "ALIGN_CENTER");
	
//CREATE SLIDER & ADD IT TO SIZER
	slider = slider_construct(frame, "ID_ANY", 12, 0, 100);
	sizer.add(slider, 0, "ALIGN_CENTER");
	
//CREATE TOGGLE BUTTON & ADD IT TO SIZER
	tglbtn = togglebutton_construct(frame, "ID_ANY", "toggle button");
	sizer.add(tglbtn, 0, "ALIGN_CENTER");

//CREATE SPIN BUTTON BUTTON & ADD IT TO SIZER
	spinbtn = spinbutton_construct(frame, 17);
	sizer.add(spinbtn, 0, "ALIGN_CENTER");
	
//CREATE LOG WINDOW
	logwin = logwindow_construct(frame, "My Log Window");
	log_onlog("LOG_Status", "test", 0);
	
	frame.connect("ID_EXIT", "EVT_COMMAND_MENU_SELECTED", myFunc);
	frame.connect("ID_ABOUT", "EVT_COMMAND_MENU_SELECTED", myFunc2);
	frame.connect(0, "EVT_COMMAND_MENU_SELECTED", create_dialog);
	frame.connect("ID_OK", "EVT_COMMAND_BUTTON_CLICKED", myFunc);
	frame.connect(10, "EVT_COMMAND_BUTTON_CLICKED", myFunc);
	frame.connect("ID_OPEN", "EVT_COMMAND_MENU_SELECTED", create_filedialog);
	frame.connect(11, "EVT_COMMAND_MENU_SELECTED", oncreate_wizard);
	frame.connect(12, "EVT_COMMAND_BUTTON_CLICKED", change_fontdialog);
	frame.connect(13, "EVT_COMMAND_BUTTON_CLICKED", change_backgroundcolourdialog);
	frame.connect(14, "EVT_COMMAND_BUTTON_CLICKED", change_textcolourdialog);
	frame.connect(15, "EVT_COMMAND_CHECKBOX_CLICKED", onCheckboxClicked);
	frame.connect(16, "EVT_COMMAND_MENU_SELECTED", create_treebook);
	frame.connect(17, "EVT_SPIN_UP", spinup_evtfunction);

	frame.fit();
	frame.refresh();
	frame.centre();
}

app_oninitadd(app, oninitfunc);
app_start(app);

std::dllunimportdeltalib(dll);

