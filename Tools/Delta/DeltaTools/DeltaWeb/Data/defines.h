///////////////////////////////////////////////////////////////////
// Some utility macros that make easier to define
// the site content.
// A. Savidis, November 2009.
///////////////////////////////////////////////////////////////////

#define PP 				,@paragraph,
#define SS 				,@section,
#define NL 				,@linebreak,
#define TAB				,@tab,
#define	COMMA			,

#define _text			type "text",
#define _list			type "list",
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
#define _table(x,y)		type "table", rows x, collumns y,

#define	COMMENT(s)		!comment s, NL1
#define	ERROR(s)		!error s, NL1
#define	NOTE(s)			!note s, NL1

///////////////////////////////////////////////////////////////////
// Various tab stops

#define TAB1			@tab, 
#define TAB2			@tab, @tab, 
#define TAB3			@tab, @tab, @tab, 
#define TAB4			@tab, @tab, @tab, @tab, 
#define TAB5			@tab, @tab, @tab, @tab, @tab,

#define CTAB0			!dsrc
#define CTAB1			TAB1 !dsrc
#define CTAB2			TAB2 !dsrc
#define CTAB3			TAB3 !dsrc
#define CTAB4			TAB4 !dsrc
#define CTAB5			TAB5 !dsrc

///////////////////////////////////////////////////////////////////
// Various line breaks

#define	NL1				@linebreak
#define	NL2				NL1, NL1
#define	NL3				NL2, @linebreak

#define	CNL1			NL1,
#define	CNL2			NL2,
#define	CNL3			NL3,

#define	NLC1			,NL1
#define	NLC2			,NL2
#define	NLC3			,NL3

#define	SL1				[ _text [ @linebreak ] ]
#define	SL2				[ _text [ NL1, NL1 ] ]
#define	SL3				[ _text [ NL2, @linebreak ] ]

///////////////////////////////////////////////////////////////////
// Document and section markers

#define	START_DOC(i,t)	[ _document _id(i) title t
#define	END_DOC			]

#define	START_SEC(i,t)	[_section _id(i) _title(t)
#define	END_SEC			]

#define	DOC_REF(r,t)	[ _link _ref(r) text t ]
#define	SEC_REF(r,t)	[ _link sectionref r, text t ]

///////////////////////////////////////////////////////////////////
// Indepednent figures, text blocks and links 

#define	FIGURE(p,a,d)	[ type "image", path p, wrap "no", align #a, descr d ]
#define	TEXT(t)			[ _text [ t ] ]
#define	TEXT_START		[ _text [
#define	TEXT_END		] ]
#define	LIST_START		[ _list
#define	LIST_END		]
#define	LINK(url, t)	[_link _url(url) text  t ]
#define	LINK_IN_TEXT(url, t)	TEXT_END, LINK(url, t), TEXT_START

///////////////////////////////////////////////////////////////////
// Code fragments

#define	CODE_BLOCK		!dsrc 	!program_code
#define	CPP_BLOCK		!cpp 	!program_code

///////////////////////////////////////////////////////////////////
