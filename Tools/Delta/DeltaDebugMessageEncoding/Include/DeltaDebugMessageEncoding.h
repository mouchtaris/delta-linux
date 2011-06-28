// DeltaDebugMessageEncoding.h
// Common debug-message encoding functions
// shared between the debug server and client.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Some newer functions August 2008.
// Updated June 2009 to use unbinaryio platform-independent functions.
//

#ifndef	DELTADEBUGMESSAGEENCODING_H
#define	DELTADEBUGMESSAGEENCODING_H

#include "DeltaDebuggerCommonDefs.h"
#include <string.h>
#include "utypes.h"
#include "ubinaryio.h"
#include <list>

////////////////////////////////////////////////////////

namespace DebugMessages {

DVMDEBUGCOMMON_FUNC const char*	WriteString (const char* buffer, const std::string& str);
DVMDEBUGCOMMON_FUNC const char*	WriteStringList (const char* buffer, const std::list<std::string>& l);
DVMDEBUGCOMMON_FUNC const char*	WriteUnsignedInt (const char* buffer, util_ui32 val);
DVMDEBUGCOMMON_FUNC const char* WriteSimple (const char* buffer, util_ui32 item);
DVMDEBUGCOMMON_FUNC const char* WriteSimple (const char* buffer, bool item);
DVMDEBUGCOMMON_FUNC const char* WriteBinaryData (const char* buffer, void* data, util_ui32 size);

DVMDEBUGCOMMON_FUNC char*		ReadStringList (char* buffer, std::list<std::string>& l);
DVMDEBUGCOMMON_FUNC char*		ReadString (char* buffer, char** str);
DVMDEBUGCOMMON_FUNC char*		ReadUnsignedInt (char* buffer, util_ui32* val);
DVMDEBUGCOMMON_FUNC char*		ReadSimple (char* buffer, util_ui32* item);
DVMDEBUGCOMMON_FUNC char*		ReadSimple (char* buffer, bool* item);

DVMDEBUGCOMMON_FUNC util_ui32	SizeString (const std::string& str);
DVMDEBUGCOMMON_FUNC util_ui32	SizeStringList (const std::list<std::string>& l);
DVMDEBUGCOMMON_FUNC util_ui32	SizeUnsignedInt (void);
DVMDEBUGCOMMON_FUNC char*		ForwardHeader (char* buffer);

template <class T> util_ui32 SizeSimple (T item) 
	{ return sizeof(T); }

class DVMDEBUGCOMMON_CLASS  Buffer {
	private:
	char*		buffer;
	util_ui32	size;

	public:
	char*	Get (void);
	void	Prepare (util_ui32 requestedSize);
	void	Clear (void);

	Buffer (void);
	~Buffer();
};

}

////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

