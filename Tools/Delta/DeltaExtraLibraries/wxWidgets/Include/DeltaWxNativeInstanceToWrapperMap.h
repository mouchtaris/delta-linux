// DeltaWxNativeInstanceToWrapperMap.h
// A singleton that maps a wx native instance to a
// Wrapper's serial number
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef WX_NATIVE_INSTANCE_TO_WRAPPER_MAP
#define WX_NATIVE_INSTANCE_TO_WRAPPER_MAP

#include "usingleton.h"
#include <map>
#include <string>

namespace wxWidgets			{

////////////////////////////////////////////////////////////////

	struct WrapperInfo {
		util_ui32		serialNo;
		std::string		classId;

		WrapperInfo
		(
			util_ui32			_serialNo	= 0,
			const std::string&	_classId	= ""
		) :
		serialNo(_serialNo), classId(_classId)
			{ }
	};

////////////////////////////////////////////////////////////////

	class NativeInstanceToWrapperMap : public std::map<void*, WrapperInfo> {	// singleton
		private:
		DFRIENDDESTRUCTOR()
		USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(NativeInstanceToWrapperMap)

////////////////////////////////////////////////////////////////

		NativeInstanceToWrapperMap (void);
		virtual ~NativeInstanceToWrapperMap (void);

////////////////////////////////////////////////////////////////

		public:
		bool				HasKey (void* key) const;

		void				AddEntry (void* key, util_ui32 serialNo, const std::string& classId);

		void				RemoveEntry (void* key);

		util_ui32			GetSerialNo (void* key) const;

		const WrapperInfo&	GetValue (void* key);

////////////////////////////////////////////////////////////////

		void				CleanUp (void);

////////////////////////////////////////////////////////////////

		USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
		USINGLETON_APISTYLE_DECLARE_GETTER(NativeInstanceToWrapperMap)
	};
	USINGLETON_INLINE_ACCESS_HELPER(NativeInstanceToWrapperMap)

////////////////////////////////////////////////////////////////

}	// wxWidgets

#endif
