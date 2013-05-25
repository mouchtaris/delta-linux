// DeltaWxDefineMap.h
// A singleton that holds all the wxWidgets macro names
// and their value.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef WX_DEFINE_MAP
#define WX_DEFINE_MAP

#include "usingleton.h"
#include <map>
#include <string>

namespace wxWidgets			{

////////////////////////////////////////////////////////////////

	class DefineMap : public std::map<std::string, int> {	// singleton
		private:
		DFRIENDDESTRUCTOR()
		USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(DefineMap)

////////////////////////////////////////////////////////////////

		DefineMap (void);
		virtual ~DefineMap (void);

		void		Initialise (void);
		void		CleanUp (void);

////////////////////////////////////////////////////////////////

		public:
		int			Search (std::string str, bool *found = NULL) const;

////////////////////////////////////////////////////////////////

		USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
		USINGLETON_APISTYLE_DECLARE_GETTER(DefineMap)
	};
	USINGLETON_INLINE_ACCESS_HELPER(DefineMap)

////////////////////////////////////////////////////////////////

}	// wxWidgets

#endif
