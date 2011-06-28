#define PP 				,@paragraph,
#define SS 				,@section,
#define NL 				,@linebreak,
#define TAB				,@tab,

#define	COMMENT			!green
#define CTAB0			!code
#define CTAB1			@tab, !code
#define CTAB2			@tab, @tab, !code
#define CTAB3			@tab, @tab, @tab, !code
#define CTAB4			@tab, @tab, @tab, @tab, !code
#define CTAB5			@tab, @tab, @tab, @tab, @tab, !code

#define _text			type "text",
#define _link			type "link",
#define _image			type "image",
#define _imagelink		type "imagelink",
#define _video			type "video",
#define _document		type "document",
#define _section		type "section",
#define _submenu		type "submenu",
#define _simpleitem 	type "simpleitem",
#define _imageitem		type "imageitem",
#define _wrap			wrap "yes",
#define _left			align "left",
#define _center			align "center",
#define _right			align "right",
#define _color(str)		color str,
#define _id(str)		itemref	str,
#define _title(str)		title str,
#define _path(str)		path str,
#define _descr(str)		descr str,
#define _url(str)		url str,
#define _ref(str)		contentref str,
#define _class(str)		class str,
#define _table(x,y)		type "table", rows x,collumns y,
