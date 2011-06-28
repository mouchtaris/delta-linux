#include "ColourAddOnManager.h"

#include <assert.h>
#include <algorithm>
#include <functional>
#include "Functional.h"

namespace iviews {

static bool ReturnAlwaysFalse(AddOnApplication *)
	{ return false; }	


bool ColourAddOnManager::AddOrRemoveConditionalAddOn(
		AddOn * addOn, 
		ColourAddOnData * data,
		LayersRenderingInfo * layers
	) {

	assert(addOn && data && layers);
	bool result		= false;
	bool deleteData = true;

	if (addOn->CanRemoveFromManager(data)) {
		RemoveFirst(addOn, layers);
		addOn->RemovedFromManager();
		result = true;
	}


	if (addOn->CanAddInManager(data)) {
		InsertAndApplyAddOn(addOn, data, layers);
		result		= true;
		deleteData	= false;
	}
	
	if (deleteData){
		delete data;
		data = (ColourAddOnData *)0;
	}
	return result;
}

//-----------------------------------------------------------------------

void ColourAddOnManager::Apply(LayersRenderingInfo * layers) 
	{ Apply(addOns.begin(), addOns.end(), layers); }


//-----------------------------------------------------------------------

void ColourAddOnManager::Exclude(LayersRenderingInfo * layers) {
	Undo(layers, ReturnAlwaysFalse);
	Apply(addOns.begin(), addOns.end(), layers);
}

//-----------------------------------------------------------------------

void ColourAddOnManager::RestoreColours(LayersRenderingInfo * layers){
	Undo(layers, ReturnAlwaysFalse);
	Clear();
}

//-----------------------------------------------------------------------

bool ColourAddOnManager::RemoveAll(const AddOn * addOn, LayersRenderingInfo * layers) {
	assert(addOn && layers);

	if (std::find(addOns.begin(), addOns.end(), *addOn) == addOns.end())
		return false;
	
	Undo(layers, ReturnAlwaysFalse);
	utility::RemovePointerFromList(addOns, AddOnApplication(const_cast<AddOn *>(addOn)));

	Apply(addOns.begin(), addOns.end(), layers);
	return true;
}

//-----------------------------------------------------------------------

bool ColourAddOnManager::RemoveFirst(const AddOn * addOn, LayersRenderingInfo * layers){
	assert(addOn && layers);


	if (std::find(addOns.begin(), addOns.end(), *addOn) == addOns.end())
		return false;

	AddOnApplicationPtrList::iterator 
	undoEnd = Undo(
				layers, 
				bind2nd(
					std::mem_fun(
						//me auto ton tropo dialegw poion operator 8a parw
						(bool (AddOnApplication::*)(const AddOn * addOn) const)&AddOnApplication::operator ==
					),
					addOn
				)
			);

	delete (*undoEnd);
	(*undoEnd) = (AddOnApplication *)0;

	AddOnApplicationPtrList::iterator beginOfApply = undoEnd;
	beginOfApply++;
	addOns.erase(undoEnd);
	Apply(beginOfApply, addOns.end(), layers);
	return true;
}

//-----------------------------------------------------------------------

void ColourAddOnManager::Clear(void) {
	std::for_each(
		addOns.begin(),
		addOns.end(),
		utility::DeletePointersFromList<AddOnApplication>()
	);
	addOns.clear();
}

//-----------------------------------------------------------------------

void ColourAddOnManager::InsertAndApplyAddOn(
		AddOn * addOn, 
		ColourAddOnData * data,
		LayersRenderingInfo * layers
	) {
	assert(addOn && data && layers);

	addOns.push_back(		//insert addOn
		new AddOnApplication(
			addOn,
			data
		)
	);

	//Apply addOn
	addOns.back()->undoData = addOn->Apply(layers, data);
}

//-----------------------------------------------------------------------

void ColourAddOnManager::Apply(
		AddOnApplicationPtrList::iterator begin, 
		AddOnApplicationPtrList::iterator end,
		LayersRenderingInfo * layers
	) {
	for (AddOnApplicationPtrList::iterator i = begin; i != end; ++i) {
		if ((*i)->undoData) {
			delete (*i)->undoData;
			(*i)->undoData = (ColourAddOnUndoData *)0;
		}
		(*i)->undoData = (*i)->addOn->Apply(layers, (*i)->data);
	}
}











//-----------------------------------------------------------------------

AddOnApplication::~AddOnApplication(){
	if (data) {
		delete data;
		data = (ColourAddOnData *)0;
	}

	if (undoData) {
		delete undoData;
		undoData = (ColourAddOnUndoData *)0;
	}
}

//-----------------------------------------------------------------------

AddOnApplication::AddOnApplication(
	AddOn * addOn_, 
	ColourAddOnData	* data_, 
	ColourAddOnUndoData * undoData_
) :	addOn(addOn_),
	data(data_),
	undoData(undoData_)
{}

//-----------------------------------------------------------------------

bool AddOnApplication::operator== (const AddOn * addOn_) const {
	assert(addOn);
	return addOn == addOn_;
}

//-----------------------------------------------------------------------

bool AddOnApplication::operator!= (const AddOn * addOn) const 
	{ return !(*this == addOn); }

//-----------------------------------------------------------------------

bool AddOnApplication::operator== (const AddOnApplication & a) const
	{ return addOn == a.addOn; }

//-----------------------------------------------------------------------

bool AddOnApplication::operator!= (const AddOnApplication & a) const
	{ return !(*this == a); }


//-----------------------------------------------------------------------

bool operator== (const AddOnApplication * app, const AddOn & addOn) {
	assert(app && app->addOn);
	return app->addOn == &addOn;
}

//-----------------------------------------------------------------------

bool operator== (const AddOnApplication * app1, const AddOnApplication & app2) {
	assert(app1 && app1->addOn);
	return app1->addOn == app2.addOn;
}

}	//namespace iviews 