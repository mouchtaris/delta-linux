// DebugWatchValueRcEncoder.cpp
// Resource format converter for debug watch values.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#include "DebugWatchValueRcEncoder.h"
#include "DebugWatchValueRcEncodingDefs.h"
#include "DebugWatchValueTextEncoders.h"
#include "ustrings.h"
#include "ulexutil.h"
#include "DDebug.h"

////////////////////////////////////////////////////////////////

#define	RC(s)		std::string(s)
#define RC_SPACE	RC(" ")
#define RC_LINE		RC("\n")
#define	RC_COMMA	RC(",")
#define	RC_LB		RC("[")
#define	RC_RB		RC("]")
#define	RC_CHOICE(c,s1,s2) ((c)? QUOTED(s1) : QUOTED(s2))
#define	QUOTED(s)	(RC("\"") + (s) + RC("\""))

#define	GROUP_ITEM_DONE(k) \
	DECTABS(); \
	text.append(RC_LINE + TABS + RC_RB + ((k) ? (RC_COMMA+RC_LINE) : ""));

#define	TABS			stabs
#define	INCTABS()		stabs = utabstops(++tabs)
#define	DECTABS()		stabs = utabstops(--tabs)
#define	GROUP_OPEN()	text.append(TABS + RC_LB + RC_LINE); INCTABS()
#define	GROUP_CLOSE()	DECTABS(); text.append(RC_LINE + TABS + RC_RB);
#define	GROUP_ID(id)	\
		text.append(TABS + id + RC_SPACE + RC_LB + RC_LINE); INCTABS()

#define	TUPPLE_ID(id)	text.append(TABS + id + RC_SPACE)
#define	ADD_COMMA()		text.append(RC_COMMA + RC_LINE)

#define	RC_STRING(s) \
		uunparseescapesequences(ucstringarg(s))

////////////////////////////////////////////////////////////////

static void WriteTupple (
		std::string&		text,
		bool				cond, 
		const std::string&	firstChoice, 
		const std::string&	secondChoice, 
		bool				hasValue,
		const std::string&	value = ""
	) {
	text.append(RC_LB + RC(RC_SYM_TYPE) + RC_SPACE + RC_CHOICE(cond, firstChoice, secondChoice));
	if (cond && hasValue)
		text.append(RC_COMMA + RC_SPACE + RC_SYM_VALUE + RC_SPACE + QUOTED(value));
	text.append(RC_RB);
}

////////////////////////////////////////////////////////////////

#define	WriteValue(kwd, val) \
	text.append(TABS + RC(kwd) + RC_SPACE + QUOTED(val))

#define	WriteNumber(kwd, num) \
	text.append(TABS + RC(kwd) + RC_SPACE + uconstructstr("%d", num))

////////////////////////////////////////////////////////////////

const std::string DebugWatchValueRcEncoder::Encode (const DebugWatchValueInfo& value) {

	std::string text(RC_SYM_TYPE + RC_SPACE);
	if (value.GetType() == DebugWatchValueInfo::SimpleType)
		return text + QUOTED(RC_KWD_SIMPLE) + RC_LINE + RC_SYM_VALUE + RC_SPACE + QUOTED(RC_STRING(value.GetSimple()));
	else
		DASSERT(value.GetType() == DebugWatchValueInfo::CompositeType);

	text.append(QUOTED(RC_KWD_COMPOSITE) + RC_LINE);	
	util_ui32	tabs = 0;
	std::string stabs;

	GROUP_ID(RC_SYM_VALUE);

	GROUP_ID(RC_SYM_CONTENTS);
	const DebugWatchValueInfo::Composite& comp = value.GetComposite();

	WriteValue(RC_SYM_OVERVIEW, RC_STRING(comp.GetOverview()));	
	ADD_COMMA();	

	WriteValue(RC_SYM_ABSOLUTEREF, RC_STRING(comp.GetAbsoluteReference()));	
	ADD_COMMA();	

	const std::list<DebugWatchValueInfo::Content>& contents = comp.GetContents();
	util_ui32 n = (util_ui32) contents.size();
	
	WriteNumber(RC_SYM_SIZE, n);
	if (n) ADD_COMMA();

	for (	std::list<DebugWatchValueInfo::Content>::const_iterator i = contents.begin();
			i != contents.end();
			++i		) {

		--n;
		GROUP_OPEN();

		WriteValue(RC_SYM_SUBINDEX, RC_STRING(i->GetSubIndex()));
		ADD_COMMA();	

		TUPPLE_ID(RC_SYM_DISPLAYDESC);
		WriteTupple(
			text, 
			i->HasDisplayDesc(), RC_KWD_DESC, RC_KWD_NODESC, 
			true, 
			RC_STRING(i->GetDisplayDesc())
		);
		ADD_COMMA();

		TUPPLE_ID(RC_SYM_KEYTYPEINFO);
		WriteTupple(
			text, 
			i->HasKeyType(), RC_KWD_KEYTYPE, RC_KWD_NOKEYTYPE, 
			true, 
			RC_STRING(i->GetKeyType())
		);
		ADD_COMMA();

		WriteValue(RC_SYM_KEYACCESS, i->IsKeyVisible() ? RC_KWD_VISIBLE : RC_KWD_HIDDEN);
		ADD_COMMA();

		GROUP_ID(RC_SYM_FIELDKEYS);

		typedef std::list<DebugWatchValueInfo::FieldKey> FieldKeys;
		const FieldKeys& keys = i->GetFieldKeys();

		util_ui32 m = (util_ui32) keys.size();
		WriteNumber(RC_SYM_SIZE, m);
		if (m) ADD_COMMA();

		for (FieldKeys::const_iterator j = keys.begin(); j != keys.end(); ++j) {

			--m;
			GROUP_OPEN();
			
			TUPPLE_ID(RC_SYM_KEYCONTENTREF);
			WriteTupple(
				text, 
				j->HasContentReference(), RC_KWD_KEYREF, RC_KWD_NOKEYREF, 
				true, 
				RC_STRING(j->GetContentReference())
			);
			ADD_COMMA();

			WriteValue(RC_SYM_DISPLAYEDKEY, RC_STRING(j->GetDisplayedText()));

			GROUP_ITEM_DONE(m); // FieldKey
		}
		GROUP_CLOSE(); // FieldKeys

		GROUP_ITEM_DONE(n); // Content
	}
	GROUP_CLOSE(); // Contents

	GROUP_CLOSE(); // Value
	return text;
}

////////////////////////////////////////////////////////////////
