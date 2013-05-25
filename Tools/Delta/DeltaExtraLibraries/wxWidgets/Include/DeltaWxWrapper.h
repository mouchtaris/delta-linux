// DeltaWxWrapper.h
// The base class of all wx wrappers.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef WX_WIDGETS_WRAPPER
#define WX_WIDGETS_WRAPPER

#include "uvalidatable.h"
#include "DeltaStdLibTemplates.h"
#include <list>

namespace wxWidgets {

////////////////////////////////////////////////////////////////

	VCLASSID_HEADER(WrapperClassId)

////////////////////////////////////////////////////////////////

	class Wrapper : public DeltaNativeClassWrapperSuperclass<wxWidgets::WrapperClassId> {
		private:
		typedef std::list<util_ui32>	ChildrenList;
		// Holds the serial numbers of the wrappers.
		ChildrenList					children;
		Wrapper*						parent;

////////////////////////////////////////////////////////////////

		public:
		bool				IsChild (Wrapper* child) const;

		void				AddChild (Wrapper* child);

		void				RemoveChild (Wrapper* child);

		void				SetParent (Wrapper* _parent);

		Wrapper*			GetParent (void) const;

////////////////////////////////////////////////////////////////

		void				DeleteChildren (void);

////////////////////////////////////////////////////////////////

		Wrapper (void);

		virtual ~Wrapper (void);
	};

////////////////////////////////////////////////////////////////

}	// wxWidgets

#endif
