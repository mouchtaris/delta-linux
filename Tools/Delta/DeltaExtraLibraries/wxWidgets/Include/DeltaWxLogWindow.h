#ifndef DELTAWXLOGWINDOW_H
#define DELTAWXLOGWINDOW_H
#include <wx/log.h>
#include "uvalidatable.h"
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLogChain.h"

class wxLogWindowDerived : public wxLogWindow
{
private:
#if !wxCHECK_VERSION(2, 9, 5)	//OnFrameCreate was removed in 2.9.5
	std::list<DeltaValue> onCreate;
#endif
	std::list<DeltaValue> onClose;
	std::list<DeltaValue> onDelete;

public:
#if !wxCHECK_VERSION(2, 9, 5)	//OnFrameCreate was removed in 2.9.5
	void AddOnCreate(const DeltaValue f) { onCreate.push_back(f); }
	std::list<DeltaValue> *GetOnCreateFuncs() { return &onCreate; }
	virtual void OnFrameCreate (wxFrame *frame) {
		wxLogWindow::OnFrameCreate(frame);
		NotifyCreate(frame);
	}
	void NotifyCreate(wxFrame *frame);
#endif

	void AddOnClose(const DeltaValue f) { onClose.push_back(f); }
	std::list<DeltaValue> *GetOnCloseFuncs() { return &onClose; }
	virtual bool OnFrameClose (wxFrame *frame) {
		if (!wxLogWindow::OnFrameClose(frame))
			return false;
		return NotifyClose(frame);
	}
	bool NotifyClose(wxFrame *frame);

	void AddOnDelete(const DeltaValue f) { onDelete.push_back(f); }
	std::list<DeltaValue> *GetOnDeleteFuncs() { return &onDelete; }
	virtual void OnFrameDelete (wxFrame *frame) {
		wxLogWindow::OnFrameDelete(frame);
		NotifyDelete(frame);
		wxLog::SetActiveTarget(new wxLog());
	}
	void NotifyDelete(wxFrame *frame);

	wxLogWindowDerived(wxWindow *pParent,
                const wxChar *szTitle,
                bool bShow = true,
				bool bPassToOld = true) :
	wxLogWindow(pParent, szTitle, bShow, bPassToOld) { }
};

VCLASSID_HEADER(DeltaWxLogWindowClassId)

typedef DeltaNativeClassSingleBaseWrapper<
				DeltaWxLogWindowClassId,
				DeltaWxLogChain,
				wxLogWindowDerived
		> DeltaWxLogWindow;

WX_WRAPPER_UTIL_FUNCS_DEFINITION(LogWindow)

#endif	// Do not add stuff beyond this point.