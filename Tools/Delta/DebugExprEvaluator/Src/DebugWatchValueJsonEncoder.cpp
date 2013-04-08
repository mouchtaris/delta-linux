// DebugWatchValueJsonEncoder.cpp
// Json format converter for debug watch values.
// Debug expession evaluator.
// Giannhs Apostolidhs, April 2013.
//

#include "DebugWatchValueJsonEncoder.h"
#include "DebugWatchValueJsonEncodingDefs.h"
#include "DebugWatchValueTextEncoders.h"
#include "ustrings.h"
#include "ulexutil.h"
#include "DDebug.h"

////////////////////////////////////////////////////////////////

#define	JN(s)		std::string(s)
#define JN_SPACE	JN(" ")
#define JN_LINE		JN("\n")
#define	JN_COMMA	JN(",")
#define	JN_CL		JN(":")
#define	JN_LBR		JN("{")
#define	JN_RBR		JN("}")
#define	JN_LBL		JN("[")
#define	JN_RBL		JN("]")
#define	JN_CHOICE(c,s1,s2) ((c)? QUOTED(s1) : QUOTED(s2))
#define	QUOTED(s)	(JN("\"") + (s) + JN("\""))

#define	TABS			stabs
#define	INCTABS()		stabs = utabstops(++tabs)
#define	DECTABS()		stabs = utabstops(--tabs)

#define	OBJ_OPEN()		text.append(TABS + JN_LBR + JN_LINE); INCTABS()
#define	OBJ_ID_OPEN(id)	WriteValue(id, JN_LBR); text.append(JN_LINE); INCTABS()
#define	OBJ_CLOSE()		DECTABS(); text.append(JN_LINE + TABS + JN_RBR)

#define	ARR_OPEN()		text.append(TABS + JN_LBL + JN_LINE); INCTABS()
#define	ARR_ID_OPEN(id)	WriteValue(id, JN_LBL); text.append(JN_LINE); INCTABS()
#define	ARR_CLOSE()		DECTABS(); text.append(JN_LINE + TABS + JN_RBL)

#define	ADD_COMMA()		text.append(JN_COMMA + JN_LINE)

#define	JN_STRING(s) \
	uunparseescapesequences(ucstringarg(s))

////////////////////////////////////////////////////////////////

#define	WriteValue(kwd, val) \
	text.append(TABS + QUOTED( JN(kwd) ) + JN_CL + JN_SPACE + val )

////////////////////////////////////////////////////////////////

static void WriteTupple (
		std::string&		text,
		const std::string&	stabs,
		bool				cond, 
		const std::string&	firstChoice, 
		const std::string&	secondChoice, 
		const std::string&	value
	) {
	WriteValue(JSON_VAL_TYPE, JN_CHOICE(cond, firstChoice, secondChoice) );
	if(cond){
		ADD_COMMA();
		WriteValue(JSON_VAL_VALUE, value);
	}
}

////////////////////////////////////////////////////////////////

const std::string DebugWatchValueJsonEncoder::Encode (const DebugWatchValueInfo& value) {

	util_ui32	tabs = 0;
	std::string stabs;
	std::string text;

	OBJ_OPEN();
	if (value.GetType() == DebugWatchValueInfo::SimpleType){
		WriteValue(JSON_VAL_TYPE, QUOTED( JN_STRING(JSON_KWD_SIMPLE) ) );
		ADD_COMMA();	
		WriteValue(JSON_VAL_VALUE, QUOTED( JN_STRING(value.GetSimple()) ) );
		OBJ_CLOSE();
		return text;
	}
	else
		DASSERT(value.GetType() == DebugWatchValueInfo::CompositeType);

	WriteValue(JSON_VAL_TYPE, QUOTED( JN_STRING(JSON_KWD_COMPOSITE) ) );
	ADD_COMMA();

	OBJ_ID_OPEN(JSON_VAL_VALUE);

	const DebugWatchValueInfo::Composite& comp = value.GetComposite();

	WriteValue(JSON_VAL_OVERVIEW, QUOTED( JN_STRING(comp.GetOverview()) ) );	
	ADD_COMMA();	

	WriteValue(JSON_VAL_ABSOLUTEREF, QUOTED( JN_STRING(comp.GetAbsoluteReference()) ) );	
	ADD_COMMA();	

	const std::list<DebugWatchValueInfo::Content>& contents = comp.GetContents();
	util_ui32 n = (util_ui32) contents.size();

	WriteValue(JSON_VAL_SIZE, uconstructstr("%d", n) );	
	ADD_COMMA();	
	
	ARR_ID_OPEN(JSON_VAL_CONTENTS);
	for (	std::list<DebugWatchValueInfo::Content>::const_iterator i = contents.begin();
			i != contents.end();
			++i		) {
		--n;
		OBJ_OPEN();

		WriteValue(JSON_VAL_SUBINDEX, QUOTED( JN_STRING(i->GetSubIndex()) ) );
		ADD_COMMA();	

		OBJ_ID_OPEN(JSON_VAL_DISPLAYDESC);
			WriteTupple( text, stabs, i->HasDisplayDesc(), JSON_KWD_DESC, JSON_KWD_NODESC, QUOTED( JN_STRING(i->GetDisplayDesc()) ) );
		OBJ_CLOSE();
		ADD_COMMA();

		OBJ_ID_OPEN(JSON_VAL_KEYTYPEINFO);
			WriteTupple( text, stabs, i->HasKeyType(), JSON_KWD_KEYTYPE, JSON_KWD_NOKEYTYPE, QUOTED( JN_STRING(i->GetKeyType()) ) );
		OBJ_CLOSE();
		ADD_COMMA();

		WriteValue(JSON_VAL_KEYACCESS, JN_CHOICE(i->IsKeyVisible(), JSON_KWD_VISIBLE, JSON_KWD_HIDDEN) );
		ADD_COMMA();

		typedef std::list<DebugWatchValueInfo::FieldKey> FieldKeys;
		const FieldKeys& keys = i->GetFieldKeys();

		util_ui32 m = (util_ui32) keys.size();	

		WriteValue(JSON_VAL_SIZE, uconstructstr("%d", m) );	
		ADD_COMMA();

		ARR_ID_OPEN(ENCODING_TAG_FIELDKEYS);

		for (FieldKeys::const_iterator j = keys.begin(); j != keys.end(); ++j) {
			--m;
			OBJ_OPEN();

			OBJ_ID_OPEN(JSON_VAL_KEYCONTENTREF);
				WriteTupple( text, stabs, j->HasContentReference(), JSON_KWD_KEYREF, JSON_KWD_NOKEYREF, QUOTED( JN_STRING(j->GetContentReference()) ) );
			OBJ_CLOSE();
			ADD_COMMA();

			WriteValue(JSON_VAL_DISPLAYEDKEY, QUOTED( JN_STRING(j->GetDisplayedText()) ) );

			OBJ_CLOSE();	// FieldKey
			if (m) ADD_COMMA();
		}
		ARR_CLOSE();	// FieldKeys

		OBJ_CLOSE();	// Content
		if (n) ADD_COMMA();
	}
	ARR_CLOSE();	// Contents

	OBJ_CLOSE();	// type

	OBJ_CLOSE();	// file wrapper

	return text;
}

////////////////////////////////////////////////////////////////