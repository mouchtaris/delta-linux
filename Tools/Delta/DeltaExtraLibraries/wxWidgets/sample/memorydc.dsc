using wx;

dll = std::dllimportdeltalib(wx::DLL);

app = app_construct();

function onPaint(frame, e)
{
	local dc		= bufferedpaintdc_construct(frame);
	dc.clear();
	dc.drawtext("Hello world", 0, 0);
	dc.drawcircle(100, 100, 50);
	dc.drawellipse(200, 200, 100, 400);
	dc.drawbitmap(frame.bitmap, 100, 100);
}

function oninitfunc() {
	frame = frame_construct(nil, "ID_ANY", "test");
	frame.show(true);

	frame.setsize(800, 600);
	frame.centre();
	
	local bitmap	= bitmap_construct(1000, 1000);
	local memdc		= memorydc_construct(bitmap);
	memdc.clear();
	local pen		= pen_construct(colour_construct(255, 0, 0));
	local brush		= brush_construct(colour_construct(0, 0, 255));
	memdc.setpen(pen);
	memdc.setbrush(brush);
	memdc.drawrectangle(0, 0, 100, 200);
	memdc.drawcircle(300, 300, 50);
	local image		= bitmap.converttoimage();
	frame.bitmap	= bitmap_construct(image);
	
	frame.connect(wx::EVT_PAINT, onPaint);
	frame.refresh();
}

app_oninitadd(app, oninitfunc);
app_start(app);

std::dllunimportdeltalib(dll);

