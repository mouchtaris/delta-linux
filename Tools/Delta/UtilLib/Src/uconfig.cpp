///////////////////////////////////////////////////////////
// uconfig.cpp
// Simple config mechanism with key value pairs.
// ScriptFighter Project.
// A. Savidis, July 2014.
//

#include "DDebug.h"
#include "uconfig.h"
#include "ustrings.h"

///////////////////////////////////////////////////////////

bool uconfig::Restart (const std::string& path) {

	DASSERT(!fp);

	if (!(fp = fopen(path.c_str(), "rt"))) {
		SetError(uconstructstr("Failed to open '%s'!", ucstringarg(path)));
		return false;
	}
	else {
		values.clear();
		error.clear();
		eof = false;
		lookAhead = line = col = 0;
		return true;
	}	
}

///////////////////////////////////////////////////////////

void uconfig::SetError (const std::string& s) {
	if (error.empty()) {
		error += uconstructstr("line %u, col %u:", line, col);
		eof = true;
	}
	else
		DASSERT(eof);
	error += s;
}

///////////////////////////////////////////////////////////

char uconfig::Input (void) {
	if (eof)
		return EOF;
	else {
		char c;
		if (lookAhead)
			{ c = lookAhead; lookAhead = 0; }
		else {
			c = fgetc(fp);
			if (c == EOF)
				eof = true;
		}

		if (c != EOF)
			if (c == '\n')
				{ ++line; col = 0; }
			else
				++col;

		return c;
	}
}

///////////////////////////////////////////////////////////

void uconfig::SkipWhitespace (void) {

	DASSERT(!lookAhead);
	
	char c = ' ';
	while (!eof && isspace(c))
		c = Input();

	if (!eof)
		lookAhead = c;
}

///////////////////////////////////////////////////////////

bool uconfig::ParseItem (std::string& s) {

	s.clear();
	SkipWhitespace();

	while (true) {	

		char c = Input();
		if (eof)
			return false;
		else
		if (!isprint(c)) {
			SetError("Non printable character!");
			return false;
		}
		else
		if (isspace(c)) {
			DASSERT(!s.empty());
			return true;
		}
		else
			s += c;
	}
}

///////////////////////////////////////////////////////////

bool uconfig::Parse (const std::string& path) {

	if (!Restart(path))
		return false;

	std::string key, val;

	while (!eof) {

		if (!ParseItem(key))
			if (eof)
				continue;
			else
				SetError("Error reading <key>.");
		else
		if (!ParseItem(val))
			SetError("Error reading <value>.");
		else
			values[key] = val;
	}

	bool hasError = !error.empty();
	if (hasError)
		values.clear();
	return !hasError;
}

///////////////////////////////////////////////////////////

bool uconfig::GetBool (const std::string& id) const {

	Values::const_iterator i = values.find(id);
	DASSERT(i != values.end());

	std::string val (i->second);
	ustrupper(val);

	if (val == UBOOL_CONST_STRING_TRUE)
		return true;
	else
	if (val == UBOOL_CONST_STRING_FALSE)
		return false;
	else
		{ DASSERT(false); return false; }
}

///////////////////////////////////////////////////////////

util_i32 uconfig::GetInt (const std::string& id) const {
	Values::const_iterator i = values.find(id);
	DASSERT(i != values.end());
	return atoi(ucstringarg(i->second));
}

///////////////////////////////////////////////////////////

float uconfig::GetFloat (const std::string& id) const {
	Values::const_iterator i = values.find(id);
	DASSERT(i != values.end());
	return float(atof(ucstringarg(i->second)));
}

///////////////////////////////////////////////////////////

const std::string& uconfig::GetString (const std::string& id) const {
	Values::const_iterator i = values.find(id);
	DASSERT(i != values.end());
	return i->second;
}

///////////////////////////////////////////////////////////
