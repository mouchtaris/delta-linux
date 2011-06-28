/**
 *	BinaryBuffer.h
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	TODO (29/11/2010): the serialization functions to be moved within a central project where
 *  types and conversions offered by components are placed in distinct headers.
 *	November 2009
 */
#ifndef BINARY_BUFFER_H_
#define BINARY_BUFFER_H_

#include "LibraryAPIDecls.h"

class _BASE_API BinaryBuffer {
public:
	BinaryBuffer(void);
	BinaryBuffer(void *data, unsigned size);
	BinaryBuffer(const BinaryBuffer& buffer);
	virtual ~BinaryBuffer();

	void *	AllocateSize	(unsigned size);
	void	Clear			(void);

	void *		GetData(void) const { return data ? data->data : (void *) 0; }
	unsigned	GetSize(void) const { return data ? data->size : 0; }
private:
	struct Data {
		void *data;
		unsigned size;
		unsigned refCounter;
	};
	Data *data;
};

class InputBinaryBuffer : public BinaryBuffer {
	public:
	InputBinaryBuffer (void){}
	InputBinaryBuffer(void *data, unsigned size) : BinaryBuffer(data, size){}
	InputBinaryBuffer(const InputBinaryBuffer& buffer) : BinaryBuffer(buffer){}
	~InputBinaryBuffer(){}
};

class SerializedEditorAST : public BinaryBuffer {
	public:
	SerializedEditorAST (void){}
	SerializedEditorAST(void *data, unsigned size) : BinaryBuffer(data, size){}
	SerializedEditorAST(const SerializedEditorAST& buffer) : BinaryBuffer(buffer){}
	~SerializedEditorAST(){}
};

// ATTENTION(29/11/2010): Do not put other overloaded load /save in here. Place them
// in separate component-specific headers.

#endif	//BINARY_BUFFER_H_