#ifndef DELTAWXAPP_H
#define DELTAWXAPP_H
#include <wx/app.h>
#include <wx/string.h>
#include <list>
#include "DeltaValue.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "wxWrapperUtilFunctions.h"
#include "uvalidatable.h"
#include <iostream>
#include <DeltaWxEvtHandler.h>

////////////////////////////////////////////////////////
// AS, 2010. Wrapper class that can support either an externally created xwApp
// instance or an internally created one (derivative).

class wxAppAdapter  {

	private:
	wxApp*					app;
	bool					isDelegate;
	std::list<DeltaValue>	onInit;
	std::list<DeltaValue>	onExit;
	std::list<DeltaValue>	onRun;
	
	class wxAppDerived : public wxApp {

		private:
		wxAppAdapter* app;

		public:
		virtual bool	OnInit (void) { return wxApp::OnInit() && app->NotifyInit(); }
		virtual int		OnExit (void) { app->NotifyExit(); return wxApp::OnExit(); }
		virtual int		OnRun (void)  { app->NotifyRun(); return wxApp::OnRun(); }

		wxAppDerived (wxAppAdapter* _app) : app(_app){}
		virtual ~wxAppDerived(){}
	};

	////////////////////////////////////////////////////////

	public:
	wxApp*					GetApp (void) { return app; }
	bool					IsDelegate (void) const
								{ return isDelegate; }
	void					AddOnInit (const DeltaValue& f) 
								{ onInit.push_back(f); }
	std::list<DeltaValue>*	GetOnInitFuncs (void) 
								{ return &onInit; }
	virtual bool			OnInit (void) {
								if (IsDelegate())
									return NotifyInit();
								else
									return app->OnInit();
							}
	bool					NotifyInit (void);

	void					AddOnExit (const DeltaValue& f) 
								{ onExit.push_back(f); }
	std::list<DeltaValue>*	GetOnExitFuncs (void) 
								{ return &onExit; }
	virtual int				OnExit (void) {
								if (IsDelegate()) 
									{ NotifyExit(); return true; }
								else
									return app->OnExit();
							}
	void					NotifyExit (void);

	void					AddOnRun(const DeltaValue& f) 
								{ onRun.push_back(f); }
	std::list<DeltaValue>*	GetOnRunFuncs (void) 
								{ return &onRun; }
	virtual int				OnRun (void) {
								if (IsDelegate())
									{ NotifyRun(); return true; }
								else
									return app->OnRun();
							}
	void					NotifyRun();

	// Delegate functions.
	bool				Dispatch()							{ return app->Dispatch(); }
	void				ExitMainLoop()						{ if (!isDelegate) app->ExitMainLoop(); }
	bool				GetExitOnFrameDelete() const		{ return app->GetExitOnFrameDelete(); }
	wxString			GetAppName() const					{ return app->GetAppName(); }
	wxString			GetClassName() const				{ return app->GetClassName(); }
	wxWindow*			GetTopWindow() const				{ return app->GetTopWindow(); }
	bool				GetUseBestVisual() const			{ return app->GetUseBestVisual(); }
	const wxString&		GetVendorName()						{ return app->GetVendorName(); }
	bool				IsActive() const					{ return app->IsActive(); }
	bool				Pending()							{ return app->Pending(); }
	void				SetAppName(const wxString& name)	{ app->SetAppName(name); }
	void				SetClassName(const wxString& name)	{ app->SetClassName(name); }
	void				SetExitOnFrameDelete(bool flag)		{ app->SetExitOnFrameDelete(flag); }
	 void				SetTopWindow(wxWindow *win)			{ app->SetTopWindow(win); }
	 void				SetVendorName(const wxString& name)	{ app->SetVendorName(name); }
	 void				SetUseBestVisual( bool flag, bool forceTrueColour = false )		
							{ app->SetUseBestVisual(flag, forceTrueColour); }
	 bool				Yield(bool onlyIfNeeded)			{ return app->Yield(onlyIfNeeded); }

	wxAppAdapter (void) : app (new wxAppDerived(this)), isDelegate(false)
		{ app->argc = 0,  app->argv = (wxChar**) 0; }

	wxAppAdapter (wxApp* _app) : app (_app), isDelegate(true)
		{}
	virtual ~wxAppAdapter() 
		{ if (!isDelegate) delete app; }
};

VCLASSID_HEADER(DeltaWxAppAdapterClassId)

typedef DeltaNativeClassSingleBaseWrapper<
				DeltaWxAppAdapterClassId,
				DeltaWxEvtHandler,
				wxAppAdapter
		> DeltaWxAppAdapter;

WX_WRAPPER_UTIL_FUNCS_DEFINITION(AppAdapter)

#endif	// Do not add stuff beyond this point.