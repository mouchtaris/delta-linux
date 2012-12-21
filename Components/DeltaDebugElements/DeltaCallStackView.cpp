/**
 *	DeltaCallStackView.cpp
 *
 *	-- IDE Call Stach View --
 *
 *	View of a Delta program's call stack during debugging.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "DeltaCallStackView.h"

#include "StringUtils.h"
#include "PropertyUtils.h"
#include "ImageList.h"

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"

#include "ComponentRegistry.h"
#include "BitmapRegistry.h"

#include "UndefineAllWin32Shit.h"

#include <wx/filename.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Icons/breakpoint_mark.xpm"
#include "Icons/stack_mark.xpm"

namespace ide
{
	BEGIN_EVENT_TABLE(DeltaCallStackView, ItemListCtrl)
		EVT_LIST_ITEM_ACTIVATED(wxID_ANY, DeltaCallStackView::onItemActivated)
	END_EVENT_TABLE();

enum StackMarks {
	BreakpointMark	= 0,
	StackMark		= 1
};

	//-------------------------------------------------------//
	//---- class DeltaCallStackView -------------------------//

	COMPONENT_METADATA(
		DeltaCallStackView,
		_("Call Stack"),
		_("Debugger call stack during tracing"),
		_T("Themistoklis Bourdenas, Yannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(DeltaCallStackView, ItemListCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DeltaCallStackView, table)
	{
		conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
			conf::safe_prop_cast<const conf::EnumStringProperty>(
				table.GetProperty("docking")
			)
		);
		if (docking)
			docking->SetOption(5);
	}

	//-----------------------------------------------------------------------

	DeltaCallStackView::DeltaCallStackView(void)
	{
	}

	//-----------------------------------------------------------------------

	DeltaCallStackView::~DeltaCallStackView(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* DeltaCallStackView::GenerateWindow(wxWindow* parent)
	{
		Create(parent, wxLC_REPORT | wxLC_SINGLE_SEL);

		ImageList* images = new ImageList(16,16);
		images->Add(_T("breakpoint_mark"), BitmapRegistry::Instance().Get(_T("breakpoint_mark")));
		images->Add(_T("stack_mark"), BitmapRegistry::Instance().Get(_T("stack_mark")));
		ItemListCtrl::AssignImageList(images, wxIMAGE_LIST_SMALL);

		ItemListCtrl::InsertColumn(0, String(),				wxLIST_FORMAT_LEFT, 25);
		ItemListCtrl::InsertColumn(1, _T("Function"),		wxLIST_FORMAT_LEFT, 150);
		ItemListCtrl::InsertColumn(2, _T("VM id"),			wxLIST_FORMAT_LEFT, 100);
		ItemListCtrl::InsertColumn(3, _T("Definition Line"),wxLIST_FORMAT_LEFT, 70);
		ItemListCtrl::InsertColumn(4, _T("Call Line"),		wxLIST_FORMAT_LEFT, 70);
		ItemListCtrl::InsertColumn(5, _T("Scope"),			wxLIST_FORMAT_LEFT, 50);
		ItemListCtrl::InsertColumn(6, _T("Call Params"),	wxLIST_FORMAT_LEFT, 100);
		ItemListCtrl::InsertColumn(7, _T("Source File"),	wxLIST_FORMAT_LEFT, 200);

		RefreshCallStackView();
		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* DeltaCallStackView::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaCallStackView, void, Initialize, (void))
	{
		BitmapRegistry::Instance().Insert(_T("breakpoint_mark"), breakpoint_mark_xpm);
		BitmapRegistry::Instance().Insert(_T("stack_mark"), stack_mark_xpm);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaCallStackView, void, CleanUp, (void))
	{
		BitmapRegistry::Instance().Remove(_T("breakpoint_mark"));
		BitmapRegistry::Instance().Remove(_T("stack_mark"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaCallStackView, int, GetTotalStackFrames, (void))
	{
		return GetItemCount();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaCallStackView, String, GetStackFrame, (uint index))
	{
		return Call<String (uint)>(this, "DeltaVM", "GetStackFrame")(index);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaCallStackView, void, MoveStackFrame, (uint index))
	{
		Call<void (uint)>(this, "DeltaVM", "MoveStackFrame")(index);
		ClearActivationRecordMarkings();
		MarkActivationRecord(ItemListCtrl::GetItemCount() - 1);
		MarkActivationRecord(index);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaCallStackView, void, RefreshCallStackView, (void))
	{
		ItemListCtrl::DeleteAllItems();
		uint totalFrames = Call<uint (void)>(this, "DeltaVM", "GetTotalStackFrames")();
		for (uint i = 0; i < totalFrames; ++i) {
			const String stackFrame = Call<String (uint)>(this, "DeltaVM", "GetStackFrame")(i);
			if(!stackFrame.empty()) {
				std::vector<String> tokens;
				util::stringtokenizer(tokens, stackFrame, _T("#"));
				assert(tokens.size() == 5);
				AddStackFrame(
					tokens[0],
					boost::lexical_cast<uint>(tokens[1]),
					boost::lexical_cast<uint>(tokens[2]),
					boost::lexical_cast<uint>(tokens[3]),
					tokens[4]);
			}
		}
		ClearActivationRecordMarkings();
		MarkActivationRecord(ItemListCtrl::GetItemCount() - 1);
		MarkActivationRecord(Call<uint (void)>(this, "DeltaVM", "GetCurrentStackFrameIndex")());
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(DeltaCallStackView, View, _("/{10}View/{100}Call Stack\tCtrl+Shift+K"), MT_MAIN, "")
	{
		EnsureVisibility("DeltaCallStackView");
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(DeltaCallStackView, void, slotPushStackFrame,
		(const std::string& classId, const String& record, uint defLine, uint callLine, uint scope, const String& params),
		"PushStackFrame")
	{
		AddStackFrame(record, defLine, callLine, scope, params);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(DeltaCallStackView, void, slotStackFrameMoved,
		(const std::string& classId, uint index), "StackFrameMoved")
	{
		ClearActivationRecordMarkings();
		MarkActivationRecord(ItemListCtrl::GetItemCount() - 1);
		MarkActivationRecord(index);

		long itemIndex = GetItemCount() - index - 1;

		String symbolic;
		uint lineno = 0, definitionLine = 0;

        wxListItem info;
		info.m_itemId = itemIndex;
        info.m_mask = wxLIST_MASK_TEXT;
        info.m_col = 7;	// source file
        if (GetItem(info))
			symbolic = info.GetText();

        info.m_col = 3;	// definition file
		if (GetItem(info)) {
			if (info.GetText().empty())
				return;
			definitionLine = boost::lexical_cast<uint>(info.GetText());
		}

		if (definitionLine) {
			if (!itemIndex)
				lineno = Call<uint (void)>(this, "DeltaVM", "GetStopPointLine")();
			else {
				info.m_itemId = itemIndex - 1;
				info.m_col = 4;	// call line
				if (GetItem(info)) {
					if (info.GetText().empty())
						return;
					lineno = boost::lexical_cast<uint>(info.GetText());
				}
			}
		}

		EnsureVisibility("EditorManager");

		const HandleVec& editors = Call<const HandleVec& (void)>(this, "EditorManager", "GetEditors")();
		BOOST_FOREACH(const Handle& editor, editors)
			Call<void (void)>(this, editor, "RemoveTraceArrow")();

		if (lineno) {
			Call<void (const String&, int)>(this, "DeltaVM", "GotoSymbolicDocument")(symbolic, lineno);

			if (itemIndex) {	// not the top activation record
				const Handle& editor = Call<const Handle& (const String&)>(this, "DeltaVM", "GetEditorFromSymbolicURI")(symbolic);
				if (editor) {
					Call<void (int)>(this, editor, "SetTraceArrow")(lineno);
					tracePoint = TracePointInfo(Call<const String& (void)>(this, editor, "GetURI")(), lineno);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(DeltaCallStackView, void, slotClear,
		(const std::string& classId),
		"DebugResumed")
	{
		ItemListCtrl::DeleteAllItems();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(DeltaCallStackView, void, slotTreeItemSymbolicURIChanged,
		(const Handle& handle, const String& oldUri, const String& newUri), "TreeItemSymbolicURIChanged")
	{
		if (handle.GetClassId() == "Script" || handle.GetClassId() == "Aspect") {
			wxListItem info;
			info.m_col = 7;	//source
			info.m_mask = wxLIST_MASK_TEXT;

			for (int i=0; i < GetItemCount(); ++i) {
				info.m_itemId = i;
				GetItem(info);
				if (info.m_text == oldUri) {
					ItemListCtrl::SetItem(i, 7, newUri);
					break;
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(DeltaCallStackView, void, slotFileNameChanged,
		(const Handle& editor, const String& uri), "FileNameChanged")
	{
		if (wxFileName(tracePoint.first).SameAs(util::normalizepath(uri)))
			Call<void (int), SafeCall>(s_classId, editor, "SetTraceArrow")(tracePoint.second);
	}

	//-----------------------------------------------------------------------

	void DeltaCallStackView::MarkActivationRecord(uint index)
	{
		if (index >= (uint)GetItemCount())
			return;

		wxListItem info;
		info.m_col = 0;
		info.m_mask = wxLIST_MASK_IMAGE;
		info.m_itemId = GetItemCount() - index - 1;
		info.m_image = info.m_itemId ? StackMark : BreakpointMark;
		SetItem(info);
	}

	//-----------------------------------------------------------------------

	void DeltaCallStackView::ClearActivationRecordMarkings(void)
	{
		wxListItem info;
		info.m_col = 0;
		info.m_mask = wxLIST_MASK_IMAGE;

		for (int i=0; i < GetItemCount(); ++i) {
			info.m_itemId = i;
			info.m_image = -1;
			SetItem(info);
		}
	}

	//-----------------------------------------------------------------------

	void DeltaCallStackView::AddStackFrame(const String& record, uint defLine, uint callLine, uint scope, const String& params)
	{
		String function, vmId, source;
		uint delimiter_index_1 = 0, delimiter_index_2 = 0;

		if ((delimiter_index_2 = (uint) record.find_last_of(_T("::"))) != String::npos)
		{
			delimiter_index_1 = (uint) record.find_first_of(_T("::"));
			source = record.substr(0, delimiter_index_1);
			vmId = record.substr(delimiter_index_1 + 2, delimiter_index_2 - delimiter_index_1 - 3);
			function = record.substr(delimiter_index_2 + 1);
		}
		else
			function = record;

		long id = ItemListCtrl::InsertItem(0, -1);
		ItemListCtrl::SetItem(id, 1, function);									///< Function
		ItemListCtrl::SetItem(id, 2, vmId);										///< VM id
		ItemListCtrl::SetItem(id, 3, boost::lexical_cast<String>(defLine));		///< Definition Line
		ItemListCtrl::SetItem(id, 4, boost::lexical_cast<String>(callLine));	///< Call Line
		ItemListCtrl::SetItem(id, 5, boost::lexical_cast<String>(scope));		///< Scope
		ItemListCtrl::SetItem(id, 6, params);									///< Call
		ItemListCtrl::SetItem(id, 7, source);									///< Source File

		ClearActivationRecordMarkings();
		MarkActivationRecord(ItemListCtrl::GetItemCount() - 1);
	}

	//-----------------------------------------------------------------------

	void DeltaCallStackView::onItemActivated(wxListEvent& event)
	{
		Call<void (uint)>(this, "DeltaVM", "MoveStackFrame")(GetItemCount() - event.GetIndex() - 1);
	}

	//-----------------------------------------------------------------------
}
