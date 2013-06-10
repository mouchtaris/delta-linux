// DeltaWxWrapperToExternIdMap.h
// A singleton that maps a wrapper instance to a
// ExternId's serial number
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef WX_WRAPPER_TO_EXTERNID_MAP
#define WX_WRAPPER_TO_EXTERNID_MAP

#include "usingleton.h"
#include <map>

namespace wxWidgets			{

////////////////////////////////////////////////////////////////

	class WrapperToExternIdMap : public std::map<util_ui32, util_ui32> {	// singleton
		private:
		DFRIENDDESTRUCTOR()
		USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(WrapperToExternIdMap)

////////////////////////////////////////////////////////////////

		WrapperToExternIdMap (void);
		virtual ~WrapperToExternIdMap (void);

////////////////////////////////////////////////////////////////

		public:
		bool		HasKey (util_ui32 key) const;

		void		AddEntry (util_ui32 key, util_ui32 serialNo);

		void		RemoveEntry (util_ui32 key);

		util_ui32	GetValue (util_ui32 key) const;

////////////////////////////////////////////////////////////////

		void		CleanUp (void);

////////////////////////////////////////////////////////////////

		USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
		USINGLETON_APISTYLE_DECLARE_GETTER(WrapperToExternIdMap)
	};
	USINGLETON_INLINE_ACCESS_HELPER(WrapperToExternIdMap)

////////////////////////////////////////////////////////////////

}	// wxWidgets

#endif
