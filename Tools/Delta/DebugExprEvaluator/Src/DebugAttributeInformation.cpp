// DebugAttributeInformation.cpp
// Holds information about object attributes (the pattern), and
// is stored as an externid in values produced by the expression evaluator.
// ScriptFighter Project.
// A. Savidis, December 2009.
//

#include "DebugAttributeInformation.h"
#include "DeltaByteCodeTypes.h"
#include "DDebug.h"
#include "ufunctors.h"
#include <algorithm>

#define	INFO_VAL(val)	\
			VGET_INST(DebugAttributeInformation, val, ATTR_INFO_TYPE_STR)

///////////////////////////////////////////////////////////////

ubag<Validatable*>*			DebugAttributeInformation::insts		= (ubag<Validatable*>*) 0;
DeltaExternIdFieldGetter*	DebugAttributeInformation::fieldGetter	= (DeltaExternIdFieldGetter*) 0;

bool DebugAttributeInformation::IsSingletonCreated(void)
	{ return insts && fieldGetter; }

void DebugAttributeInformation::SingletonCreate (void) {
	DASSERT(!IsSingletonCreated()); 
	unew(insts); 
	unew(fieldGetter);
	DPTR(fieldGetter)->SetIncludeReservedKeys(false);	// Hide standard keys.
	DPTR(fieldGetter)->SetGetByString(fieldGetters, uarraysize(fieldGetters));
	DPTR(fieldGetter)->SetKeyToGetKeys("keys");
}

void DebugAttributeInformation::SingletonDestroy (void) {
	DASSERT(IsSingletonCreated());
	DASSERT(DPTR(insts)->empty());
	udelete(insts); 
	udelete(fieldGetter);
}

void DebugAttributeInformation::CleanUp (void) {
	DASSERT(IsSingletonCreated());
	if (!DPTR(insts)->empty()) {
		ubag<Validatable*> copied(*DPTR(insts)); // Necessary, entries auto-remove from 'insts' on destruction.
		std::for_each(
			copied.begin(),
			copied.end(),
			udestroy_first< std::pair<Validatable*, bool> >()
		);
		DASSERT(DPTR(insts)->empty());	// Must have been auto-removed.
	}
}

///////////////////////////////////////////////////////////////

DebugAttributeInformation::DebugAttributeInformation (const DeltaTable::Attribute* _attr) : attr(_attr) {
	DASSERT(IsSingletonCreated());
	DASSERT(DPTR(insts)->find(this) == DPTR(insts)->end());
	DPTR(insts)->insert(this);
}

DebugAttributeInformation::~DebugAttributeInformation() {
	DASSERT(IsSingletonCreated());
	ubag<Validatable*>::iterator i = DPTR(insts)->find(this);
	DASSERT( i != DPTR(insts)->end());
	DPTR(insts)->remove(i);
}

///////////////////////////////////////////////////////////////

void DebugAttributeInformation::ToString (DeltaString* at, void* val) {

	DASSERT(IsSingletonCreated());
	DebugAttributeInformation* info = INFO_VAL(val);
	DASSERT(info && DPTR(insts)->find(info) != DPTR(insts)->end());

	const DeltaTable::Attribute* attr = DPTR(info)->attr;

	DeltaValue value;
	DPTR(attr)->GetContentBySkippingUserCode(&value);
	
	DPTR(at)->Add(
			uconstructstr(
				"@%s(value '%s', set '%s', get '%s')", 
				DPTR(attr)->GetId().c_str(),
				value.ConvertToStringBySkippingUserCode().c_str(),
				DPTR(attr)->GetSetter().ConvertToStringBySkippingUserCode().c_str(),
				DPTR(attr)->GetGetter().ConvertToStringBySkippingUserCode().c_str()
			)
		);
}

///////////////////////////////////////////////////////////////

bool DebugAttributeInformation::GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(fieldGetter)->GetAllKeys(val, at); }

bool DebugAttributeInformation::GetValue (void* val, DeltaValue* at) 
	{  DPTR(INFO_VAL(val)->GetAttribute())->GetContentBySkippingUserCode(at); return true; }

bool DebugAttributeInformation::GetSetter (void* val, DeltaValue* at) 
	{  at->Assign(DPTR(INFO_VAL(val)->GetAttribute())->GetSetter()); return true; }

bool DebugAttributeInformation::GetGetter (void* val, DeltaValue* at) 
	{  at->Assign(DPTR(INFO_VAL(val)->GetAttribute())->GetGetter()); return true; }

DeltaExternIdFieldGetter::GetByStringFuncEntry DebugAttributeInformation::fieldGetters[4] = {
	{ "keys",	&DebugAttributeInformation::GetKeys,	DELTA_GETBYSTRING_NO_PRECOND },
	{ "value",	&DebugAttributeInformation::GetValue,	DELTA_GETBYSTRING_NO_PRECOND },
	{ "@set",	&DebugAttributeInformation::GetSetter,	DELTA_GETBYSTRING_NO_PRECOND },
	{ "@get",	&DebugAttributeInformation::GetGetter,	DELTA_GETBYSTRING_NO_PRECOND }
};

void DebugAttributeInformation::ConvertToDelta (const DeltaTable::Attribute* attr, DeltaValue* at){
	DASSERT(IsSingletonCreated());
	DebugAttributeInformation* info = DNEWCLASS(DebugAttributeInformation, (attr));
	at->FromExternId(
		(void*) DPTR(info)->GetSerialNo(),
		DeltaExternId_NonCollectable,
		ToString,
		ATTR_INFO_TYPE_STR,
		fieldGetter
	); 
}

///////////////////////////////////////////////////////////////
