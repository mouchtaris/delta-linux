/**
 *	EditorWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "EditorWindow.h"
#include "EditorModuleLoader.h"
#include "EditorInterfaces/LanguageModuleIface.h"
#include "Scintilla.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#include <wx/accel.h>
#include <wx/file.h>

#include "Icons/AutoCompleteFunction.xpm"
#include "Icons/AutoCompleteAttr.xpm"
#include "Icons/AutoCompleteIndex.xpm"
#include "Icons/AutoCompleteGlobalVar.xpm"
#include "Icons/AutoCompleteConst.xpm"
#include "Icons/AutoCompleteNamespace.xpm"
#include "Icons/round.xpm"
#include "Icons/round-blue.xpm"
#include "Icons/round-red.xpm"
#include "Icons/noidea.xpm"

#include "Icons/marker_rounded_blue.xpm"
#include "Icons/marker_red_round.xpm"
#include "Icons/marker_red_round_empty.xpm"
#include "Icons/marker_yellow_arrow.xpm"
#include "Icons/marker_green_arrow.xpm"
#include "Icons/marker_error.xpm"

namespace editor {

enum MarkerTypes {
	MarkerRoundedBlue		= 0,
	MarkerRoundedRed		= 1,
	MarkerRoundedRedEmpty	= 2,
	MarkerArrowYellow		= 3,
	MarkerArrowGreen		= 4,
	MarkerError				= 5
};

////////////////////////////////////////////////////////////////////////
// EditorWindow class
//
BEGIN_EVENT_TABLE(EditorWindow, wxScintilla)
	EVT_SCI_SAVEPOINTREACHED(wxID_ANY, EditorWindow::onSavePointReached)
	EVT_SCI_SAVEPOINTLEFT(wxID_ANY, EditorWindow::onLeftSavePoint)
	EVT_SCI_AUTOCOMP_SELECTION(wxID_ANY, EditorWindow::onAutoCompleteSelection)
	EVT_KILL_FOCUS(EditorWindow::onKillFocus)
END_EVENT_TABLE();


//**********************************************************************

EditorWindow::EditorWindow(void) : m_otherView(0), m_langIface(0),
	m_yellowArrowLine(0), m_greenArrowLine(0), m_modified(false), m_internalSaves(0)
{
}

//**********************************************************************

EditorWindow::EditorWindow (wxWindow* parent) : m_otherView(0), m_langIface(0),	m_yellowArrowLine(0),
	m_greenArrowLine(0), m_internalSaves(0), m_modified(false), m_duringReload(false)
{
	this->Create(parent);

}

//**********************************************************************

EditorWindow::~EditorWindow(void)
{
	if (m_otherView)
		m_otherView->SetDocPointer(0);

	this->UnLoadLanguageModule();
}

//**********************************************************************

bool EditorWindow::Create (wxWindow* parent)
{
	if (
		!this->wxScintilla::Create(
			parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxVSCROLL)
	) {
		return false;
	}
	m_langIface = EditorModuleLoader::GetLanguageModule();
	m_langIface->SetEditor(this);

	this->RegisterImage(0, auto_complete_function_xpm);
	this->RegisterImage(1, round_blue_xpm);					// Function argument
	this->RegisterImage(2, auto_complete_index_xpm);		
	this->RegisterImage(3, auto_complete_global_var_xpm);	// Variable
	this->RegisterImage(4, auto_complete_attr_xpm);			
	this->RegisterImage(5, noidea_xpm);					
	this->RegisterImage(6, auto_complete_namespace_xpm);
	this->RegisterImage(7, auto_complete_const_xpm);

	this->MarkerDefineBitmap(MarkerRoundedBlue, marker_rounded_blue_xpm);
	this->MarkerDefineBitmap(MarkerRoundedRed, marker_red_round_xpm);
	this->MarkerDefineBitmap(MarkerRoundedRedEmpty, marker_red_round_empty_xpm);
	this->MarkerDefineBitmap(MarkerArrowYellow, marker_yellow_arrow_xpm);
	this->MarkerDefineBitmap(MarkerArrowGreen, marker_green_arrow_xpm);
	this->MarkerDefineBitmap(MarkerError, marker_error_xpm);

	// this->CmdKeyClearAll();
	// this->UsePopUp(false);
	this->CmdKeyAssign('Z', wxSCI_SCMOD_CTRL | wxSCI_SCMOD_SHIFT, wxSCI_CMD_REDO);

	////////////////////////////////////////////////////////////////////
	// Setup folding margin style
	//
	this->SetFoldFlags(16);
	this->SetMarginMask(MARGIN_FOLDING, wxSCI_MASK_FOLDERS);

	this->MarkerDefine(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS);
	this->MarkerDefine(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS);
	this->MarkerDefine(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE);
	this->MarkerDefine(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNER);

	this->MarkerDefine(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUSCONNECTED);
	this->MarkerDefine(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED);
	this->MarkerDefine(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER);

	this->SetMarginSensitive(MARGIN_LINENUMBER, true);
	this->SetMarginSensitive(MARGIN_EDITORMARKS, true);
	this->SetMarginSensitive(MARGIN_FOLDING, true);
	this->SetFoldMarginColour(true, *wxWHITE);

	wxColour gray(150, 150, 150);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDER, gray);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDEROPEN, gray);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDERSUB, gray);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDERTAIL, gray);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDEREND, gray);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDEROPENMID, gray);
	this->MarkerSetForeground(wxSCI_MARKNUM_FOLDERMIDTAIL, gray);

	////////////////////////////////////////////////////////////////////
	// Indicators & styling
	//
	this->IndicatorSetStyle(0, wxSCI_INDIC_SQUIGGLE);
	this->IndicatorSetStyle(1, wxSCI_INDIC_TT);
	this->IndicatorSetStyle(2, wxSCI_INDIC_BOX);

	this->IndicatorSetForeground(0, wxColour(170, 0, 0));
	this->IndicatorSetForeground(1, wxColour(30, 170, 100));
	this->IndicatorSetForeground(2, wxColour(220, 220, 220));

	////////////////////////////////////////////////////////////////////
	// Identation
	//
	this->SetTabIndents(true);
	this->SetBackSpaceUnIndents(true);

	////////////////////////////////////////////////////////////////////
	// Auto completion
	//
	this->AutoCompSetIgnoreCase(true);
	this->AutoCompSetChooseSingle(false);
	this->AutoCompSetSeparator('\a');
	this->AutoCompSetMaxWidth(0);
	this->AutoCompSetCancelAtStart(true);

	////////////////////////////////////////////////////////////////////
	// Other params
	//
	this->SetMouseDwellTime(1000);

	return true;
}

//**********************************************************************

wxScintilla* EditorWindow::CreateOtherView (wxWindow* parent)
{
	if (m_otherView)
		m_otherView->Reparent(parent);
	else {
		m_otherView = new wxScintilla(parent, wxID_ANY, wxDefaultPosition,
			wxDefaultSize, wxNO_BORDER | wxVSCROLL);

		m_otherView->SetDocPointer(this->GetDocPointer());
	}
	return m_otherView;
}

//**********************************************************************

void EditorWindow::DestroyOtherView (void)
{
	if (m_otherView) {
		m_otherView->Destroy();
		m_otherView = 0;
	}
}

//**********************************************************************

bool EditorWindow::Open (const String& uri)
{
	SCOPED_BUSY(this);

	if (this->LoadFile(uri)) {
		m_uri = uri;
		return true;
	}
	return false;
}

//**********************************************************************

bool EditorWindow::Reload (void) {
	SCOPED_BUSY(this);
	m_duringReload = true;
	bool retval = this->LoadFile(m_uri, false);
	m_duringReload = false;
	return retval;
}

//**********************************************************************

bool EditorWindow::Save (void)
{
	return this->save(m_uri);
}

//**********************************************************************

bool EditorWindow::SaveAs (const String& uri)
{
	if (this->save(uri)) {
		m_uri = uri;
		return true;
	}
	return false;
}

//**********************************************************************

bool EditorWindow::SaveToDisk (const String& uri)
{
	if (uri.empty())
		return false;

	SCOPED_BUSY(this);
	IncrementInternalSaveCounter();
	std::ofstream output(uri.c_str(), std::ios_base::out | std::ios_base::binary);
	if (output.is_open()) {
		output << this->GetText();
		output.close();
		return true;
	}
	else {
		DecrementInternalSaveCounter();	//no actual changes
		return false;
	}
}

//**********************************************************************

void EditorWindow::LoadLanguageModule (const String& moduleNamePath)
{
	EditorModuleLoader::DestroyLanguageModule(m_langModPath, m_langIface);
	m_langIface = EditorModuleLoader::GetLanguageModule(moduleNamePath);
	m_langIface->SetEditor(this);
	m_langModPath = moduleNamePath;

	wxAcceleratorEntry* entry;
	int size;
	m_langIface->GetAccelerators(&entry, &size);
	this->SetAcceleratorTable(wxAcceleratorTable(size, entry));
}

void EditorWindow::UnLoadLanguageModule (void)
{
	this->SetAcceleratorTable(wxAcceleratorTable());

	EditorModuleLoader::DestroyLanguageModule(m_langModPath, m_langIface);
	m_langIface = EditorModuleLoader::GetLanguageModule();
	m_langModPath.clear();
}

//**********************************************************************

bool EditorWindow::save (const String& uri)
{
	if (uri.empty())
		return false;
	else if (uri == m_uri && wxFile::Exists(uri) && !IsModified())
		return true;

	SCOPED_BUSY(this);
	IncrementInternalSaveCounter();
	std::ofstream output(uri.c_str(), std::ios_base::out | std::ios_base::binary);
	if (output.is_open()) {
		output << this->GetText();
		m_modified = false;
		this->SetSavePoint();
		output.close();
		return true;
	}
	else {
		DecrementInternalSaveCounter();	//no actual changes
		return false;
	}
}

//**********************************************************************

void EditorWindow::SetModified (bool val)
{
	bool modified = this->GetModify();
	if (val) {
		if (!modified) {
			wxScintillaEvent event(wxEVT_SCI_SAVEPOINTLEFT, this->GetId());
			event.SetModificationType(1);
			this->AddPendingEvent(event);
			m_modified = true;
		}
	}
	else {
		bool modified = IsModified();
		m_modified = false;
		if (modified)
			this->SetSavePoint();
	}
}

//**********************************************************************

bool EditorWindow::IsModified (void)
{
	return m_modified || this->GetModify();
}

//**********************************************************************

void EditorWindow::UpdateMatchedBrace (void)
{
	int currPos = this->GetCurrentPos();
	int c = this->GetCharAt(currPos);

	if (c == '<' || c == '>') {	//first check for << >> braces (unfortunately we can only highlight the outer <> pair)
		int offset = 0;
		if (currPos > 0 && this->GetCharAt(currPos - 1) == c)
			offset = -1;
		else if (currPos < this->GetLength() - 1 && this->GetCharAt(currPos + 1) == c)
			offset = 1;

		if (offset) {
			int oppositeBrace = this->BraceMatch(currPos);
			int otherOppositeBrace = this->BraceMatch(currPos + offset);
			if (oppositeBrace == -1 || otherOppositeBrace == -1)
				if (c == '<')
					this->BraceBadLight(offset > 0 ? currPos : currPos + offset);
				else
					this->BraceBadLight(offset > 0 ? currPos + offset: currPos);
			else {
				int min, max;
				if (c == '<') {
					min = std::min(currPos, currPos + offset);
					max = std::max(oppositeBrace, otherOppositeBrace);
				}
				else {
					max = std::max(currPos, currPos + offset);
					min = std::min(oppositeBrace, otherOppositeBrace);
				}
				this->BraceHighlight(min, max);
			}
			return;
		}
	}

	static const std::string braceChars("()[]{}");	//then check for the rest braces
	if (braceChars.find(c) == String::npos) {
		if (--currPos < 0 || braceChars.find(this->GetCharAt(currPos)) == std::string::npos) {
			this->BraceHighlight(-1, -1);
			this->BraceBadLight(-1);
			return;
		}
	}
	int oppositeBrace = this->BraceMatch(currPos);

	if (oppositeBrace == INVALID_POSITION)
		this->BraceBadLight(currPos);
	else
		this->BraceHighlight(currPos, oppositeBrace);
}

//**********************************************************************

void EditorWindow::ShowFoldingView (bool value)
{
	this->SetMarginWidth(MARGIN_FOLDING, value ? 12 : 0);
}

//**********************************************************************

void EditorWindow::RequestToggleFoldingAtPos (int pos)
{
	int line = this->LineFromPosition(pos);
	int levelClick = this->GetFoldLevel(line);
	if (levelClick & wxSCI_FOLDLEVELHEADERFLAG)
		this->ToggleFold(line);
}

//**********************************************************************

void EditorWindow::AddIndicator (int start, int end, int mask)
{
	this->StartStyling(start, wxSCI_INDICS_MASK);
	for (int i = start; i < end; ++i)
		this->SetStyling(1, this->GetStyleAt(i) | mask);
}

//**********************************************************************

void EditorWindow::RemoveIndicator (int start, int end, int mask)
{
	this->StartStyling(start, wxSCI_INDICS_MASK);
	for (int i = start; i < end; ++i)
		this->SetStyling(1, this->GetStyleAt(i) & ~mask);
}

//**********************************************************************

void EditorWindow::InsertEnabledBreakpointToLine (int line) { this->insert(line, MarkerRoundedRed); }
void EditorWindow::RemoveEnabledBreakpointFromLine (int line) { this->remove(line, MarkerRoundedRed); }

void EditorWindow::InsertDisabledBreakpointToLine (int line) { this->insert(line, MarkerRoundedRedEmpty); }
void EditorWindow::RemoveDisabledBreakpointFromLine (int line) { this->remove(line, MarkerRoundedRedEmpty); }

void EditorWindow::InsertErrorToLine (int line) { this->insert(line, MarkerError); }
void EditorWindow::RemoveErrorFromLine (int line) { this->remove(line, MarkerError); }

void EditorWindow::InsertBookmarkToLine (int line) { this->insert(line, MarkerRoundedBlue); }
void EditorWindow::RemoveBookmarkFromLine (int line) { this->remove(line, MarkerRoundedBlue); }
void EditorWindow::ToggleBookmarkAtLine (int line) { this->toggle(line, MarkerRoundedBlue); }
bool EditorWindow::HasBookmarkAtLine (int line) { return this->MarkerGet(line) & 0x1; }

int EditorWindow::NextBookmarkFromLine (int line)
	{ return this->MarkerNext(line, 0x1); }

int EditorWindow::PrevBookmarkFromLine (int line)
	{ return this->MarkerPrevious(line, 0x1); }

void EditorWindow::ClearBookmarks (void)
	{ return this->removeall(MarkerRoundedBlue); }

const std::vector<int> EditorWindow::GetAllBookmarksFromLine (int line)
	{ return this->getAll(line, MarkerRoundedBlue); }

const std::vector<int> EditorWindow::GetAllEnabledBreakpointsFromLine (int line)
	{ return this->getAll(line, MarkerRoundedRed); }

const std::vector<int> EditorWindow::GetAllDisabledBreakpointsFromLine (int line)
	{ return this->getAll(line, MarkerRoundedRedEmpty); }

//**********************************************************************

void EditorWindow::SetYellowArrowToLine (int line)
{
	this->RemoveYellowArrow();
	this->MarkerAdd(line, MarkerArrowYellow);
	this->GotoLine(line);
	m_yellowArrowLine = line;
}

void EditorWindow::RemoveYellowArrow (void)
{
	this->MarkerDelete(m_yellowArrowLine, MarkerArrowYellow);
}

//**********************************************************************

void EditorWindow::SetGreenArrowToLine (int line)
{
	this->RemoveGreenArrow();
	this->MarkerAdd(line, MarkerArrowGreen);
	this->GotoLine(line);
	m_greenArrowLine = line;
}

void EditorWindow::RemoveGreenArrow (void)
{
	this->MarkerDelete(m_greenArrowLine, MarkerArrowGreen);
}

//**********************************************************************


void EditorWindow::insert (int line, int type)
{
	if (!(this->MarkerGet(line) & (0x1 << type)))
		this->MarkerAdd(line, type);
}

void EditorWindow::remove (int line, int type)
	{ this->MarkerDelete(line, type); }

void EditorWindow::toggle (int line, int type)
{
	if (this->MarkerGet(line) & (0x1 << type))
		this->remove(line, type);
	else
		this->insert(line, type);
}

void EditorWindow::removeall (int type)
	{ this->MarkerDeleteAll(type); }

//**********************************************************************

const std::vector<int> EditorWindow::getAll (int line, int type)
{
	std::vector<int> result;
	while ((line = this->MarkerGet(line) & (0x1 << type)) != -1)
		result.push_back(line++);
	return result;
}

//**********************************************************************

void EditorWindow::onSavePointReached (wxScintillaEvent& event)
{
	if (!m_modified)
		event.Skip();
}

//**********************************************************************

void EditorWindow::onAutoCompleteSelection (wxScintillaEvent& event) {
}

//**********************************************************************

void EditorWindow::onLeftSavePoint (wxScintillaEvent& event)
{
	//ModificationType == 1 signals that the leftSavePoint is artificial
	if (event.GetModificationType() == 1 || !m_modified)
		event.Skip();
}

//**********************************************************************

void EditorWindow::onKillFocus (wxFocusEvent& event) {
	if (this->AutoCompActive())
		this->AutoCompCancel();
}

////////////////////////////////////////////////////////////////////////

} // namespace editor
