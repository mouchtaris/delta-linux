/**
 *	ChatRoom.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ChatRoom.h"
#include "StringUtils.h"
#include "Streams.h"
#include "ListBasicView.h"
#include "TextCtrl.h"
#include "CommonGUIUtils.h"

#include <boost/lambda/bind.hpp>

#include <wx/mimetype.h>

using namespace ide;

namespace talkie {

////////////////////////////////////////////////////////////////////////
// ChatRoom class
//
BEGIN_EVENT_TABLE(ChatRoom, wxSplitterWindow)
	EVT_TEXT_URL(wxID_ANY, ChatRoom::onLink)
	EVT_TEXT_ENTER(wxID_ANY, ChatRoom::onEnter)
END_EVENT_TABLE();

////////////////////////////////////////////////////////////////////////

ChatRoom::ChatRoom (wxWindow* parent, boost::asio::io_service& service, const FriendEntry& me) :
	wxSplitterWindow(parent, wxID_ANY), m_service(service, me)
{
	wxFont defaultFont(
		9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Arial")
	);
	wxSplitterWindow* hsplitter = new wxSplitterWindow(this, wxID_ANY);

	m_output =  new TextCtrl(
		hsplitter,
		wxNO_BORDER | wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY | wxTE_AUTO_URL,
		wxSize(270, 460)
	);
	m_output->SetCaret(0);
	m_output->SetDefaultStyle(this->getStyle());

	m_input = new TextCtrl(
		hsplitter,
		wxNO_BORDER | wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_AUTO_SCROLL,
		wxSize(270, 40)
	);
	m_input->SetFont(defaultFont);

	hsplitter->SetSashGravity(1.0);
	hsplitter->SetMinimumPaneSize(30);
	hsplitter->SplitHorizontally(m_output, m_input, -40);

	FriendListView* participantView = new FriendListView(this, wxSize(80, 100));
	participantView->SetController(this);
	participantView->SetModel(&m_service.GetFriendListModel());

	this->SetSashGravity(0.9);
	this->SetMinimumPaneSize(80);
	this->SplitVertically(hsplitter, participantView, -80);

	////////////////////////////////////////////////////////////////////
	// Connect slots
	//
	m_service.sigChatRoomCreated.connect(boost::bind(boost::ref(sigChatRoomConnected), this));
	m_service.sigChatRoomError.connect(boost::bind(&ChatRoom::error, this, _1));
	m_service.sigNewMessage.connect(boost::bind(&ChatRoom::newMessage, this, _1, _2));
	m_service.sigUserDisconnected.connect(boost::bind(&ChatRoom::userDisconnected, this, _1, _2));
	m_service.sigUserConnected.connect(boost::bind(&ChatRoom::userConnected, this, _1, _2));

	////////////////////////////////////////////////////////////////////
	// Initialize view commands & decorators
	//
	if (s_viewCommands.empty()) {
		s_viewCommands.insert(std::make_pair(_T("nudge"), VC_NUDGE));
	}
	if (s_decorators.empty()) {
		s_decorators.insert(std::make_pair(_T('*'),
			wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_BOLD, false, _T("Arial")))
		);
		s_decorators.insert(std::make_pair(_T('/'),
			wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC,
				wxFONTWEIGHT_NORMAL, false, _T("Arial")))
		);
		s_decorators.insert(std::make_pair(_T('_'),
			wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_NORMAL, true, _T("Arial")))
		);
	}
}

//**********************************************************************

ChatRoom::~ChatRoom (void)
{

}

//**********************************************************************

const String ChatRoom::GetRoomTitle (void) const
{
	const FriendListModel& participants = m_service.GetFriendListModel();

	if (participants.GetFriends().empty())
		return _("Chat room is empty");

	FriendEntryVec::const_iterator i = participants.GetFriends().begin();
	String summary = i->GetHandle();
	while (++i != participants.GetFriends().end())
		summary += _T(", ") + i->GetHandle();
	return summary;
}

//**********************************************************************

const String ChatRoom::GetRoomLabel (void) const
{
	const FriendListModel& participants = m_service.GetFriendListModel();

	return participants.GetFriends().empty() ? _("empty") :
		participants.GetFriends()[0].GetHandle();
}

//**********************************************************************

void ChatRoom::AppendSystemMessage (const String& message, wxColour color)
{
	m_output->SetDefaultStyle(this->getStyle(STYLE_SYSTEM, color));
	m_output->AppendText(message);
	m_output->AppendText(_T("\n"));

	m_output->ScrollLines(10);
}

//**********************************************************************

void ChatRoom::SetFocus (void)
{
	m_input->SetFocus();
}

//**********************************************************************

bool ChatRoom::DropItems (const FriendEntryVec& items)
{
	m_service.AddParticipants(items);
	return true;
}

//**********************************************************************

void ChatRoom::newMessage (const FriendEntry& entry, const String& message)
{
	m_output->SetDefaultStyle(this->getStyle(STYLE_HANDLE, wxColour(0, 0, 160)));
	m_output->AppendText(entry.GetHandle() + _T(": "));
	m_output->AppendText(_T("\n"));

	////////////////////////////////////////////////////////////////////
	// Command processing
	//
	util::range_type_list tokens;
	util::stringrangetokenizer(tokens, message);

	wxTextAttr msgAttr = this->getStyle(STYLE_MSG);

	util::range_type_list::iterator iter = tokens.begin();
	while (iter != tokens.end()) {
		int firstDec = iter->first, lastDec = iter->first + iter->second - 1;
		assert(firstDec >= 0 || lastDec < message.length());
		String::char_type dec = message[firstDec++];
		if (
			firstDec == lastDec || dec != message[lastDec] &&
			s_decorators.find(dec) != s_decorators.end()
		) {
			iter = tokens.erase(iter);
		}
		else
			++iter;
	}

	String::size_type start = 0;

	{ GUI_SCOPED_FREEZE(m_output);

		for (iter = tokens.begin(); iter != tokens.end(); ++iter) {

			DecoratorMap::const_iterator decIter = s_decorators.find(message[iter->first]);
			if (decIter != s_decorators.end()) {

				m_output->SetDefaultStyle(this->getStyle(STYLE_MSG));
				m_output->AppendText(message.substr(start, iter->first - start));

				msgAttr.SetFont(decIter->second);
				m_output->SetDefaultStyle(msgAttr);
				const String msg = message.substr(iter->first + 1, iter->second - 2);
				m_output->AppendText(msg);
				start = iter->first + iter->second;

				if (decIter->first == _T('*')) {
					ViewCommandMap::iterator commIter = s_viewCommands.find(msg);
					if (commIter != s_viewCommands.end())
						sigReceivedViewCommand(this, commIter->second);
				}
			}
		}
	} // GUI_SCOPED_FREEZE

	m_output->SetDefaultStyle(this->getStyle(STYLE_MSG));

	if (start == 0)
		m_output->AppendText(message);
	else if (start < message.length())
		m_output->AppendText(message.substr(start));

	m_output->AppendText(_T("\n"));
	m_output->ScrollLines(10);

	sigReceivedNewUserMessage(this, entry, message);
}

//**********************************************************************

void ChatRoom::userDisconnected (const FriendEntry& entry, const String& message)
{
	const String msg = entry.GetName() + _T(" ") + _("left the chatroom") +
		_T(" (") + message + _T(")");
	this->AppendSystemMessage(msg, wxColour(160, 0, 0));
}

//**********************************************************************

void ChatRoom::userConnected (const FriendEntry& entry, const String& message)
{
	const String msg = entry.GetName() + _T(" ") + _("joined the chatroom") +
		_T(" (") + message + _T(")");
	this->AppendSystemMessage(msg, wxColour(0, 160, 0));
}

//**********************************************************************

void ChatRoom::error (const String& reason)
{
	const String msg = _("ERROR:") + String(_T(" ")) + reason;
	this->AppendSystemMessage(msg, wxColour(250, 0, 0));
}

//**********************************************************************

void ChatRoom::onLink (wxTextUrlEvent& event)
{
	if (event.GetMouseEvent().LeftDown()) {
		const String link = m_output->GetRange(event.GetURLStart(), event.GetURLEnd());
		DBGOUT << "Click: " << link << std::endl;
		wxFileType* filetype = wxTheMimeTypesManager->GetFileTypeFromMimeType(_T("text/html"));
		if (filetype) {
			wxString cmd;
			if (filetype->GetOpenCommand(&cmd, wxFileType::MessageParameters(link)))
				::wxExecute(cmd);
			delete filetype;
		}
	}
}

//**********************************************************************

void ChatRoom::onEnter (wxCommandEvent& event)
{
	const String value = m_input->GetValue();
	String storage;
	const String& strippedValue = util::strip(value, storage);

	if (!strippedValue.empty())
		m_service.SendText(strippedValue);

	m_input->Clear();
}

//**********************************************************************

const wxTextAttr& ChatRoom::getStyle (StyleType type, wxColour color) const
{
	static bool attributesInit = false;

	static wxTextAttr attributes[] = {
		wxTextAttr(wxNullColour, wxNullColour,
			wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_NORMAL, false, _T("New Roman"))),

		wxTextAttr(wxNullColour, wxNullColour,
			wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_BOLD, false, _T("Arial"))),

		wxTextAttr(wxNullColour, wxNullColour,
			wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
				wxFONTWEIGHT_NORMAL, false, _T("Arial"))),
	};
	if (!attributesInit) {
		attributesInit = true;
		attributes[STYLE_SYSTEM].SetLeftIndent(100);
		attributes[STYLE_HANDLE].SetLeftIndent(0);
		attributes[STYLE_MSG].SetLeftIndent(0);
	}
	assert(type < SIZEOF_ARRAY(attributes));

	attributes[type].SetTextColour(color);
	return attributes[type];
}

//**********************************************************************

ChatRoom::ViewCommandMap	ChatRoom::s_viewCommands;
ChatRoom::DecoratorMap		ChatRoom::s_decorators;

////////////////////////////////////////////////////////////////////////

} // namespace talkie
