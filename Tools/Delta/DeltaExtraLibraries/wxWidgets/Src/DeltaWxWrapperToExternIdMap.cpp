// DeltaWxWrapperToExternIdMap.cpp
// A singleton that maps a wrapper instance to a
// ExternId's serial number
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#include "DeltaWxWrapperToExternIdMap.h"
#include "DeltaValue.h"

namespace wxWidgets			{

////////////////////////////////////////////////////////////////

USINGLETON_APISTYLE_DEFINE_PRIVATEINSTANCE(WrapperToExternIdMap)
USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(WrapperToExternIdMap)
USINGLETON_APISTYLE_IMPL_GETTER(WrapperToExternIdMap)

////////////////////////////////////////////////////////////////

WrapperToExternIdMap::WrapperToExternIdMap (void)
	{ }

WrapperToExternIdMap::~WrapperToExternIdMap (void)
	{ }

////////////////////////////////////////////////////////////////

void WrapperToExternIdMap::CleanUp (void)
{
	// Kill all collectables in clean up, because
	// their destructors are invalid when the
	// library is not loaded
	while (!empty()) {
		iterator i					= begin();
		util_ui32 wrapperSerialNo	= i->first;
		util_ui32 externIdSerialNo	= i->second;
		DeltaValue externId;
		externId.FromExternIdBySerialNo(externIdSerialNo);
		ubag<DeltaValue*> referrers	= externId.GetExternId()->GetReferrers();
		for (ubag<DeltaValue*>::iterator i = referrers.begin(); i != referrers.end(); ++i) {
			DeltaValue* val			= i->first;
			val->Undefine();
		}
	}

	clear();
}

////////////////////////////////////////////////////////////////

bool WrapperToExternIdMap::HasKey (util_ui32 key) const
	{ return find(key) != end(); }

void WrapperToExternIdMap::AddEntry (util_ui32 key, util_ui32 serialNo)
{
	DASSERT(!HasKey(key));
	insert(value_type(key, serialNo));
}

void WrapperToExternIdMap::RemoveEntry (util_ui32 key)
{
	DASSERT(HasKey(key));
	erase(key);
}

util_ui32 WrapperToExternIdMap::GetValue (util_ui32 key) const
{
	DASSERT(HasKey(key));
	return find(key)->second;
}

////////////////////////////////////////////////////////////////

}	// wxWidgets
