// DeltaWxWrapper.cpp
// The base class of all wx wrappers.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#include "DeltaWxWrapper.h"
#include "DeltaWxNativeInstanceToWrapperMap.h"
#include "DeltaWxWrapperToExternIdMap.h"
#include "uvalidatable.h"

using namespace std;

namespace wxWidgets {

////////////////////////////////////////////////////////////////

VCLASSID_IMPL(WrapperClassId, "wxInstance")

////////////////////////////////////////////////////////////////

bool Wrapper::IsChild (Wrapper* child) const
{
	util_ui32 childSerialNo	= child->GetSerialNo();
	return	find(children.begin(), children.end(), childSerialNo)
			!= children.end();
}

void Wrapper::AddChild (Wrapper* child)
{
	util_ui32 serialNo	= GetSerialNo();
	if (child) {
		util_ui32 childSerialNo		= child->GetSerialNo();
		Wrapper* oldParent			= child->GetParent();
		util_ui32 oldParentSerialNo	= oldParent ? oldParent->GetSerialNo() : 0;
		if (oldParentSerialNo				&&
			oldParentSerialNo == serialNo	&&
			IsChild(child))
			return;

		if (child == this)
			return;

		if (oldParentSerialNo				&&
			oldParentSerialNo != serialNo)
			oldParent->RemoveChild(child);

		child->SetParent(this);

		if (!IsChild(child))
			children.push_back(childSerialNo);
	}
}

void Wrapper::RemoveChild (Wrapper* child)
{
	if (child && IsChild(child)) {
		DASSERT(child->GetParent() == this);
		children.erase(find(children.begin(), children.end(), child->GetSerialNo()));
	}
}

void Wrapper::SetParent (Wrapper* _parent)
	{ parent		= _parent; }

Wrapper* Wrapper::GetParent (void) const
	{ return parent; }

////////////////////////////////////////////////////////////////

void Wrapper::DeleteChildren (void)
{
	while (!children.empty()) {
		util_ui32 childSerialNo	= children.front();
		Validatable* child		= VALIDATABLE_VGET_INST(childSerialNo);
		DASSERT(child);
		Wrapper* wrapper		= static_cast<Wrapper*>(child->CastTo(child->GetExtClassString()));
		DDELETE(wrapper);
	}
}

////////////////////////////////////////////////////////////////

Wrapper::Wrapper (void) :
	children(), parent(NULL)
	{ }

Wrapper::~Wrapper (void)
{
	DeleteChildren();

	if (parent) {
		DASSERT(parent->IsChild(this));
		parent->RemoveChild(this);
	}

	void* nativeInstance	= GetCastToNativeInstance();
	DASSERT(nativeInstance);
	if (NativeInstanceToWrapperMapGet().HasKey(nativeInstance))
		NativeInstanceToWrapperMapGet().RemoveEntry(nativeInstance);

	util_ui32 serialNo		= GetSerialNo();
	if (WrapperToExternIdMapGet().HasKey(serialNo))
		WrapperToExternIdMapGet().RemoveEntry(serialNo);
}

////////////////////////////////////////////////////////////////

}	// wxWidgets
