/**
 *	BinaryBuffer.cpp
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	November 2009
 */
#include "BinaryBuffer.h"
#include <string.h>

//**********************************************************************

BinaryBuffer::BinaryBuffer(void) : data(0) {}

BinaryBuffer::BinaryBuffer(void *data, unsigned size) : data(0)
{
	AllocateSize(size);
	memcpy(this->data->data, data, size);
}

BinaryBuffer::BinaryBuffer(const BinaryBuffer& buffer) : data(buffer.data)
	{ if (data) ++data->refCounter; }

BinaryBuffer::~BinaryBuffer() { if (data && --data->refCounter == 0) Clear(); }

//**********************************************************************

void * BinaryBuffer::AllocateSize(unsigned size)
{
	data = new Data;
	data->data = new char[data->size = size];
	data->refCounter = 1;	
	return GetData();
}

//**********************************************************************

void BinaryBuffer::Clear(void)
{
	if (data) {
		delete[] data->data;
		delete data;
		data = (Data *) 0;
	}
}