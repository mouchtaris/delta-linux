// DeltaDebugMessages::MessageEncoding.cpp
// Common debug-message encoding functions
// shared between the debug server and client.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Some newer functions August 2008.
// Updated June 2009 to use unbinaryio platform-independent functions.
//

#include "DeltaDebugMessageEncoding.h"
#include "DeltaDebugCommands.h"
#include "DDebug.h"
#include "ubinaryio.h"

//-----------------------------------------------------------

DVMDEBUGCOMMON_FUNC const char* DebugMessages::WriteSimple (const char* buffer, util_ui32 item) {
	ubinaryio::WriteInteger((void*) buffer, item);
	return buffer + ubinaryio::SizeInteger<util_ui32>();
}

DVMDEBUGCOMMON_FUNC const char* DebugMessages::WriteSimple (const char* buffer, bool item) {
	ubinaryio::WriteBool((void*) buffer, item);
	return buffer + ubinaryio::SizeBool();
}

DVMDEBUGCOMMON_FUNC const char* DebugMessages::WriteBinaryData (const char* buffer, void* data, util_ui32 size) {
	memcpy(const_cast<char*>(WriteSimple(buffer, size)), data, size);
	return buffer + SizeSimple(size) + size;
}

DVMDEBUGCOMMON_FUNC char* DebugMessages::ReadSimple (char* buffer, util_ui32* item) {
	ubinaryio::ReadInteger(buffer, item);
	return buffer + ubinaryio::SizeInteger<util_ui32>();
}

DVMDEBUGCOMMON_FUNC char* DebugMessages::ReadSimple (char* buffer, bool* item) {
	ubinaryio::ReadBool(buffer, item);
	return buffer + ubinaryio::SizeBool();
}

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC const char* DebugMessages::WriteString (const char* buffer, const std::string& str) {
	util_ui32 len = (util_ui32) str.length() + 1;
	memcpy((void*) buffer, (void*) str.c_str(), len);
	return buffer + len;
}

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC char* DebugMessages::ReadString (char* buffer, char** str) {	// String read is part of buffer.
	*str = buffer;
	return buffer + strlen(*str) + 1;
}

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC util_ui32 DebugMessages::SizeString (const std::string& str) 
	{ return (util_ui32) str.length() + 1; }

DVMDEBUGCOMMON_FUNC char* DebugMessages::ForwardHeader (char* buffer)
	{ return buffer + ubinaryio::SizeInteger<util_ui32>(); }

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC util_ui32 DebugMessages::SizeStringList (const std::list<std::string>& l) {

	util_ui32 size = sizeof(util_ui32);
	for (	std::list<std::string>::const_iterator i = l.begin();
			i != l.end();
			size += DebugMessages::SizeString((*i++).c_str()))
			; // Intentionally empty stmt

	return size;
}

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC const char*	DebugMessages::WriteStringList (const char* buffer, const std::list<std::string>& l) {

	buffer = WriteSimple(buffer, (util_ui32) l.size());

	for (	std::list<std::string>::const_iterator i = l.begin();
			i != l.end();
			buffer = WriteString(buffer, (*i++).c_str())	)
			; // Intentionally empty stmt

	return buffer;
}

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC char* DebugMessages::ReadStringList (char* buffer, std::list<std::string>& l) {

	util_ui32 n;
	char* p = ReadSimple(buffer, &n);

	while (n--) {
		char* s;
		p = ReadString(p, &s);
		l.push_back(s);
	}

	return p;
}

/////////////////////////////////////////////////////////////

DVMDEBUGCOMMON_FUNC const char*	DebugMessages::WriteUnsignedInt (const char* buffer, util_ui32 val)
	{ return (char*) ubinaryio::WriteInteger((void*) const_cast<char*>(buffer), val); }

DVMDEBUGCOMMON_FUNC char* DebugMessages::ReadUnsignedInt (char* buffer, util_ui32* val) {
	ubinaryio::ReadInteger(buffer, val);
	return (char*) POFFS(buffer, ubinaryio::SizeInteger<util_ui32>());
}

DVMDEBUGCOMMON_FUNC util_ui32 DebugMessages::SizeUnsignedInt (void) 
	{ return ubinaryio::SizeInteger<util_ui32>(); }

//-----------------------------------------------------------

DebugMessages::Buffer::Buffer (void) : 
	buffer((char*) 0), 
	size(0){}

DebugMessages::Buffer::~Buffer() 
	{  Clear(); }

/////////////////////////////////////////////////////////////

char* DebugMessages::Buffer::Get (void) 
	{ return buffer; }

/////////////////////////////////////////////////////////////

void DebugMessages::Buffer::Prepare (util_ui32 requestedSize) {
	if (size < requestedSize) {
		if (buffer)
			DDELARR(buffer);
		size = requestedSize;
		buffer = DNEWARR(char, size);
	}
	else
		DASSERT(buffer);
}

/////////////////////////////////////////////////////////////

void DebugMessages::Buffer::Clear (void) {
	if (buffer) 
		DDELARR(buffer); 
	unullify(buffer); 
}

//-----------------------------------------------------------
