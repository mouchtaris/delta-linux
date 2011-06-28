#include "AddOn.h"



namespace iviews {

//-----------------------------------------------------------------------

AddOn::AddOn(void) {
	static id_t nextId_ = 0;
	id = nextId_++;
}

//-----------------------------------------------------------------------

id_t AddOn::Id (void) const { return id; }

//-----------------------------------------------------------------------

bool AddOn::operator== (const AddOn * addOn) const {
	assert(addOn);
	return id == addOn->Id();
}

//-----------------------------------------------------------------------

bool AddOn::operator!= (const AddOn * addOn) const
	{	return ! (this ==(addOn)); }

}