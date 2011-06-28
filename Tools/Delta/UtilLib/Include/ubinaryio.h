// ubinaryio.h
// Platform (machine) independent binary I/O.
// Both buffers and files.
// ScriptFighter Project.
// A. Savidis, March 2009.
//

#ifndef	UBINARYIO_H
#define	UBINARYIO_H

#include "utypes.h"
#include "uptr.h"
#include "ufunctors.h"
#include "DDebug.h"

#include <string>
#include <stdio.h>
#include <list>
#include <functional>

//---------------------------------------------------------------

namespace ubinaryio {

	///////////////////////////////////////////////////////////////////
	// SIZE

	template <typename T>
	util_ui32				SizeInteger (void) { return sizeof(T); }
	UTILLIB_FUNC util_ui32  SizeBool (void);
	UTILLIB_FUNC util_ui32  SizeDouble (void);
	UTILLIB_FUNC util_ui32	SizeString (const std::string& s);

	///////////////////////////////////////////////////////////////////
	// BUFFER I/O

	template <typename T>
	void* WriteInteger (void* b, T val) {	// Write from lsb->msb
		for (util_ui32 i = sizeof(T); i; --i, val >>= 8)
			b = WriteByte(b, val & 0xff);
		return b;
	}

	template <typename T>
	void* WriteInteger (void* b, util_ui32* n, T val) {	// Write from lsb->msb
		DASSERT(*n >= sizeof(T));
		*n -= sizeof(T);
		return WriteInteger(b, val);
	}

	template <typename T>
	void ReadInteger (void* b, T* val) {	// Read from lsb->msb
		*val = 0;
		util_ui8 byte, shifts = 0;
		for (util_ui32 i = sizeof(T); i; --i, *val |= (byte << shifts), shifts += 8) {
			byte = *((util_ui8*) b);
			PINC(b, 1);
		}
	}

	template <typename T>
	void* ReadInteger (void* b, util_ui32* n, T* val) {	// Read from lsb->msb
		DASSERT(*n >= sizeof(T));
		ReadInteger(b, val);
		*n -= sizeof(T);
		return POFFS(b, sizeof(T));
	}

	UTILLIB_FUNC void* WriteByte (void* b, util_ui8 byte, util_ui32 n = 1);
	UTILLIB_FUNC void* WriteBool (void* b, util_ui32* n, bool val);
	UTILLIB_FUNC void* WriteBool (void* b, bool val);
	UTILLIB_FUNC void* WriteDouble (void* b, util_ui32* n, double val);	// |precision 1 byte|rest of bytes|
	UTILLIB_FUNC void* WriteString (void* b, util_ui32* n, const std::string& s);

	UTILLIB_FUNC void* ReadByte (void* b, util_ui8* byte);
	UTILLIB_FUNC void* ReadBool (void* b, util_ui32* n, bool* val);
	UTILLIB_FUNC void* ReadDouble (void* b, util_ui32* n, double* val);
	UTILLIB_FUNC void* ReadString (void* b, util_ui32* n, std::string& s); // Returns null on failure.

	UTILLIB_FUNC void ReadBool (void* b, bool* val);
	UTILLIB_FUNC void ReadDouble (void* b, double* val);
	UTILLIB_FUNC void ReadString (void* b, std::string& s);

	///////////////////////////////////////////////////////////////////
	// FILE I/O

	UTILLIB_FUNC bool WriteByte (FILE* fp, util_ui8 byte, util_ui32 n = 1);
	UTILLIB_FUNC bool WriteBool (FILE* fp,  bool val);
	UTILLIB_FUNC bool WriteDouble (FILE* fp, double val);
	UTILLIB_FUNC bool WriteString (FILE* fp, const std::string& s);
	UTILLIB_FUNC bool WriteBuffer (FILE* fp, const void* buffer, util_ui32 size);

	UTILLIB_FUNC bool ReadByte (FILE* fp, util_ui8* byte);
	UTILLIB_FUNC bool ReadBool (FILE* fp, bool* val);
	UTILLIB_FUNC bool ReadDouble (FILE* fp, double* val);
	UTILLIB_FUNC bool ReadString (FILE* fp, std::string& s);
	UTILLIB_FUNC bool ReadBuffer (FILE* fp, void* buffer, util_ui32 size);

	template <typename T>
	bool WriteInteger (FILE* fp, T val) {	// Write from lsb->msb
		for (util_ui32 i = sizeof(T); i; --i, val >>= 8)
			if (!WriteByte(fp, val & 0xff))
				return false;
		return true;
	}

	template <typename T>
	bool ReadInteger (FILE* fp, T* val) {	// Read from lsb->msb
		*val = 0;
		util_ui8 byte, shifts = 0;
		for (util_ui32 i = sizeof(T); i; --i, *val |= (byte << shifts), shifts += 8)
			if (!ReadByte(fp, &byte))
				return false;
		return true;
	}

	///////////////////////////////////////////////////////////////////
	// BUFFER CLASS (INPUT / OUTPUT)	

	class UTILLIB_CLASS OutputBuffer {

		public:
		enum typetag_t { 
			ui8_tag		= 0,
			ui16_tag	= 1,
			i16_tag		= 2,
			ui32_tag	= 3,
			i32_tag		= 4,
			bool_tag	= 5,
			double_tag	= 6,
			string_tag	= 7,
			buffer_tag	= 8
		};

		typedef utriple<util_ui8*, util_ui32, typetag_t>	Packet;
		typedef std::list<Packet>							PacketList;
		
		private:
		PacketList	packets;
		util_ui32	size;

		void			Add (const Packet& p) 
							{ packets.push_back(p); size += p.second; }
		
		template <typename T> struct int_tag_traits {};
		
		//****************************

		public:
		template <typename T>
		void 			WriteInteger (T val) {
							util_ui32 n = SizeInteger<T>();
							Packet packet(DNEWARR(util_ui8, n), n, (typetag_t) int_tag_traits<T>::val);
							ubinaryio::WriteInteger(packet.first, &n, val);
							Add(packet);
						}
							
		void 			WriteByte (util_ui8 byte) {
							Packet packet(DNEWARR(util_ui8, 1), 1, ui8_tag);
							ubinaryio::WriteByte(packet.first, byte);
							Add(packet);
						}

		void 			WriteBool (bool val) {
							Packet packet(DNEWARR(util_ui8, 1), 1, bool_tag);
							ubinaryio::WriteByte(packet.first, val ? 1 : 0);
							Add(packet);
						}

		void 			WriteDouble (double val);
		void 			WriteString (const std::string& s);
		void			WriteBuffer (const void* buffer, util_ui32 size);

		//****************************

		bool			IsEmpty (void) const 
							{ return packets.empty(); }
		util_ui32		GetSize (void) const
							{ return size; }
		util_ui32		GetTotalPackets (void) const
							{ return (util_ui32) packets.size(); }
		const PacketList&
						GetPackets (void) const
							{ return packets; }

		//****************************

		void			Append (const OutputBuffer& buffer);
		void*			MakeBuffer (util_ui32* n) const;
		static void		ClearBuffer (void* data);
		void			Flush (FILE* fp) const;
		void			Clear (void);

		OutputBuffer (void);
		virtual ~OutputBuffer();
	};
	
	template <> struct OutputBuffer::int_tag_traits<util_ui8>	{ enum { val = ui8_tag	}; };
	template <> struct OutputBuffer::int_tag_traits<util_ui16>	{ enum { val = ui16_tag }; };
	template <> struct OutputBuffer::int_tag_traits<util_i16>	{ enum { val = i16_tag	}; };
	template <> struct OutputBuffer::int_tag_traits<util_ui32>	{ enum { val = ui32_tag }; };
	template <> struct OutputBuffer::int_tag_traits<util_i32>	{ enum { val = ui32_tag }; };

	//********************************

	class UTILLIB_CLASS InputBuffer {

		private:
		void*		buffer;
		bool		isBufferCopied;
		util_ui32	size;
		void*		curr;
		util_ui32	remaining;

		bool		Invariant (void) const {
						return	buffer				&& 
								size				&& 
								remaining <= size	&&
								(uptrinside(curr, buffer, POFFS(buffer, size-1)) ||
								 (!remaining && POFFS(buffer, size) == curr));
					}

		public:

		template <typename T>
		bool 		ReadInteger (T* val) {
						DASSERT(Invariant());
						util_ui32 n = SizeInteger<T>();
						if (n > remaining)
							return false;
						else {
							ubinaryio::ReadInteger(curr, val);
							PINC(curr, n);
							remaining -= n;
							return true;
						}
					}

		bool 		ReadByte (util_ui8* byte);
		bool 		ReadBool (bool* val);
		bool 		ReadDouble (double* val);
		bool 		ReadString (std::string& s); 
		bool		ReadBuffer (void* buffer, util_ui32 size);

		void		Set (void* _buffer, util_ui32 _size);
		void		Set (const OutputBuffer& ob);

		void		Rewind (void) 
						{ DASSERT(Invariant()); curr = buffer; remaining = size; }
		bool		IsEof (void) const 
						{ DASSERT(Invariant()); return !remaining; }

		util_ui32	GetSize (void) const 
						{ DASSERT(Invariant()); return size; }
		void*		GetBuffer (void) const
						{ return buffer; }
		util_ui32	GetRemainingBytes (void) const
						{ DASSERT(Invariant()); return remaining; }
		void*		MakeRemainingBuffer (util_ui32* n) const;

		InputBuffer (void* buffer, util_ui32 size, bool copy = true);
		InputBuffer (const OutputBuffer& ob);
		InputBuffer (const InputBuffer& input) 
			{ InputBuffer::InputBuffer(input.buffer, input.size); }
		virtual ~InputBuffer();
	};
}

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
