// DeltaExternId.cpp
// Implementation of extern id class.
// ScriptFigher Project.
// A. Savidis, June 2003.
//

#include "DeltaExternId.h"
#include "ustrings.h"
#include "DeltaExternIdFactory.h"
#include "DeltaVirtualMachine.h"
#include "ContainerDelayedCollector.h"
#include "DeltaObject.h"
#include "udebugsupport.h"

#define	DELTA_EXTERNID_INITIAL_REFCOUNTER	1
#define	DELTA_EXTERNID_DIED_REFCOUNTER		0

////////////////////////////////////////////////////////////

DeltaExternId::ExternIdMaps* DeltaExternId::externIdMaps = (ExternIdMaps*) 0;

void DeltaExternId::SingletonCreate (void) 
	{ unew(externIdMaps); }

void DeltaExternId::SingletonDestroy (void) 
	{ udelete(externIdMaps); }

bool DeltaExternId::IsSingletonCreated (void) 
	{ return !!externIdMaps; }

DeltaExternId* DeltaExternId::Get (const std::string& typeStr, void* val) {
	DASSERT(IsSingletonCreated());
	ExternIdMap&			p = (*DPTR(externIdMaps))[typeStr];
	ExternIdMap::iterator	i = p.find(val);
	return i != p.end() ? i->second : (DeltaExternId*) 0;
}

//******************************

void DeltaExternId::Register (void) {
	DASSERT(IsSingletonCreated() && Invariant());
	(*DPTR(externIdMaps))[typeStr][val] = this;
}

void DeltaExternId::Cancel (void) {

	DASSERT(IsSingletonCreated() && Invariant());

	ExternIdMaps::iterator	i = DPTR(externIdMaps)->find(typeStr);
	DASSERT(i != DPTR(externIdMaps)->end());

	ExternIdMap&			p = i->second;
	ExternIdMap::iterator	j = p.find(val);
	DASSERT(j != p.end());

	p.erase(j);
	if (p.empty())
		DPTR(externIdMaps)->erase(i);
}

////////////////////////////////////////////////////////////

DeltaExternId::DeltaExternId (
		void*							_val,
		DeltaExternIdType				_type,
		ToStringFunc					_toString,
		const std::string&				_typeStr,
		const DeltaExternIdFieldGetter*	_fieldGetter
	):	type			( _type								),
		val				( _val								),
		typeStr			( _typeStr							),
		refCounter		( DELTA_EXTERNID_INITIAL_REFCOUNTER	),
		toString		( _toString							),
		fieldGetter		( _fieldGetter						),
		userData		( (DeltaObject*) 0					) {

	DASSERT(!Get(typeStr, val));	// Uniqueness test.
	Register();	
	CreateUserData();
}

//******************************

DeltaExternId::~DeltaExternId(){ 

	DASSERT(Invariant()); 
	DASSERT(this == Get(typeStr, val));	

	Cancel();			
	
	if (userData) {	// It is forced collection in global cleanup with delayed collection.
		DASSERT(DeltaExternIdFactory::GetSingleton().IsUnderDestruction());
		DASSERT(DeltaExternIdFactory::GetSingleton().JustCallingDestructor(this));
		DASSERT(DELTA_COLLECTOR_HAS_ENTERED());
		unullify(userData);
	}
	else	// Must have been auto collected.
		DASSERT(refCounter == DELTA_EXTERNID_INITIAL_REFCOUNTER);
	refCounter = DELTA_EXTERNID_DIED_REFCOUNTER;
}

////////////////////////////////////////////////////////////

void DeltaExternId::CreateUserData (void) {
	DASSERT(!userData);
	userData = DeltaObject::NewObject();
	DPTR(userData)->SetIsUserDataOfExternId(this);
	DPTR(userData)->GetCreationInfo().SetCreatorInformation(
		DELTA_EXTERNID_USERDATA_DESC,
		DELTA_CREATION_LINE_HIDDEN,
		uconstructstr("%s '%s'", DeltaValue::GetTypeStr(DeltaValue_ExternId), typeStr.c_str())
	);
}

////////////////////////////////////////////////////////////

bool DeltaExternId::Invariant (void) const {
	return	(	type == DeltaExternId_NonCollectable		||
				type == DeltaExternId_CollectableContainer	||
				type == DeltaExternId_CollectableSimple	)	&&
				!typeStr.empty()							&&
				refCounter;
}

////////////////////////////////////////////////////////////

bool DeltaExternId::Equal (const std::string& _typeStr, void* _val) const
	{ return typeStr == _typeStr && val == _val; }

bool DeltaExternId::operator==(const DeltaExternId& p) const {
	DASSERT(Invariant()); 
	if (this == &p) 
		return true;
	else
		{ DASSERT(p.val != val || typeStr != typeStr); return false; }
}

////////////////////////////////////////////////////////////
// In case the wrapped value is collectable (simple ro container)
// we need to refer to it.
//
void DeltaExternId::ReferToCollectable (DeltaValue* owner) {
	if (type == DeltaExternId_CollectableSimple)
		DELTA_EGC_SIMPLE_INCREFCOUNTER(val);
	else
	if (type == DeltaExternId_CollectableContainer)
		DELTA_EGC_CONTAINER_INCREFCOUNTER(val, owner);
}

void DeltaExternId::UnreferToCollectable (DeltaValue* owner) {
	if (type == DeltaExternId_CollectableSimple)
		{ DASSERT(val); DELTA_EGC_SIMPLE_DECREFCOUNTER(val); }
	else
	if (type == DeltaExternId_CollectableContainer)
		{ DASSERT(val); DELTA_EGC_CONTAINER_DECREFCOUNTER(val, owner); }
}

////////////////////////////////////////////////////////////

void DeltaExternId::IncRefCounter (DeltaValue* owner) { 

	DASSERT(Invariant()); 

	++refCounter;

	if (owner) {
		DASSERT(!referrers.in(owner));
		referrers.insert(owner);
		ReferToCollectable(owner);
	}

	DPTR(userData)->IncRefCounter(owner);
}

////////////////////////////////////////////////////////////

void DeltaExternId::DecRefCounter (DeltaValue* owner) {

	DASSERT(Invariant() && refCounter); 

	if (owner) {
		DASSERT(referrers.in(owner));
		referrers.remove(owner);
		UnreferToCollectable(owner);
	}

	// May need to disacossiate userdata from its externid when the latter
	// is collected since userdata may live longer due to absence of referrers.

	bool shouldDeleteExternId = (--refCounter == DELTA_EXTERNID_INITIAL_REFCOUNTER);
	if (shouldDeleteExternId)
		DPTR(userData)->ResetIsUserDataOfExternId(this); 

	// The DecRefCounter may cause reentrancy to this function if extenrid referrers
	// exist in its userdata. Since the extenrid may die by such a reentrant call,
	// just to be safe, no access to externid local data should follow the DecRefCounter,
	// except from the first time it is deleted.

	DPTR(userData)->DecRefCounter(owner);
	if (shouldDeleteExternId)
		{ unullify(userData); DeltaExternIdFactory::Delete(this); }
}

////////////////////////////////////////////////////////////

void DeltaExternId::ConvertToString (DeltaString* at, void* closure) const {

	DPTR(at)->Add(uconstructstr("sn(0x%x)", GetSerialNo()));
	if (closure)
		DPTR(at)->Add((const char*) closure);

	EnterConvertToString(); 
	UTIL_ONFUNC_RETURN(ConvertToString, ExitConvertToString(););
	ConvertToStringHandler(at, &CollectableContainer::ConvertToString); 
}

////////////////////////////////////////////////////////////

void DeltaExternId::ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) const {
	
	DPTR(at)->Add(uconstructstr("sn(0x%x)", GetSerialNo()));
	if (closure)
		DPTR(at)->Add((const char*) closure);

	EnterConvertToStringBySkippingUserCode(); 
	UTIL_ONFUNC_RETURN(ConvertToStringBySkippingUserCode, ExitConvertToStringBySkippingUserCode(););
	ConvertToStringHandler(at, &CollectableContainer::ConvertToStringBySkippingUserCode); 
}

////////////////////////////////////////////////////////////

void DeltaExternId::ConvertToStringHandler (
		DeltaString*					at, 
		void (CollectableContainer::*	converter)(DeltaString*, void*)
	) const {

	DASSERT(Invariant()); 
	
	if (type == DeltaExternId_CollectableContainer)
		(DELTA_EGC_CONTAINER_GETVALUE(val)->*converter)(at, val);
	else
	if (toString) 
		(*toString)(at, val);
	else
		DPTR(at)->Add(
			uconstructstr(
				"%s(0x%x, '%s')", 
				GetExtClassString(), 
				GetSerialNo(), 
				typeStr.c_str()
			)
		);
}

////////////////////////////////////////////////////////////

bool DeltaExternId::IsReservedFieldKey (const std::string& key) {
	return	key == DELTA_EXTERNID_TYPE_INDEX		|| 
			key == DELTA_EXTERNID_USERDATA_INDEX	||
			key == DELTA_EXTERNID_MADEIN_INDEX;
}

void DeltaExternId::GetReservedField (
		const std::string&	key, 
		DeltaValue&			externId, 
		DeltaValue*			at
	) {
	if (key == DELTA_EXTERNID_TYPE_INDEX)
		at->FromString(externId.GetExternIdTypeString());
	else
	if (key == DELTA_EXTERNID_MADEIN_INDEX)
		at->FromString(externId.GetExternId()->GetCreationInfo().GetDesc());
	else
	if (key == DELTA_EXTERNID_USERDATA_INDEX)
		if (DeltaTable* userData = externId.GetExternIdUserData())
			at->FromTable(userData);
		else
			at->FromNil();
	else
		DASSERT(false);
}

////////////////////////////////////////////////////////////

void DeltaExternId::AddReservedFieldKeys (std::list<std::string>* keys) {
	keys->push_back(DELTA_EXTERNID_TYPE_INDEX);
	keys->push_back(DELTA_EXTERNID_MADEIN_INDEX);
	keys->push_back(DELTA_EXTERNID_USERDATA_INDEX);
}

////////////////////////////////////////////////////////////

util_i32 DeltaExternId::toStringCounter = 0;

void DeltaExternId::EnterConvertToString (void) {
	DASSERT(!IsStringConversionInProgress() || !UseConvertToStringBySkippingUserCode());
	++toStringCounter;
}

void DeltaExternId::EnterConvertToStringBySkippingUserCode (void) {
	DASSERT(!IsStringConversionInProgress() || !UseConvertToString());
	--toStringCounter;
}

void DeltaExternId::ExitConvertToString (void) {
	DASSERT(UseConvertToString() && !UseConvertToStringBySkippingUserCode());
	--toStringCounter;
}

void DeltaExternId::ExitConvertToStringBySkippingUserCode (void) {
	DASSERT(!UseConvertToString() && UseConvertToStringBySkippingUserCode());
	++toStringCounter;
}

bool DeltaExternId::UseConvertToStringBySkippingUserCode (void) {
	DASSERT(IsStringConversionInProgress());
	return toStringCounter < 0;
}

bool DeltaExternId::UseConvertToString (void) {
	DASSERT(IsStringConversionInProgress());
	return toStringCounter > 0;
}

bool DeltaExternId::IsStringConversionInProgress (void)
	{ return toStringCounter != 0; }

////////////////////////////////////////////////////////////
