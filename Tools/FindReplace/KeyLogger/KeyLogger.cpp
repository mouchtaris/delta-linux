/*
 * KeyLogger.cpp
 * author: Koutsopoulos Nikolaos, Christofilopoulos Michalis
 * mail	 : koutsop@ics.forth.gr, mhristof@csd.uoc.om
 */
 
#include "keylogger.h"


namespace frep {

std::ofstream KeyLogger::logfile;
bool KeyLogger::enabled;

//-----------------------------------------------------------------------

bool KeyLogger::Init(char *filename){
	logfile.open(filename);
	enabled = true;
	return logfile.is_open();
}

//-----------------------------------------------------------------------

bool KeyLogger::Write(const char* const format, ...){
	if(!(enabled) || !(logfile).is_open())
		return false;
	else {
		char temp[BUFFER_SIZE]; //tmp buffer to place the msg to be printed.
		va_list va;
		va_start(va, format);
		vsnprintf_s(temp, BUFFER_SIZE, BUFFER_SIZE, format, va);
		logfile << temp;
		logfile.flush();
		va_end(va);
	}
	return true;
}

//-----------------------------------------------------------------------

void KeyLogger::Enable(void)
	{ enabled = true; }

//-----------------------------------------------------------------------

void KeyLogger::Disable(void)
	{ enabled = false; }

//-----------------------------------------------------------------------

void KeyLogger::Terminate(void)
	{ logfile.close(); }

}	//namespace frep