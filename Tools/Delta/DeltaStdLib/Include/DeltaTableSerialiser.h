// DeltaTableSerialiser.h
// Serialiser for tables.
// ScriptFighter Project.
// A. Savidis, November 2009.
//

#ifndef	DELTATABLESERIALISER_H
#define	DELTATABLESERIALISER_H

/////////////////////////////////////////////////////////////////

#include "ubinaryio.h"
#include "DeltaTable.h"
#include <map>

class DVM_CLASS DeltaTableSerialiser {
	
	private:
	enum typetag_t { bool_t = 1, number_t =	2, string_t	= 3, table_t = 4 };	
	typedef std::map<util_ui32, DeltaTable*> Tables;
	Tables tables;

	bool						Serialise (ubinaryio::OutputBuffer* ob, const DeltaTable* t);
	bool						Serialise (ubinaryio::OutputBuffer* ob, const DeltaValue& obj);

	bool						Deserialise (ubinaryio::InputBuffer* ib, DeltaTable* t);
	bool						Deserialise (ubinaryio::InputBuffer* ib, DeltaValue* obj);

	/////////////////////////////////////////////////////////////////

	public:
	ubinaryio::OutputBuffer*	Serialise (const DeltaTable* t);
	void*						Serialise (const DeltaTable* t, util_ui32* size);
	static void					DestroyBuffer (void* data) { DDELARR((util_ui8*) data); }

	DeltaTable*					Deserialise (ubinaryio::InputBuffer* ib);
	DeltaTable*					Deserialise (void* data, util_ui32 size);

	DeltaTableSerialiser(void){}
	~DeltaTableSerialiser(){}
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.