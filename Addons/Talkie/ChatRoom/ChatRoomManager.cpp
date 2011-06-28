/**
 *	ChatRoomManager.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ChatRoomManager.h"
#include "Streams.h"
#include "SoundManager.h"

#include <wx/icon.h>
#include <wx/timer.h>

using namespace ide;

namespace talkie {

////////////////////////////////////////////////////////////////////////

static wxPoint _GetNextOffset (uint step);

////////////////////////////////////////////////////////////////////////
// ChatRoomManager class
//
BEGIN_EVENT_TABLE(ChatRoomManager, wxFrame)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, ChatRoomManager::onChatRoomChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, ChatRoomManager::onChatRoomClosed)
	EVT_CLOSE(ChatRoomManager::onClose)
	EVT_SET_FOCUS(ChatRoomManager::onFocused)
	EVT_TIMER(wxID_ANY, ChatRoomManager::onTimer)
END_EVENT_TABLE();

//**********************************************************************

ChatRoomManager::ChatRoomManager (wxWindow* parent, boost::asio::io_service& service) :
	wxFrame(parent, wxID_ANY, String(), wxDefaultPosition, wxSize(350, 500),
		wxDEFAULT_FRAME_STYLE),
	m_service(service), m_timerStep(0)
{
	this->SetIcon(wxIcon(_T("talkie_icon")));
	this->CreateStatusBar();
	this->SetStatusText(_("Connecting..."));

	m_notebook = new NotebookCnt(this);
	m_timer = new wxTimer(this);
}

//**********************************************************************

ChatRoomManager::~ChatRoomManager (void)
{
	delete m_timer;
}

//**********************************************************************

void ChatRoomManager::CreateChatRoom (const FriendEntryVec& participants, const FriendEntry& me)
{
	ChatRoom* room = this->createAndAddChatRoom(me);
	room->GetService().InitializeChatRoom(participants);
}

//**********************************************************************

void ChatRoomManager::JoinChatRoom (net::connection_ptr conn, const FriendEntry& me)
{
	ChatRoom* room = this->createAndAddChatRoom(me);
	room->GetService().JoinChatRoom(conn);
}

//**********************************************************************

ChatRoom* ChatRoomManager::createAndAddChatRoom (const FriendEntry& me)
{
	ChatRoom* room = new ChatRoom(m_notebook, m_service, me);

	room->sigChatRoomConnected.connect(
		boost::bind(&ChatRoomManager::slotChatRoomConnected, this, _1)
	);
	room->sigChatRoomDisconnected.connect(
		boost::bind(&ChatRoomManager::slotChatRoomDisconnected, this, _1)
	);
	room->sigReceivedNewUserMessage.connect(
		boost::bind(&ChatRoomManager::slotReceivedNewUserMessage, this, _1, _2, _3)
	);
	room->sigReceivedViewCommand.connect(
		boost::bind(&ChatRoomManager::slotReceivedViewCommand, this, _1, _2)
	);

	m_rooms.push_back(room);

	bool select = m_notebook->GetPageCount() == 0;
	m_notebook->AddPage(room, room->GetRoomLabel(), select,
		select ? wxNullBitmap : wxBitmap(_T("ind_important")));

	return room;
}

//**********************************************************************

void ChatRoomManager::slotChatRoomConnected (ChatRoom* room)
{
	int index = m_notebook->GetPageIndex(room);
	m_notebook->SetPageText(index, room->GetRoomLabel());

	if (index == m_notebook->GetSelection())
		this->SetTitle(room->GetRoomTitle());

	this->SetStatusText(_("Connected"));
}

//**********************************************************************

void ChatRoomManager::slotChatRoomDisconnected (ChatRoom* room)
{

}

//**********************************************************************

void ChatRoomManager::slotReceivedNewUserMessage (
		ChatRoom* room, const FriendEntry& entry, const String& msg
	)
{
	int index = m_notebook->GetPageIndex(room);
	this->SetStatusText(_("Also sprach") + String(_T(" ")) + entry.GetName());

	if (index != m_notebook->GetSelection()) {
		if (this->IsShown() && this->IsActive())
			m_notebook->SetPageBitmap(index, wxBitmap(_T("ind_important")));
		else {
			m_notebook->SetSelection(index);
			sigUserTalkedWithoutFocus(entry, msg);
		}
	}
	else if (!this->IsShown() || !this->IsActive())
		sigUserTalkedWithoutFocus(entry, msg);
}

//**********************************************************************

void ChatRoomManager::slotReceivedViewCommand (ChatRoom* room, ChatRoom::ViewCommand command)
{
	if (!m_timer->IsRunning()) {
		m_initialPos = this->GetPosition();
		m_timerStep = 0;
		m_timer->Start(50);
		sound::SoundManager::GetSound(_T("talkie-nudge")).Play(false);
	}
}

//**********************************************************************

void ChatRoomManager::onChatRoomChanged (wxAuiNotebookEvent& event)
{
	int page = event.GetSelection();
	const String title = m_rooms[page]->GetRoomTitle();
	DBGOUT << "Title " << page << ":" << title << std::endl;
	m_notebook->SetPageBitmap(page, wxNullBitmap);
	this->SetTitle(title);
}

//**********************************************************************

void ChatRoomManager::onChatRoomClosed (wxAuiNotebookEvent& event)
{
	int tab = event.GetSelection();
	m_rooms.erase(m_rooms.begin() + tab);

	DBGOUT << "Removink tab: " << tab << std::endl;

	if (m_rooms.empty())
		sigAllChatRoomsClosed();
}

//**********************************************************************

void ChatRoomManager::onActivated (wxKeyEvent& event)
{
	DBGOUT << "chat room activated" << std::endl;
}

//**********************************************************************

void ChatRoomManager::onClose (wxCloseEvent& event)
{
	DBGOUT << "on close" << std::endl;
	sigClosed();
}

//**********************************************************************

void ChatRoomManager::onFocused (wxFocusEvent& event)
{
	ChatRoom* room = m_rooms[m_notebook->GetSelection()];
	room->SetFocus();
}

//**********************************************************************

void ChatRoomManager::onTimer (wxTimerEvent& event)
{
	wxPoint offset = _GetNextOffset(m_timerStep++);

	if (offset.x == 0 && offset.y == 0)
		m_timer->Stop();

	DBGOUT << "Moving by: (" << offset.x << ", " << offset.y << ")" << std::endl;
	this->SetPosition(m_initialPos + offset);
}

////////////////////////////////////////////////////////////////////////
// Nudge data
//
static wxPoint _GetNextOffset (uint step)
{
	static const int orientations[][2] = {
		{ -1, 1 }, { 1, 1 }, { -1, -1 }, { 1, -1 },
	};
	static const int values[] = {
		1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 6, 5, 4, 4, 3, 3, 2, 2, 1
	};
	if (step >= SIZEOF_ARRAY(values))
		return wxPoint(0, 0);
	else {
		uint index = step % SIZEOF_ARRAY(orientations);
		return wxPoint(
			orientations[index][0] * values[step],
			orientations[index][1] * values[SIZEOF_ARRAY(values) - step - 1]
		);
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
