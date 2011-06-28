// DeltaTableSerialiser.cpp
// Serialiser for tables.
// ScriptFighter Project.
// A. Savidis, November 2009.
//

#include "DeltaTableSerialiser.h"
#include "DeltaObject.h"
#include "uerrorclass.h"

#define	ERROR_HANDLER(what, errclass)				\
	uerror::GetSingleton().post##errclass(			\
		"Serialiser: error writing %s!", what		\
	); goto FAIL;									\

/////////////////////////////////////////////////////////////////

bool DeltaTableSerialiser::Serialise (ubinaryio::OutputBuffer* ob, const DeltaTable* t) {

	DPTR(ob)->WriteInteger((util_ui8) table_t);
	DPTR(ob)->WriteInteger(DPTR(t)->GetSerialNo());
	DPTR(ob)->WriteInteger(DPTR(t)->Total());

	for (DeltaTable::const_iterator i = t->begin(); i != t->end(); ++i) {

		DeltaValue obj;
		i->GetIndex(&obj);

		UCHECK_DOMINO_ERROR(
			Serialise(ob, obj), 
			uconstructstr(
				"index of '%s' type for %s" DELTA_OBJECT_SN_FORMAT, 
				obj.TypeStr(),
				t->GetExtClassString(), 
				t->GetSerialNo()
			)
		);

		i->GetContent(&obj);
		UCHECK_DOMINO_ERROR(
			Serialise(ob, obj), 
			uconstructstr(
				"content of '%s' type for %s" DELTA_OBJECT_SN_FORMAT, 
				obj.TypeStr(),
				t->GetExtClassString(), 
				t->GetSerialNo()
			)
		);
	}

	return true;
	FAIL: return false;
}

//*****************************

bool DeltaTableSerialiser::Serialise (ubinaryio::OutputBuffer* ob, const DeltaValue& obj) {

	switch (obj.Type()) {

		case DeltaValue_Bool		: 
			{ DPTR(ob)->WriteInteger((util_ui8) bool_t);	DPTR(ob)->WriteBool(obj.ToBool());		return true; }

		case DeltaValue_Number		: 
			{ DPTR(ob)->WriteInteger((util_ui8) number_t);	DPTR(ob)->WriteDouble(obj.ToNumber());	return true; }

		case DeltaValue_String		: 
			{ DPTR(ob)->WriteInteger((util_ui8) string_t);	DPTR(ob)->WriteString(obj.ToString());	return true; }

		case DeltaValue_Table		: 
			{ Serialise(ob, const_cast<DeltaValue&>(obj).ToTable());	return true; }

		default:  {
			UCHECK_PRIMARY_ERROR(false, uconstructstr("'%s' item (not serialisable)", obj.TypeStr()));
			FAIL: return false;
		}
	}
}

//*****************************

ubinaryio::OutputBuffer* DeltaTableSerialiser::Serialise (const DeltaTable* t) {
	ubinaryio::OutputBuffer* ob = DNEW(ubinaryio::OutputBuffer);
	if (!Serialise(ob, t))
		udelete(ob);
	return ob;
}

//*****************************

void* DeltaTableSerialiser::Serialise (const DeltaTable* t, util_ui32* size) {
	ubinaryio::OutputBuffer* ob = Serialise(t);
	if (ob) {
		void* data = ob->MakeBuffer(size);
		DDELETE(ob);
		return data;
	}
	else
		{ *size = 0; return (void*) 0; }
}


/////////////////////////////////////////////////////////////////

#undef	ERROR_HANDLER
#define	ERROR_HANDLER(what, errclass)				\
	uerror::GetSingleton().post##errclass(			\
		"Deserialiser: error reading '%s'!", what	\
	); goto FAIL;									\


//*****************************

bool DeltaTableSerialiser::Deserialise (ubinaryio::InputBuffer* ib, DeltaTable* t) {

	util_ui32 total;
	UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadInteger(&total), "total elements");

	while (total--) {
		DeltaValue index, content;
		UCHECK_PRIMARY_ERROR(Deserialise(ib, &index), "index");
		UCHECK_PRIMARY_ERROR(Deserialise(ib, &content), "content");
		t->Set(index, content);
	}

	return true;
	FAIL: return false;
}

//*****************************

bool DeltaTableSerialiser::Deserialise (ubinaryio::InputBuffer* ib, DeltaValue* obj) {

	util_ui8 type;	
	UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadByte(&type), "type tag");
	
	switch (type) {

		case bool_t: {
			bool val; 
			UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadBool(&val),		"bool value");
			obj->FromBool(val);
			break;
		}

		case number_t: {
			double val; 
			UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadDouble(&val),	"number value");
			obj->FromNumber(val);
			break;
		}

		case string_t: {
			std::string val; 
			UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadString(val),		"string value");
			obj->FromString(val);
			break;
		}

		case table_t: {
			util_ui32 val; 
			UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadInteger(&val),	"serial no value");

			DeltaTable* t = (DeltaTable*) 0;
			Tables::iterator i = tables.find(val);

			if (i != tables.end())
				t = i->second;
			else
				tables[val] = t = DeltaObject::NewObject();
		
			obj->FromTable(t);
			UCHECK_DOMINO_ERROR(Deserialise(ib, t), "table");
			break;
		}

		default: UPRIMARYERROR("invalid type tag");
	}

	return true;
	FAIL: return false;
}

//*****************************

DeltaTable* DeltaTableSerialiser::Deserialise (ubinaryio::InputBuffer* ib) {

	DeltaTable* t = DeltaObject::NativeCodeHelpers::NewObject();
	
	if (DPTR(ib)->GetRemainingBytes()) {

		util_ui8 type;
		util_ui32 serialNo;
		
		UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadByte(&type), "type tag");
		UCHECK_PRIMARY_ERROR(DPTR(ib)->ReadInteger(&serialNo), "serial no");
		tables[serialNo] = t;

		UCHECK_DOMINO_ERROR(Deserialise(ib, t), "deserialising table");
	}

	return t;
	FAIL: DeltaObject::NativeCodeHelpers::Delete(t); return (DeltaTable*) 0;
}

//*****************************

DeltaTable* DeltaTableSerialiser::Deserialise (void* data, util_ui32 size) {
	ubinaryio::InputBuffer* ib = DNEWCLASS(ubinaryio::InputBuffer, (data, size, false));
	DeltaTable* t = Deserialise(ib);
	DDELETE(ib);
	return t;
}

/////////////////////////////////////////////////////////////////
