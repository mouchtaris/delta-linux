// ubinary.cpp
// Platform (machine) independent binary I/O.
// Both buffers and files.
// ScriptFighter Project.
// A. Savidis, March 2009.
//

#include "ubinaryio.h"
#include "ufunctors.h"
#include <string.h>
#include <algorithm>

#define	DOUBLE_ENCODING_BYTES	8
#define	DOUBLE_PLATFORM_BYTES	sizeof(double)

namespace ubinaryio {

//-----------------------------------------------------------------

UTILLIB_FUNC void* WriteByte (void* b, util_ui8 byte, util_ui32 n) {
	if (n == 1)
		*((util_ui8*) b) = byte;
	else
		memset(b, byte, n);
	return POFFS(b, n);
}

UTILLIB_FUNC void* ReadByte (void* b, util_ui8* byte) {
	*byte = *((util_ui8*) b);
	return POFFS(b, 1);
}

//***************************

UTILLIB_FUNC void* WriteBool (void* b, bool val)
	{ return WriteByte(b, val ? 1 : 0); }

UTILLIB_FUNC void* WriteBool (void* b, util_ui32* n, bool val) {	// 0 or 1 
	DASSERT(*n >= 1);
	--*n;
	return WriteByte(b, val ? 1 : 0);
}

UTILLIB_FUNC void* ReadBool (void* b, util_ui32* n, bool* val) {
	DASSERT(*n >= 1);
	--*n;
	ReadBool(b, val);
	return POFFS(b, 1);
}

UTILLIB_FUNC void ReadBool (void* b, bool* val) 
	{ *val = *((util_ui8*) b) == 1; }

//***************************
// |8 byte precision with optional zero padding|
//
UTILLIB_FUNC void* WriteDouble (void* b, util_ui32* n, double val) {

	DASSERT(*n >= DOUBLE_ENCODING_BYTES + 1);

	*((util_ui8*) b) = DOUBLE_ENCODING_BYTES;
	PINC(b, 1);

	util_ui32	padding = 0, size = DOUBLE_ENCODING_BYTES;
	void*		source	= &val;

	if (DOUBLE_PLATFORM_BYTES < DOUBLE_ENCODING_BYTES) {
		padding = DOUBLE_ENCODING_BYTES - DOUBLE_PLATFORM_BYTES;	
		size	= DOUBLE_PLATFORM_BYTES;
	}
	else
	if (DOUBLE_PLATFORM_BYTES > DOUBLE_ENCODING_BYTES)
		PINC(source, DOUBLE_PLATFORM_BYTES - DOUBLE_ENCODING_BYTES);	// Skip prec bytes. 

	if (padding) 
		b = WriteByte(b, 0, padding);
	memcpy(b, source, size);

	*n -= DOUBLE_ENCODING_BYTES + 1;
	return POFFS(b, size);
}

//***************************

UTILLIB_FUNC void* ReadDouble (void* b, util_ui32* n, double* val) {

	DASSERT(*n >= DOUBLE_ENCODING_BYTES + 1);
	ReadDouble(b, val);
	*n -= DOUBLE_ENCODING_BYTES + 1;
	return POFFS(b,  DOUBLE_ENCODING_BYTES + 1);
}

UTILLIB_FUNC void ReadDouble (void* b, double* val) {
	util_ui8 precision = *((util_ui8*) b);
	PINC(b, 1);
	*val = 0;
	void* dest = val;

	util_ui32 size = precision;
	if (precision < DOUBLE_PLATFORM_BYTES)
		PINC(dest, DOUBLE_PLATFORM_BYTES - precision);
	else 
	if (precision > DOUBLE_PLATFORM_BYTES) {
		PINC(b,  precision - DOUBLE_PLATFORM_BYTES); // Ignore precision.
		size = DOUBLE_PLATFORM_BYTES;
	}
	
	memcpy(dest, b, size);
}

//***************************
// |LENGTH+1|NULL TERMINATED STRING|
//
UTILLIB_FUNC void* WriteString (void* b, util_ui32* n, const std::string& s) {
	util_ui32 len = (util_ui32) s.length() + 1;
	DASSERT(*n >= len + SizeInteger<util_ui32>());
	memcpy(b = WriteInteger(b, n, len), s.c_str(), len);
	*n -= len;
	return POFFS(b, len);
}

UTILLIB_FUNC void ReadString (void* b, std::string& s) {
	util_ui32 len;
	ReadInteger(b, &len);
	s = (char*) POFFS(b, SizeInteger<util_ui32>()); 
}

UTILLIB_FUNC void* ReadString (void* b, util_ui32* n, std::string& s) {
	util_ui32 len;
	s = (char*) (b = ReadInteger(b, n, &len));
	if (len != s.length() + 1 || len > *n)
		return (void*) 0;
	else {
		*n -= len;
		return POFFS(b, len);
	}	
}

//-----------------------------------------------------------------

UTILLIB_FUNC bool WriteByte (FILE* fp, util_ui8 byte, util_ui32 n)
	{ return fwrite(&byte, 1, n, fp) == n; }

UTILLIB_FUNC bool ReadByte (FILE* fp, util_ui8* byte)
	{ return fread(byte, 1, 1, fp) == 1; }

//***************************

UTILLIB_FUNC bool WriteBool (FILE* fp, bool val) {	// 0 or 1 
	return WriteByte(fp, val ? 1 : 0);
}

UTILLIB_FUNC bool ReadBool (FILE* fp, bool* val) {
	util_ui8 byte;
	if (!ReadByte(fp, &byte))
		return false;
	else 
		{ *val = byte == 1; return true; }
}

//***************************

UTILLIB_FUNC bool WriteDouble (FILE* fp, double val) {	// 0 or 1 

	if (!WriteByte(fp, DOUBLE_ENCODING_BYTES))
		return false;

	util_ui32	padding = 0, size = DOUBLE_ENCODING_BYTES;
	void*		source		= &val;

	if (DOUBLE_PLATFORM_BYTES < DOUBLE_ENCODING_BYTES) {
		padding = DOUBLE_ENCODING_BYTES - DOUBLE_PLATFORM_BYTES;	
		size	= DOUBLE_PLATFORM_BYTES;
	}
	else
	if (DOUBLE_PLATFORM_BYTES > DOUBLE_ENCODING_BYTES)
		PINC(source, DOUBLE_PLATFORM_BYTES - DOUBLE_ENCODING_BYTES);	// Skip prec bytes. 

	if (!WriteByte(fp, 0, padding))
		return false;
	else
		return fwrite(source, size, 1, fp) == 1;
}

//***************************

UTILLIB_FUNC bool ReadDouble (FILE* fp, double* val) {

	util_ui8 precision;
	if (!ReadByte(fp, &precision))
		return false;

	*val		= 0;
	void* dest = val;

	util_ui32 size = precision, padding = 0;
	if (precision < DOUBLE_PLATFORM_BYTES)
		PINC(dest, DOUBLE_PLATFORM_BYTES - precision);
	else 
	if (precision > DOUBLE_PLATFORM_BYTES) {
		padding = precision - DOUBLE_PLATFORM_BYTES;
		size	= DOUBLE_PLATFORM_BYTES;
	}

	while (padding--)
		if (!ReadByte(fp, &precision) || precision != 0)
			return false;

	return fread(dest, size, 1, fp) == 1;
}

//***************************

UTILLIB_FUNC bool WriteString (FILE* fp,  const std::string& s) {
	util_ui32 len = (util_ui32) s.length() + 1;
	WriteInteger(fp, len);
	fwrite(s.c_str(), len, 1, fp);
	return true;
}

UTILLIB_FUNC bool ReadString (FILE* fp, std::string& s) {

	util_ui32 len;
	if (!ReadInteger(fp, &len))
		return false;
	DASSERT(len);

	char c;
	s.clear();
	while (true) {
		if (feof(fp) || !len || fread(&c, 1, 1, fp) != 1)
			return false;
		--len;
		if (!c)
			return !len;	// Should be 0 at null terminator.
		else
			s.push_back(c);
	}
}
//***************************

UTILLIB_FUNC bool WriteBuffer (FILE* fp, const void* buffer, util_ui32 size) 
	{ return fwrite(buffer, size, 1, fp) == 1; }

UTILLIB_FUNC bool ReadBuffer (FILE* fp, void* buffer, util_ui32 size)
	{ return fread(buffer, size, 1, fp) == 1; }

//-----------------------------------------------------------------

UTILLIB_FUNC util_ui32  SizeBool (void)	{ return 1; }
UTILLIB_FUNC util_ui32  SizeDouble (void)	{ return DOUBLE_ENCODING_BYTES + 1; }
UTILLIB_FUNC util_ui32	SizeString (const std::string& s)
	{ return SizeInteger<util_ui32>() + s.length() + 1; }

//-----------------------------------------------------------------

#define	WRITE(_size, _type, tag)					\
	util_ui32 n = _size;							\
	Packet packet(DNEWARR(util_ui8, n), n, tag);	\
	ubinaryio::Write##_type(packet.first, &n, val);	\
	Add(packet)

void OutputBuffer::WriteDouble (double val) 
	{ WRITE(DOUBLE_ENCODING_BYTES + 1, Double, double_tag); }

void OutputBuffer::WriteString (const std::string& val)
	{ WRITE(SizeString(val), String, string_tag); }

void OutputBuffer::WriteBuffer (const void* buffer, util_ui32 size) {
	Packet packet(DNEWARR(util_ui8, size), size, buffer_tag);	
	memcpy(packet.first, buffer, size);
	Add(packet);
}

//***************************

void OutputBuffer::Append (const OutputBuffer& ob) {
	if (this != &ob)
		for (PacketList::const_iterator i = ob.packets.begin(); i != ob.packets.end(); ++i) {
			util_ui8* p = DNEWARR(util_ui8, i->second);
			memcpy(p, i->first, i->second);
			Add(Packet(p, i->second, i->third));
		}
}

//***************************

void* OutputBuffer::MakeBuffer (util_ui32* n) const {

	DASSERT(size);
	void* buffer	= DNEWARR(util_ui8, size);
	void* p			= buffer;

	*n = 0;
	for (PacketList::const_iterator i = packets.begin(); i != packets.end(); ++i) {
		memcpy(p, i->first, i->second);
		*n += i->second;
		PINC(p, i->second);
	}

	DASSERT(*n == size);
	return buffer;
}

void OutputBuffer::ClearBuffer (void* data)
	{ DDELARR((util_ui8*) data); }

//***************************

void OutputBuffer::Clear (void) {
	std::for_each(
		packets.begin(),
		packets.end(),
		ufunctor_first<
			Packet, 
			uarrdestructorfunctor<util_ui8*>
		>(uarrdestructorfunctor<util_ui8*>())
	);
	packets.clear();
	size = 0;
}

//***************************

void OutputBuffer::Flush (FILE* fp) const {
	for (PacketList::const_iterator i = packets.begin(); i != packets.end(); ++i)
		fwrite(i->first, i->second, 1, fp);
}

//***************************

OutputBuffer::OutputBuffer (void) : size(0){} 
OutputBuffer::~OutputBuffer() 
	{ Clear(); }

//-----------------------------------------------------------------

#define	READ(_size, _type)									\
	util_ui32 n = _size;									\
	if (n > remaining)										\
		return false;										\
	else {													\
		ubinaryio::Read##_type(curr, val);					\
		PINC(curr, n);										\
		remaining -= n;										\
		return true;										\
	}

//***************************

bool InputBuffer::ReadByte (util_ui8* val) 
	{ READ(1, Byte); }

bool InputBuffer::ReadBool (bool* val) 
	{  READ(SizeBool(), Bool); }

bool InputBuffer::ReadDouble (double* val)
	{  READ(DOUBLE_ENCODING_BYTES + 1, Double); }

bool InputBuffer::ReadString (std::string& val) {
	util_ui32 n = remaining;
	void* b = ubinaryio::ReadString(curr, &n, val);
	if (!b)
		return false;
	else {
		remaining = n;
		curr = b;
		return true;
	}
}

//***************************

bool InputBuffer::ReadBuffer (void* _buffer, util_ui32 _size) {
	if (_size > remaining)
		return false;
	memcpy(_buffer, curr, _size);
	remaining -= _size;
	PINC(curr, _size);
	return true;
}

//***************************

void InputBuffer::Set (const OutputBuffer& ob) { 
	DDELARR((util_ui8*) buffer);
	new (this) InputBuffer(ob);
}

void InputBuffer::Set (void* _buffer, util_ui32 _size) { 
	DDELARR((util_ui8*) buffer);
	new (this) InputBuffer(_buffer, _size);
}

//***************************

void* InputBuffer::MakeRemainingBuffer (util_ui32* n) const {
	if (!(*n = GetRemainingBytes()))
		return (void*) 0;
	else
		return memcpy(DNEWARR(util_ui8, *n), curr, *n);
}

//***************************

InputBuffer::InputBuffer (const OutputBuffer& ob) : isBufferCopied(true) {
	curr = buffer = ob.MakeBuffer(&size);
	remaining = size;
}

InputBuffer::InputBuffer (void* _buffer, util_ui32 _size, bool copy) : isBufferCopied(copy) {
	if (copy) {
		curr = buffer = DNEWARR(util_ui8, _size);
		memcpy(buffer, _buffer, size = remaining = _size);
	}
	else {
		curr = buffer = _buffer;
		size = remaining = _size;
	}
}

InputBuffer::~InputBuffer() 
	{ if (isBufferCopied) DDELARR((util_ui8*) buffer); }

//-----------------------------------------------------------------

}
