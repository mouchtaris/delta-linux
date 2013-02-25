// DebugWatchValueRcDecoder.cpp
// Decoder to convert from an rc to a watch value.
// A Descent Parser implementation.
// ScriptFighter Project.
// A. Savidis, August 2008.

#include "DDebug.h"
#include "DebugWatchValueInfo.h"
#include "RcLoaderAPI.h"
#include "DeltaTable.h"
#include "RcAttrRDParser.h"
#include "DebugWatchValueRcEncodingDefs.h"

//#define	DUMP_ENCODING

/////////////////////////////////////////////////////////////

#define	CHECKFINAL(e)	if (true) { if (!(e)) { DPTR(t)->DecRefCounter((DeltaValue*) 0); return false; } } else
#define	CHECKERROR(e)	if (!(e)) return false

#define	PARSE(s)		parser.Parse(t, (s).c_str())
#define	GETSTRING()		val = parser.GetResult().GetString()
#define	RC(s)			std::string(s)
#define	dot				+ RC(".") +

/////////////////////////////////////////////////////////////

static bool Parse_Simple (RcAttrParser& parser, DeltaTable* t, DebugWatchValueInfo* at) {
	CHECKFINAL(PARSE(RC(RC_SYM_VALUE)));
	at->Set(parser.GetResult().GetString());
	t->DecRefCounter((DeltaValue*) 0);
	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_string (std::string& val, const std::string& key, RcAttrParser& parser, DeltaTable* t) {
	CHECKERROR(PARSE(key));
	GETSTRING();
	return true;
}

//************************

static bool Parse_number (util_ui32* val, const std::string& key, RcAttrParser& parser, DeltaTable* t) {
	CHECKERROR(PARSE(key));
	*val = (util_ui32) parser.GetResult().GetNumber();
	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_TypeAndValue (
		std::string&			at,
		const std::string&		kwdNone,
		const std::string&		kwdHas,
		const std::string&		key,
		RcAttrParser&			parser, 
		DeltaTable*				t
	) {
	CHECKERROR(PARSE(key dot  RC(RC_SYM_TYPE)));
	std::string val;
	GETSTRING();
	CHECKERROR(val == kwdNone || val == kwdHas);
	if (val == kwdHas)
		CHECKERROR(Parse_string(at, key dot RC(RC_SYM_VALUE), parser, t));
	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_DisplayDesc (
		std::string&			displayDesc,
		const std::string&		key,
		RcAttrParser&			parser, 
		DeltaTable*				t
	) {
	return Parse_TypeAndValue(
				displayDesc,
				RC_KWD_NODESC,
				RC_KWD_DESC,
				key dot RC(RC_SYM_DISPLAYDESC),
				parser,
				t
			);
}

/////////////////////////////////////////////////////////////

static bool Parse_KeyType (
		std::string&			keyType,
		const std::string&		key,
		RcAttrParser&			parser, 
		DeltaTable*				t
	) {
	return Parse_TypeAndValue(
				keyType,
				RC_KWD_NOKEYTYPE,
				RC_KWD_KEYTYPE,
				key dot RC(RC_SYM_KEYTYPEINFO),
				parser,
				t
			);
}

/////////////////////////////////////////////////////////////

static bool Parse_KeyContentReference (
		std::string&			displayDesc,
		const std::string&		key,
		RcAttrParser&			parser, 
		DeltaTable*				t
	) {
	return Parse_TypeAndValue(
				displayDesc,
				RC_KWD_NOKEYREF,
				RC_KWD_KEYREF,
				key dot RC(RC_SYM_KEYCONTENTREF),
				parser,
				t
			);
}

/////////////////////////////////////////////////////////////

static bool Parse_KeyAccess (
		bool*					visible,
		const std::string&		key,
		RcAttrParser&			parser, 
		DeltaTable*				t
	) {
	CHECKERROR(PARSE(key dot RC(RC_SYM_KEYACCESS)));
	std::string val;
	GETSTRING();
	CHECKERROR(val == RC_KWD_VISIBLE || val == RC_KWD_HIDDEN);
	*visible = (val == RC_KWD_VISIBLE);
	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_FieldKey (
		DebugWatchValueInfo::FieldKey&	fieldKey,
		const std::string&				key,
		RcAttrParser&					parser, 
		DeltaTable*						t
	) {

	std::string displayedKey, keyRef;
	CHECKERROR(Parse_string(displayedKey, key dot RC_SYM_DISPLAYEDKEY, parser, t));
	CHECKERROR(Parse_KeyContentReference(keyRef, key, parser, t));

	fieldKey.Set(displayedKey, keyRef);
	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_FieldKeys (
		std::list<DebugWatchValueInfo::FieldKey>&	fieldKeys,
		const std::string&							key,
		RcAttrParser&								parser, 
		DeltaTable*									t
	) {
	
	util_ui32 n;
	CHECKERROR(Parse_number(&n, key dot RC(RC_SYM_SIZE), parser, t));

	for (util_ui32 i = 0; i < n; ++i) {
		DebugWatchValueInfo::FieldKey fieldKey;
		CHECKERROR(Parse_FieldKey(fieldKey, key + uconstructstr("[%d]", i), parser, t));
		fieldKeys.push_back(fieldKey);
	}

	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_Content (
		DebugWatchValueInfo::Content&	content,
		const std::string&				key,
		RcAttrParser&					parser, 
		DeltaTable*						t
	) {

	std::string subIndex;
	CHECKERROR(Parse_string(subIndex, key dot RC(RC_SYM_SUBINDEX), parser, t));

	std::string displayDesc;
	CHECKERROR(Parse_DisplayDesc(displayDesc, key, parser, t));

	std::string keyType;
	CHECKERROR(Parse_KeyType(keyType, key, parser, t));

	bool visible;
	CHECKERROR(Parse_KeyAccess(&visible, key, parser,t ));

	std::list<DebugWatchValueInfo::FieldKey> fieldKeys;
	CHECKERROR(Parse_FieldKeys(fieldKeys, key dot RC(RC_SYM_FIELDKEYS), parser, t));

	content.Set(subIndex, displayDesc, visible, fieldKeys, keyType);
	return true;
}

/////////////////////////////////////////////////////////////

static bool Parse_Contents (
		std::list<DebugWatchValueInfo::Content>&	contents,
		std::string&								overview,
		std::string&								absref,
		const std::string&							key,
		RcAttrParser&								parser, 
		DeltaTable*									t
	) {

	CHECKERROR(Parse_string(overview, key dot RC(RC_SYM_OVERVIEW), parser, t));
	CHECKERROR(Parse_string(absref, key dot RC(RC_SYM_ABSOLUTEREF), parser, t));

	util_ui32 n;
	CHECKERROR(Parse_number(&n, key dot RC(RC_SYM_SIZE), parser, t));
	
	for (util_ui32 i = 0; i < n; ++i) {
		DebugWatchValueInfo::Content content;
		CHECKERROR(Parse_Content(content, key + uconstructstr("[%d]", i), parser, t));
		contents.push_back(content);
	}

	return true;
}

/////////////////////////////////////////////////////////////

bool DebugWatchValueRcDecoder (const std::string& rc, DebugWatchValueInfo* at) {
	
#if	defined(DUMP_ENCODING)
	FILE* fp = fopen("rcencoding.txt","w"); fprintf(fp,"%s", rc.c_str()); fclose(fp);
#endif

	DeltaTable* t = ResourceLoader::LoadFromString(rc);
	CHECKERROR(t);

	RcAttrParser parser;
	std::string val;
		
	CHECKFINAL(PARSE(RC(RC_SYM_TYPE)));
	GETSTRING();
	CHECKFINAL(val == RC_KWD_SIMPLE || val == RC_KWD_COMPOSITE);
	if (val == RC_KWD_SIMPLE)
		return Parse_Simple(parser, t, at);
	else {
		std::list<DebugWatchValueInfo::Content> contents;
		std::string overview, absref;

		CHECKFINAL(Parse_Contents(contents, overview,  absref, RC(RC_SYM_VALUE) dot RC(RC_SYM_CONTENTS), parser, t));
		at->Set(DebugWatchValueInfo::Composite(overview, absref, contents));

		DPTR(t)->DecRefCounter((DeltaValue*) 0);
		return true;
	}
}

/////////////////////////////////////////////////////////////
