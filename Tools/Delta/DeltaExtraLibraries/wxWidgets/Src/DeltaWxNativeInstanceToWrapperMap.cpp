// DeltaWxNativeInstanceToWrapperMap.cpp
// A singleton that maps a wx native instance to a
// Wrapper's serial number
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#include "DeltaWxNativeInstanceToWrapperMap.h"
#include "DeltaWxWrapper.h"
#include "uvalidatable.h"

using namespace std;

namespace wxWidgets			{

////////////////////////////////////////////////////////////////

USINGLETON_APISTYLE_DEFINE_PRIVATEINSTANCE(NativeInstanceToWrapperMap)
USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(NativeInstanceToWrapperMap)
USINGLETON_APISTYLE_IMPL_GETTER(NativeInstanceToWrapperMap)

////////////////////////////////////////////////////////////////

NativeInstanceToWrapperMap::NativeInstanceToWrapperMap (void)
	{ }

NativeInstanceToWrapperMap::~NativeInstanceToWrapperMap (void)
	{ }

////////////////////////////////////////////////////////////////

void NativeInstanceToWrapperMap::CleanUp (void)
{
// Kill all Wrappers in clean up
	while (!empty()) {
		iterator i					= begin();
		util_ui32 wrapperSerialNo	= i->second.serialNo;
		string wrapperClassId		= i->second.classId;
		Wrapper* wrapper			= VGET_INST(Wrapper, wrapperSerialNo, wrapperClassId);
		if (wrapper && DPTR(wrapper)->IsValid())
			DDELETE(wrapper);
	}

	clear();
}

////////////////////////////////////////////////////////////////

bool NativeInstanceToWrapperMap::HasKey (void* key) const
	{ return find(key) != end(); }

void NativeInstanceToWrapperMap::AddEntry (void* key, util_ui32 serialNo, const std::string& classId)
{
	DASSERT(!HasKey(key));
	WrapperInfo info(serialNo, classId);
	insert(value_type(key, info));
}

void NativeInstanceToWrapperMap::RemoveEntry (void* key)
{
	DASSERT(HasKey(key));
	erase(key);
}

util_ui32 NativeInstanceToWrapperMap::GetSerialNo (void* key) const
{
	DASSERT(HasKey(key));
	return find(key)->second.serialNo;
}

const WrapperInfo& NativeInstanceToWrapperMap::GetValue (void* key)
{
	DASSERT(HasKey(key));
	return find(key)->second;
}

////////////////////////////////////////////////////////////////

}	// wxWidgets
