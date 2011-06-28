// uvalidatable.cpp
// Inside the ScriptFighter Project.
// A. Savidis, December 2005.
// Many extensions after that to support better
// validation.
//

#include "uvalidatable.h"

/////////////////////////////////////////////////////////////////

util_ui32 Validatable::currSerialNo = VALIDATABLE_STARTING_SERIALNO;

Validatable::Validatable (Validatable* p) { 
	serialNo		= currSerialNo++; 
	ownerSerialNo	= (owner = p) ? p->GetSerialNo() : 0;
	ValidatableHandler::Register(this);
}

Validatable::~Validatable() {

	DASSERT(Invariant());
	Validate();

	// Invalidate all serial no pointers with the same serial no.

	if (serialNoStack.size())
		for (SerialNoPtrStack::iterator i = serialNoStack.begin(); i != serialNoStack.end(); ++i)
			if (**i == GetSerialNo())
				**i = VALIDATABLE_CANTHAVE_SERIALNO;

	ValidatableHandler::Cancel(this);
	serialNo = VALIDATABLE_CANTHAVE_SERIALNO;
}

/////////////////////////////////////////////////////////////////

bool Validatable::Invariant (void) const {
	return	serialNo != VALIDATABLE_CANTHAVE_SERIALNO	&&
			serialNo < currSerialNo						&&
			(!owner || owner->Invariant());
}

/////////////////////////////////////////////////////////////////

bool Validatable::IsValid (void) const
	{ return IsValidInstance(this) && (!owner || owner->IsValid()); }

bool Validatable::IsValidInstance (const Validatable* obj) {
	DASSERT(obj->Invariant()); 
	return DPTR(ValidatableHandler::validator)->Validate(obj); 
}

/////////////////////////////////////////////////////////////////

bool							ValidatableHandler::singletonCreated	= false;
uvalidator*						ValidatableHandler::validator			= (uvalidator*) 0;
ValidatableHandler::InverseMap*	ValidatableHandler::inverseValidators[UVALIDATOR_HASH_SIZE];

/////////////////////////////////////////////////////////////////

void ValidatableHandler::SingletonCreate (void) {

	DASSERT(!IsSingletonCreated());

	singletonCreated = true;
	unew(validator);

	for (util_ui16 i = 0; i < UVALIDATOR_HASH_SIZE; ++i)
		inverseValidators[i] = DNEW(InverseMap);
}

/////////////////////////////////////////////////////////////////

void ValidatableHandler::SingletonDestroy (void) {

	DASSERT(IsSingletonCreated());

	udelete(validator);

	for (util_ui16 i = 0; i < UVALIDATOR_HASH_SIZE; ++i)
		udelete(inverseValidators[i]);

	singletonCreated = false;
}

/////////////////////////////////////////////////////////////////

util_ui32 ValidatableHandler::Hash (util_ui32 serialNo)
	{ return  serialNo% UVALIDATOR_HASH_SIZE; }

util_ui32 ValidatableHandler::Hash (Validatable* obj) 
	{ return Hash(obj->GetSerialNo()); }

void ValidatableHandler::Register (Validatable* obj) {

	DASSERT(IsSingletonCreated());
	DASSERT(!Get(obj->GetSerialNo()));

	DPTR(validator)->Register(obj);
	(*DPTR(inverseValidators[Hash(obj)]))[obj->GetSerialNo()] = obj;
}

/////////////////////////////////////////////////////////////////

void ValidatableHandler::Cancel (Validatable* obj) {

	DASSERT(IsSingletonCreated());

	DPTR(validator)->Cancel(obj);

	util_ui16 i = Hash(obj);
	std::map<util_ui32, Validatable*>::iterator j;
	j = DPTR(inverseValidators[i])->find(obj->GetSerialNo());
	DASSERT(j != DPTR(inverseValidators[i])->end());

	DPTR(inverseValidators[i])->erase(j);
}

/////////////////////////////////////////////////////////////////

Validatable* ValidatableHandler::Get (util_ui32 serialNo) {

	DASSERT(IsSingletonCreated());

	util_ui16 i = Hash(serialNo);
	InverseMap::iterator j = DPTR(inverseValidators[i])->find(serialNo);

	if (j != DPTR(inverseValidators[i])->end() && 
		(!j->second->GetOwner() || Get(j->second->GetOwnerSerialNo())))
		return j->second;
	else
		return (Validatable*) 0;
}

/////////////////////////////////////////////////////////////////

bool ValidatableHandler::Validate (const Validatable* obj, util_ui32 serialNo) {
	DASSERT(IsSingletonCreated());
	return	DPTR(validator)->Validate(obj)	&& 
			obj == Get(serialNo)			&&
			(!obj->GetOwner() || Validate(obj->GetOwner(), obj->GetOwnerSerialNo()));
}

/////////////////////////////////////////////////////////////////
