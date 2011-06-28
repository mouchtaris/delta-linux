/**
 *	MainWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common.h"
#include "UndefineAllWin32Shit.h"

#include <wx/frame.h>
#include <wx/taskbar.h>
class wxButton;
class wxBoxSizer;

////////////////////////////////////////////////////////////////////////

#define NUM_OPERATIONS 13

////////////////////////////////////////////////////////////////////////

class MainWindow : public wxFrame {
public:
	MainWindow (const String& title);
	~MainWindow (void);

private:
	typedef void (MainWindow::*OpCallback) (void);

	struct OperationEntry {
		OpCallback		operation;
		const String	label;
	};

	DECLARE_EVENT_TABLE();

	void onOpButtonPressed (wxCommandEvent& event);
	void onClose (wxCloseEvent& event);

	//******************************************************************
	// Operations
	//
	void opOpenWorkspace (void);
	void opCloseWorkspace (void);
	void opNewWorkspace (void);
	void opRenameWorkspace (void);
	void opAddProject (void);
	void opRemoveProject (void);
	void opNewProject (void);
	void opRenameProject (void);
	void opAddFile (void);
	void opRemoveFile (void);
	void opNewFile (void);
	void opRenameFile (void);
	void opLoadProfile (void);

	wxButton* m_opButtons[NUM_OPERATIONS];

	static OperationEntry s_operations[NUM_OPERATIONS];
};

////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
