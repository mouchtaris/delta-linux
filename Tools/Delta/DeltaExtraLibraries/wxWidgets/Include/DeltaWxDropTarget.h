#ifndef DELTAWXDROPTARGET_H
#define DELTAWXDROPTARGET_H
#include <wx/dnd.h>
#include "uvalidatable.h"
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "wxWrapperUtilFunctions.h"

class wxDropTargetDerived : public wxDropTarget
{
private:
	DeltaValue *onData;
	std::list<DeltaValue> onDrop;

public:
	void AddOnData(DeltaValue *f) { onData = f; }
	DeltaValue *GetOnDataFunc() { return onData; }
	virtual wxDragResult OnData (wxCoord x, wxCoord y, wxDragResult def) {
		if (onData)
			return NotifyData(x, y, def);
		return wxDragError;
	}
	wxDragResult NotifyData(wxCoord x, wxCoord y, wxDragResult def);

	void AddOnDrop(const DeltaValue f) { onDrop.push_back(f); }
	std::list<DeltaValue> *GetOnDropFuncs() { return &onDrop; }
	virtual bool OnDrop(wxCoord x, wxCoord y) {
		if (!wxDropTarget::OnDrop(x, y))
			return false;
		return NotifyDrop(x, y);
	}
	bool NotifyDrop(wxCoord x, wxCoord y);

	wxDropTargetDerived(wxDataObject *dataObject = NULL) :
	wxDropTarget(dataObject)
	{
		onData = NULL;
	}
};

VCLASSID_HEADER(DeltaWxDropTargetClassId)

typedef DeltaNativeClassNoBaseWrapper<
					DeltaWxDropTargetClassId,
					wxWidgets::Wrapper,
					wxDropTargetDerived
			> DeltaWxDropTarget;

WX_WRAPPER_UTIL_FUNCS_DEFINITION(DropTarget)

#endif	// Do not add stuff beyond this point.