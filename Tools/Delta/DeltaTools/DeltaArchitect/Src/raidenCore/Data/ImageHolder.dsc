

const path = "Bitmaps\\";
//
const toolbar   = ::path + "Toolbar\\";
const menuToolbar = ::path + "MenuToolbar\\";
const mouse     = ::path + "Mouse\\";
const startpage = ::path + "Startpage\\";
const other     = ::path + "Other\\";
const treeview  = ::path + "Treeview\\";


/**
 *  Description: Holder of all images that Raiden uses
 **/
function images {
	if (std::isundefined(static imagedata)) {
		// icon of raiden
		icon       = wx::icon_construct(::other + "icon.bmp",wx::BITMAP_TYPE_ICO);
		iconbitmap = wx::bitmap_construct(wx::image_construct(::other + "icon.bmp"));
		// main page buttons
		buttonNewProj          = wx::bitmap_construct(image = wx::image_construct(::startpage + "buttonNewProj.bmp"));
		buttonNewProjSelected  = wx::bitmap_construct( wx::image_construct(::startpage + "buttonNewProjSelected.bmp") );
		buttonOpenProj         = wx::bitmap_construct( wx::image_construct(::startpage + "buttonOpenProj.bmp") );
		buttonOpenProjSelected = wx::bitmap_construct( wx::image_construct(::startpage + "buttonOpenProjSelected.bmp"));
		help = wx::bitmap_construct(image = wx::image_construct(::startpage + "help.bmp"));
		mask = wx::mask_construct(help, colour = wx::colour_construct("#FF00FF"));
		help.setmask(mask);
		mask = nil;
		filter = wx::bitmap_construct(image = wx::image_construct(::startpage + "filter.bmp"));
		mask = wx::mask_construct(filter, colour = wx::colour_construct("#FF00FF"));
		filter.setmask(mask);
		mask = nil;
		//
		toolbarArrow = wx::bitmap_construct(image = wx::image_construct(::toolbar + "-.bmp") );
		mask = wx::mask_construct(toolbarArrow, colour = wx::colour_construct("#FF00FF"));
		toolbarArrow.setmask(mask);
		mask = nil;
		// add shape
		addElem = wx::image_construct(::mouse + "addConstituent.bmp");
		addElem.setmaskcolour(255,0,255);
		// add use
		addUseSegment1st = wx::image_construct(::mouse + "arrow1_use.bmp");
		addUseSegment1st.setmaskcolour(255,0,255);
		addUseSegment2nd = wx::image_construct(::mouse + "arrow2_use.bmp");
		addUseSegment2nd.setmaskcolour(255,0,255);
		// add offer
		addOfferSegment1st = wx::image_construct(::mouse + "arrow1_offer.bmp");
		addOfferSegment1st.setmaskcolour(255,0,255);
		addOfferSegment2nd = wx::image_construct(::mouse + "arrow2_offer.bmp");
		addOfferSegment2nd.setmaskcolour(255,0,255);
		// add conn & addr
		addAddressSegment1st = wx::image_construct(::mouse + "arrow1_address.bmp");
		addAddressSegment1st.setmaskcolour(255,0,255);
		addAddressSegment2nd = wx::image_construct(::mouse + "arrow2_address.bmp");
		addAddressSegment2nd.setmaskcolour(255,0,255);
		// Tools
		toolRequirement = wx::bitmap_construct( wx::image_construct(::toolbar + "Requirement.bmp") );
		mask = wx::mask_construct(toolRequirement, colour = wx::colour_construct("#FF00FF"));
		toolRequirement.setmask(mask);
		mask = nil;
		toolOperation = wx::bitmap_construct( wx::image_construct(::toolbar + "Operation.bmp") );
		mask = wx::mask_construct(toolOperation, colour = wx::colour_construct("#FF00FF"));
		toolOperation.setmask(mask);
		mask = nil;
		toolRole = wx::bitmap_construct( wx::image_construct(::toolbar + "Role.bmp") );
		mask = wx::mask_construct(toolRole, colour = wx::colour_construct("#FF00FF"));
		toolRole.setmask(mask);
		mask = nil;
		toolInput   = wx::bitmap_construct( wx::image_construct(::toolbar + "io.bmp") );
		mask = wx::mask_construct(toolInput, colour = wx::colour_construct("#FF00FF"));
		toolInput.setmask(mask);
		mask = nil;
		toolOutput  = wx::bitmap_construct( wx::image_construct(::toolbar + "io.bmp") );
		mask = wx::mask_construct(toolOutput, colour = wx::colour_construct("#FF00FF"));
		toolOutput.setmask(mask);
		mask = nil;
		toolAddress = wx::bitmap_construct( wx::image_construct(::toolbar + "Address.bmp") );
		mask = wx::mask_construct(toolAddress, colour = wx::colour_construct("#FF00FF"));
		toolAddress.setmask(mask);
		mask = nil;
		toolUse = wx::bitmap_construct( wx::image_construct(::toolbar + "Use.bmp") );
		mask = wx::mask_construct(toolUse, colour = wx::colour_construct("#FF00FF"));
		toolUse.setmask(mask);
		mask = nil;
		toolOffer = wx::bitmap_construct( wx::image_construct(::toolbar + "Offer.bmp") );
		mask = wx::mask_construct(toolOffer, colour = wx::colour_construct("#FF00FF"));
		toolOffer.setmask(mask);
		mask = nil;
		//
		offer = wx::bitmap_construct( wx::image_construct(::other + "O.bmp") );
		mask = wx::mask_construct(offer, colour = wx::colour_construct("#FF00FF"));
		offer.setmask(mask);
		mask = nil;
		use = wx::bitmap_construct( wx::image_construct(::other + "U.bmp") );
		mask = wx::mask_construct(use, colour = wx::colour_construct("#FF00FF"));
		use.setmask(mask);
		mask = nil;
		address = wx::bitmap_construct( wx::image_construct(::other + "A.bmp") );
		mask = wx::mask_construct(address, colour = wx::colour_construct("#FF00FF"));
		address.setmask(mask);
		mask = nil;
		partof = wx::bitmap_construct( wx::image_construct(::other + "I.bmp") );
		mask = wx::mask_construct(partof, colour = wx::colour_construct("#FF00FF"));
		partof.setmask(mask);
		mask = nil;
		//
		imageListNB = wx::imagelist_construct(16, 16, true, 1);
		// pin the list names data images
		tactive = wx::bitmap_construct(wx::image_construct(::treeview + "active.bmp" ));
		mask = wx::mask_construct(tactive, colour = wx::colour_construct("#FF00FF"));
		tactive.setmask(mask);
		mask = nil;
		imageListNB.Active  = imageListNB.add(tactive);
		tproject = wx::bitmap_construct(wx::image_construct(::treeview + "project.bmp"));
		mask = wx::mask_construct(tproject, colour = wx::colour_construct("#FF00FF"));
		tproject.setmask(mask);
		mask = nil;
		imageListNB.Project = imageListNB.add(tproject);
		tfolder = wx::bitmap_construct(wx::image_construct(::treeview + "folder.bmp" ));
		mask = wx::mask_construct(tfolder, colour = wx::colour_construct("#FF00FF"));
		tfolder.setmask(mask);
		mask = nil;
		imageListNB.Folder  = imageListNB.add(tfolder);
		tpattern = wx::bitmap_construct(wx::image_construct(::treeview + "pattern.bmp"));
		mask = wx::mask_construct(tpattern, colour = wx::colour_construct("#FF00FF"));
		tpattern.setmask(mask);
		mask = nil;
		imageListNB.Pattern = imageListNB.add(tpattern);
		tfilter = wx::bitmap_construct(wx::image_construct(::treeview + "filter.bmp" ));
		mask = wx::mask_construct(tfilter, colour = wx::colour_construct("#FF00FF"));
		tfilter.setmask(mask);
		mask = nil;
		imageListNB.Filter = imageListNB.add(tfilter);
		trole = wx::bitmap_construct(wx::image_construct(::treeview + "role.bmp"   ));
		mask = wx::mask_construct(trole, colour = wx::colour_construct("#FF00FF"));
		trole.setmask(mask);
		mask = nil;
		imageListNB.Role = imageListNB.add(trole);
		//
		imageListNB2 = wx::imagelist_construct(12, 15, true, 1);
		imageListNB2.properties = imageListNB2.add(wx::bitmap_construct(wx::image_construct(::other + "properties.bmp")));
		imageListNB2.notes      = imageListNB2.add(wx::bitmap_construct(wx::image_construct(::other + "notes.bmp")));
		//
		quit = wx::bitmap_construct(wx::image_construct(::menuToolbar + "quit.bmp"));
		mask = wx::mask_construct(quit, colour = wx::colour_construct("#FF00FF"));
		quit.setmask(mask);
		mask = nil;
		newproject = wx::bitmap_construct(wx::image_construct(::menuToolbar + "newproject.bmp"));
		mask = wx::mask_construct(newproject, colour = wx::colour_construct("#FF00FF"));
		newproject.setmask(mask);
		mask = nil;
		openproject = wx::bitmap_construct(wx::image_construct(::menuToolbar + "openproject.bmp"));
		mask = wx::mask_construct(openproject, colour = wx::colour_construct("#FF00FF"));
		openproject.setmask(mask);
		mask = nil;
		undo = wx::bitmap_construct(wx::image_construct(::menuToolbar + "undo.bmp"));
		mask = wx::mask_construct(undo, colour = wx::colour_construct("#FF00FF"));
		undo.setmask(mask);
		mask = nil;
		redo = wx::bitmap_construct(wx::image_construct(::menuToolbar + "redo.bmp"));
		mask = wx::mask_construct(redo, colour = wx::colour_construct("#FF00FF"));
		redo.setmask(mask);
		mask = nil;
		save = wx::bitmap_construct(wx::image_construct(::menuToolbar + "save.bmp"));
		mask = wx::mask_construct(save, colour = wx::colour_construct("#FF00FF"));
		save.setmask(mask);
		mask = nil;
		saveall = wx::bitmap_construct(wx::image_construct(::menuToolbar + "saveall.bmp"));
		mask = wx::mask_construct(saveall, colour = wx::colour_construct("#FF00FF"));
		saveall.setmask(mask);
		mask = nil;
		remove = wx::bitmap_construct(wx::image_construct(::menuToolbar + "remove.bmp"));
		mask = wx::mask_construct(remove, colour = wx::colour_construct("#FF00FF"));
		remove.setmask(mask);
		mask = nil;
		export = wx::bitmap_construct(wx::image_construct(::menuToolbar + "export.bmp"));
		mask = wx::mask_construct(export, colour = wx::colour_construct("#FF00FF"));
		export.setmask(mask);
		mask = nil;
		run = wx::bitmap_construct(wx::image_construct(::menuToolbar + "run.bmp"));
		mask = wx::mask_construct(run, colour = wx::colour_construct("#FF00FF"));
		run.setmask(mask);
		mask = nil;
		props = wx::bitmap_construct(wx::image_construct(::menuToolbar + "props.bmp"));
		mask = wx::mask_construct(props, colour = wx::colour_construct("#FF00FF"));
		props.setmask(mask);
		mask = nil;
		zoomin = wx::bitmap_construct(wx::image_construct(::menuToolbar + "zoomin.bmp"));
		mask   = wx::mask_construct(zoomin, colour = wx::colour_construct("#FF00FF"));
		zoomin.setmask(mask);
		mask = nil;
		zoomout = wx::bitmap_construct(wx::image_construct(::menuToolbar + "zoomout.bmp"));
		mask    = wx::mask_construct(zoomout, colour = wx::colour_construct("#FF00FF"));
		zoomout.setmask(mask);
		mask = nil;
		zoomfit = wx::bitmap_construct(wx::image_construct(::menuToolbar + "zoomfit.bmp"));
		mask    = wx::mask_construct(zoomfit, colour = wx::colour_construct("#FF00FF"));
		zoomfit.setmask(mask);
		mask = nil;
		resizehorizontally = wx::bitmap_construct(wx::image_construct(::menuToolbar + "resizehorizontal.bmp"));
		mask    = wx::mask_construct(resizehorizontally, colour = wx::colour_construct(255,255,255));
		resizehorizontally.setmask(mask);
		mask = nil;
		resizevertically   = wx::bitmap_construct(wx::image_construct(::menuToolbar + "resizevertical.bmp"));
		mask    = wx::mask_construct(resizevertically, colour = wx::colour_construct(255,255,255));
		resizevertically.setmask(mask);
		mask = nil;
		
		//
		imagedata = [
			@raidenIcon				: icon,
			@raidenIconBMP			: iconbitmap,
			//
			@buttonNewProj			: buttonNewProj,
			@buttonNewProjSelected	: buttonNewProjSelected,
			@buttonOpenProj			: buttonOpenProj,
			@buttonOpenProjSelected : buttonOpenProjSelected,
			//
			@toolbarArrow			: toolbarArrow,
			//
			@addElement				: addElem,
			//
			@addUseSegment1st		: addUseSegment1st,
			@addUseSegment2nd		: addUseSegment2nd,
			//
			@addOfferSegment1st		: addOfferSegment1st,
			@addOfferSegment2nd		: addOfferSegment2nd,
			//
			@addPartOFSegment1st	: addOfferSegment1st,
			@addPartOFSegment2nd	: addOfferSegment2nd,
			//
			@addAddressSegment1st	: addAddressSegment1st,
			@addAddressSegment2nd	: addAddressSegment2nd,
			//
			@toolRequirement		: toolRequirement,
			@toolOperation			: toolOperation,
			@toolRole				: toolRole,
			@toolInput				: toolInput,
			@toolOutput				: toolOutput,
			@toolAddress			: toolAddress,
			@toolUse				: toolUse,
			@toolOffer				: toolOffer,
			@toolPartOF				: toolOffer,
			//
			@help					: help,
			@filter					: filter,
			@quit					: quit,
			@newproject				: newproject,
			@openproject			: openproject,
			@undo					: undo,
			@redo					: redo,
			@remove					: remove,
			@save					: save,
			@saveall				: saveall,
			@export					: export,
			@run					: run,
			@properties				: props,
			@zoomin					: zoomin,
			@zoomout				: zoomout,
			@zoomfit				: zoomfit,
			@resizehorizontally		: resizehorizontally,
			@resizevertically		: resizevertically,
			//
			@offer					: offer,
			@use					: use,
			@address				: address,
			@partof					: partof,
			// list images
			@imageListNB			: imageListNB,
			@imageListNB2			: imageListNB2
		];
	}
	return imagedata;
}
