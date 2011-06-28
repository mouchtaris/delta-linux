// DebugWatchValueXmlEncoder.cpp
// Xml format converter for debug watch values.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#include "DebugWatchValueXmlEncoder.h"
#include "DebugWatchValueXmlEncodingDefs.h"
#include "DebugWatchValueTextEncoders.h"
#include "ustrings.h"
#include "ulexutil.h"
#include "DDebug.h"

////////////////////////////////////////////////////////////////

#define	XS(s)		std::string(s)
#define XS_SP		XS(" ")
#define XS_LINE		XS("\n")
#define	XS_IS		XS("=")
#define	XS_CHOICE(c,s1,s2) ((c)? (s1) : (s2))
#define	QUOTED(s)	(XS("\"") + (s) + XS("\""))

#define	TABS			stabs
#define	INCTABS()		stabs = utabstops(++tabs)
#define	DECTABS()		stabs = utabstops(--tabs)

#define	XS_LT		XS("<")
#define	XS_ELT		XS("</")			
#define	XS_EGT		XS("/>")			
#define	XS_GT		XS(">")
#define	XS_EMPTY	""

#define	START_COMP_ELEM(id, attrs) \
	text.append(TABS + XS_LT + id + attrs + XS_GT + XS_LINE); INCTABS()

#define	END_COMP_ELEM(id) \
	text.append(TABS + XS_ELT + id + XS_GT + XS_LINE); DECTABS()

#define	SIMPLE_ELEM(id, attrs, content) \
	text.append(TABS + XS_LT + id + attrs + XS_GT + content + XS_ELT + id + XS_GT + XS_LINE);

#define	ATTR(id, value) \
	XS_SP + XS(id) + XS_IS + QUOTED(value)

#define	XML_STRING(s) \
		uunparseescapesequences(ucstringarg(s), textChars, xmlChars)

////////////////////////////////////////////////////////////////

static char textChars[] = {
	'&' ,
	'<' ,
	'>'	,
	'\'',
	'"'
};

static const char* xmlChars[] = {
	"&amp;"	, 
	"&lt;"	,
	"&gt;"	,
	"&apos;",
	"&quot;"
};

////////////////////////////////////////////////////////////////

const std::string DebugWatchValueXmlEncoder::Encode (const DebugWatchValueInfo& value) {

	std::string text, stabs;
	util_ui32	tabs = 0;

	if (value.GetType() == DebugWatchValueInfo::SimpleType) {
		SIMPLE_ELEM(
			XML_ELEM_VALUE, 
			ATTR(XML_ATTR_TYPE, XML_KWD_SIMPLE) + ATTR(XML_ATTR_VALUE, XML_STRING(value.GetSimple())),
			XS_EMPTY
		);
		return text;
	}
	else
		DASSERT(value.GetType() == DebugWatchValueInfo::CompositeType);

	const DebugWatchValueInfo::Composite& comp = value.GetComposite();
	
	START_COMP_ELEM(XML_ELEM_VALUE, ATTR(XML_ATTR_TYPE, XML_KWD_COMPOSITE));

	START_COMP_ELEM(XML_ELEM_CONTENTS, XS_EMPTY);
	SIMPLE_ELEM(XML_ELEM_OVERVIEW, XS_EMPTY, XML_STRING(comp.GetOverview()));
	SIMPLE_ELEM(XML_ELEM_ABSOLUTEREF, XS_EMPTY, XML_STRING(comp.GetAbsoluteReference()));

	const std::list<DebugWatchValueInfo::Content>& contents = comp.GetContents();
	
	for (	std::list<DebugWatchValueInfo::Content>::const_iterator i = contents.begin();
			i != contents.end();
			++i		) {

		START_COMP_ELEM(XML_ELEM_CONTENT, ATTR(XML_ATTR_SUBINDEX, XML_STRING(i->GetSubIndex())));
		
		SIMPLE_ELEM(
			XML_ELEM_DISPLAYDESC, 
			ATTR(XML_ATTR_TYPE, XS_CHOICE(i->HasDisplayDesc(), XML_KWD_DESC, XML_KWD_NODESC)),
			XML_STRING(i->GetDisplayDesc())
		);
		
		SIMPLE_ELEM(
			XML_ELEM_KEYTYPEINFO,
			ATTR(XML_ATTR_TYPE, XS_CHOICE(i->HasKeyType(), XML_KWD_KEYTYPE, XML_KWD_NOKEYTYPE)),
			XML_STRING(i->GetKeyType())
		);

		SIMPLE_ELEM(
			XML_ELEM_KEYACCESS, 
			XS_EMPTY, 
			XS_CHOICE(i->IsKeyVisible(), XML_KWD_VISIBLE, XML_KWD_HIDDEN)
		);

		START_COMP_ELEM(XML_ELEM_FIELDKEYS, XS_EMPTY);

		typedef std::list<DebugWatchValueInfo::FieldKey> FieldKeys;
		const FieldKeys& keys = i->GetFieldKeys();
		for (FieldKeys::const_iterator j = keys.begin(); j != keys.end(); ++j) {

			START_COMP_ELEM(XML_ELEM_FIELDKEY, XS_EMPTY);

			SIMPLE_ELEM(
				XML_ELEM_KEYCONTENTREF,
				ATTR(XML_ATTR_TYPE, XS_CHOICE(j->HasContentReference(), XML_KWD_KEYREF, XML_KWD_NOKEYREF)),
				XML_STRING(j->GetContentReference())
			);

			SIMPLE_ELEM(
				XML_ELEM_DISPLAYEDKEY, 
				XS_EMPTY, 
				XML_STRING(j->GetDisplayedText())
			);

			END_COMP_ELEM(XML_ELEM_FIELDKEY);
		}
		END_COMP_ELEM(XML_ELEM_FIELDKEYS);
		END_COMP_ELEM(XML_ELEM_CONTENT);
	}

	END_COMP_ELEM(XML_ELEM_CONTENTS);
	END_COMP_ELEM(XML_ELEM_VALUE);

	return text;
}

////////////////////////////////////////////////////////////////
