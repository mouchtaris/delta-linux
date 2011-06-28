// RcAttrRDParser.cpp
// Attribute expression parser to be used from within
// C++ programs for the Resource Loader. It is implemented
// as an RDP with a left-factored grammar using inherited
// attributes whose values are passed as parameters to the
// recursive parsing functions.
//
// ScriptFigher Project.
// A. Savidis, January 2007.
//
#include <stdarg.h>
#include "RcAttrRDParser.h"
#include "DeltaValue.h"
#include "DeltaString.h"

////////////////////////////////////////////////////////////////

RcAttrParser::RcAttrParser (void) :
	tokenType(RcAttrLexAn::Error),
		hasError(false),
		errorMsg(""),
		unparsedInput(""),
		resources((DeltaTable*) 0){}

void RcAttrParser::Value::Set (DeltaTable*  x) {
	type	= Table; 
	tab		= x; 
	DeltaString* s = DeltaString::New();
	DPTR(x)->ConvertToString(s); 
	toString = DPTR(s)->Content();
	DeltaString::Delete(s);
}

////////////////////////////////////////////////////////////////

bool RcAttrParser::Match (RcAttrLexAn::TokenType token)  {
	DASSERT(CurrTokenType() == token);
	if (CurrTokenType() != token)
		{ Error("Token not matched!"); return false; }
	else
		return true;
}

//****************************

void RcAttrParser::Error (const char* format, ...) {
	va_list args;
	va_start(args, format);
	static char errorText[256];
	vsprintf(errorText, format, args);
	DASSERT(strlen(errorText) < uarraysize(errorText));
	va_end(args);
	errorMsg = errorText;
	hasError = true;
}

////////////////////////////////////////////////////////////////
// E -> A, FIRST(A) = { Ident }
//
bool RcAttrParser::E_parse(DeltaTable* t, Value& val) { 

	GetNextToken();
	if (CurrTokenType() == RcAttrLexAn::Ident)	// FIRST(A)
		return A_parse(t, val);
	else {
		Error("'%s' 'id' or 'num' expected!", unparsedInput.c_str());
		return false;
	}
}

//****************************
// A -> id B
//
bool RcAttrParser::A_parse(DeltaTable* t, Value& val) {

	if (!Match(RcAttrLexAn::Ident))
		return false;
	unparsedInput.append(lexan.GetIdent());

	DeltaValue index, content;
	index.FromString(lexan.GetIdent().c_str());

	if (!DPTR(t)->Get(index, &content)) {
		Error("'%s' not found!", unparsedInput.c_str());
		return false;
	}
	else
		return B_dispatcher(content, val);
}

//****************************
// B -> B1 | B2 | empty FIRST(B1) = {.}, FIRST(B2)={ [ }
//
bool RcAttrParser::B_dispatcher(DeltaValue& attr, Value& val) {

	GetNextToken();
	
	if (CurrTokenType() == RcAttrLexAn::Dot) {
		if (attr.Type() != DeltaValue_Table) {
			Error("'%s.' not applied on array!", unparsedInput.c_str());
			return false;
		}
		else
			return B1_parse(attr.ToTable(), val);
	}
	else
	if (CurrTokenType() == RcAttrLexAn::LeftBracket) {
		if (attr.Type() != DeltaValue_Table) {
			Error("'%s[' not applied on array!", unparsedInput.c_str());
			return false;
		}
		else
			return B2_parse(attr.ToTable(), val);
	}
	else
	if (CurrTokenType() == RcAttrLexAn::EndOfString) {

		// Here we may only get a value.
		//
		if (attr.Type() == DeltaValue_Number)
			{ val.Set(attr.ToNumber()); return true; }
		else
		if (attr.Type() == DeltaValue_String)
			{ val.Set(attr.ToString()); return true; }
		else
		if (attr.Type() == DeltaValue_Bool)
			{ val.Set(attr.ToBool()); return true; }
		else
		if (attr.Type() == DeltaValue_Table)
			{ val.Set(attr.ToTable()); return true; }
		else {
			Error("'%s' not string / num / bool / object!", unparsedInput.c_str());
			return false;
		}
	}
	else {
		Error("., E or EOS expected after '%s'!", unparsedInput.c_str());
		return false;
	}
}

//****************************
// B1 -> .id B
//
bool RcAttrParser::B1_parse(DeltaTable* t, Value& val) { 
	if (!Match(RcAttrLexAn::Dot))
		return false;
	unparsedInput.append(".");

	GetNextToken();

	if (CurrTokenType() == RcAttrLexAn::Ident) {

		unparsedInput.append(lexan.GetIdent().c_str());

		DeltaValue index, content;
		index.FromString(lexan.GetIdent().c_str());

		if (!DPTR(t)->Get(index, &content)) {
			Error("'%s' not found!", unparsedInput.c_str());
			return false;
		}
		else
			return B_dispatcher(content, val);
	}
	else {
		Error("'id' expected after '%s'", unparsedInput.c_str());
		return false;
	}
}

//****************************
// B2 -> [num] B
//
bool RcAttrParser::B2_parse(DeltaTable* t, Value& val) { 

	if (!Match(RcAttrLexAn::LeftBracket))
		return false;
	unparsedInput.append("[");

	GetNextToken();
	if (CurrTokenType() != RcAttrLexAn::Number) {
		Error("'num' expected after '%s'", unparsedInput.c_str());
		return false;
	}
	util_ui32 num = lexan.GetNumber();	// Need to store locally.
	unparsedInput.append(uconstructstr("%d", num));

	GetNextToken();
	if (CurrTokenType() != RcAttrLexAn::RightBracket) {
		Error("] expected after '%s'", unparsedInput.c_str());
		return false;
	}

	unparsedInput.append("]");
	DeltaValue index, content;
	index.FromNumber((DeltaNumberValueType) num);

	if (!DPTR(t)->Get(index, &content)) {
		Error("'%s' not found!", unparsedInput.c_str());
		return false;
	}
	else
		return B_dispatcher(content, val);
}

////////////////////////////////////////////////////////////////

bool RcAttrParser::Parse (DeltaTable* t, const char* expr) { 

	DASSERT(t);
	resources		= t;
	unparsedInput	=  "rc.";

	new (&result) Value();	// Reset.

	lexan.Start(expr);
	bool status = true;
	if (E_parse(resources, result)) {
		if (!lexan.EndOfInput()) {
			Error(
				"Remained input '%s' unrecognized!", 
				lexan.RemainingInput().c_str()
			);
			status = false;
		}
	}
	else
		status = false;
	lexan.Clear();
	return status;
}

////////////////////////////////////////////////////////////////
