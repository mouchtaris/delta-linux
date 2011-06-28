// BufferFile.cpp
// Implementation of BufferFile class.
// GameMaker Project.
// A. Savidis, May 2000.
// Refactored February 2008, made independent of resource map.
//

#include <string.h>
#include "BufferFile.h"
#include "DDebug.h"

//------------------------------------------------------

BufferFile::BufferFile (void* _buffer, util_ui32 _size) 
	{ bfreload(_buffer, _size); }

BufferFile::~BufferFile() {}
 
//------------------------------------------------------
// Reload from a new buffer.
//
void BufferFile::bfreload (void* _buffer, util_ui32 _size) {
	currPtr		= 
	buffer		= _buffer;
	size		= _size;
	currIndex	= 0;
}

//------------------------------------------------------

bool BufferFile::bfeof (void) const
	{ return currIndex >= size; }

//------------------------------------------------------
// Reading here fail to load a whole item due to buffer
// end. In this case, the bfeof() check will try to
// load more if needed, and the loading may continue.
// The currSize is the size required to load the
// current item. An item may be read in various
// successive loads of the buffer file if it happens
// to be at the boundaries or if it is too large.
//
util_ui32 BufferFile::bfread (void* inBuffer, util_i32 itemSize, util_ui32 n) {

	util_ui32 totalRead = 0;
	util_ui32 currSize = itemSize;

	// Continue until the required number of items is read.
	//
	while (totalRead < n) {

		util_ui32 bytesLeft = bfbytesleft();
		util_ui32 bytesRead = currSize > bytesLeft ? bytesLeft : currSize;

		memcpy(inBuffer, currPtr, bytesRead);
		inBuffer	= POFFS(inBuffer, bytesRead);
		currPtr		= POFFS(currPtr, bytesRead);
		currIndex	+= bytesRead;

		DASSERT(currSize >= bytesRead);
		currSize	-= bytesRead;

		if (!currSize) {
			totalRead++;
			currSize = itemSize;
		}

		if (bfeof())			// Less items read.
			return totalRead;
	}

	return n;					// Exactly n items read.
}

//------------------------------------------------------

util_ui32 BufferFile::bfbytesleft (void) const
	{ DASSERT(size >= currIndex); return size - currIndex; }

util_ui32 BufferFile::bfsize (void) const
	{ return size; }

//------------------------------------------------------

char* BufferFile::bfloadstr (void) {

	util_ui32 len;
	if (!bfread((void*) &len, sizeof(util_ui32), 1))
		return (char*) 0;

	char* s = DNEWARR(char,len);
	if (bfread((void*) s, sizeof(char), len) != len) {
		DDELARR(s);
		return (char*) 0;
	}

	return s;
}

//------------------------------------------------------

bool BufferFile::bfloadstr (std::string& at) {

	util_ui32 len;
	if (!bfread(&len, sizeof(len), 1))
		return false;

	char add[2] = { '\0', '\0' };
	while (len--)
		if (!bfread(&add[0], sizeof(char), 1))
			return false;
		else
			at.append(add);

	return true;
}

//------------------------------------------------------

bool BufferFile::bfloadstr (char* s) {

	*s='\0';
	util_ui32 len;
	if (!bfread((void*) &len, sizeof(util_ui32), 1))
		return false;

	if (!bfread((void*) s, sizeof(char), len)) {
		*s='\0';
		return false;
	}

	return true;
}

//------------------------------------------------------
