/**
 *	ChatRoomManager.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_CHATROOMMANAGER_H
#define TALKIE_CHATROOMMANAGER_H

#include "Common.h"
#include "ChatRoom.h"
#include "NotebookCnt.h"

#include <boost/signal.hpp>
#include <vector>
#include <wx/frame.h>
class wxTimer;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class ChatRoomManager : public wxFrame {
public:
	typedef boost::signal<void (void)>									Sig1;
	typedef boost::signal<void (const FriendEntry&, const String& msg)> Sig2;

	ChatRoomManager (
		wxWindow*					parent,
		boost::asio::io_service&	service
	);
	~ChatRoomManager (void);

	void CreateChatRoom (const FriendEntryVec& participants, const FriendEntry& me);
	void JoinChatRoom (net::connection_ptr conn, const FriendEntry& me);

	uint GetNumberOfActiveChatRooms (void) const { return m_rooms.size(); }

	Sig1 sigClosed;
	Sig1 sigAllChatRoomsClosed;
	Sig2 sigUserTalkedWithoutFocus;

private:
	typedef std::vector<ChatRoom*> ChatRooms;

	DECLARE_EVENT_TABLE();

	ChatRoom* createAndAddChatRoom (const FriendEntry& me);

	void slotChatRoomConnected (ChatRoom* room);
	void slotChatRoomDisconnected (ChatRoom* room);

	void slotReceivedNewUserMessage (
		ChatRoom* room, const FriendEntry& entry, const String& msg
	);
	void slotReceivedViewCommand (ChatRoom* room, ChatRoom::ViewCommand command);

	void onChatRoomChanged (wxAuiNotebookEvent& event);
	void onChatRoomClosed (wxAuiNotebookEvent& event);

	void onActivated (wxKeyEvent& event);
	void onClose (wxCloseEvent& event);

	void onFocused (wxFocusEvent& event);
	void onTimer (wxTimerEvent& event);

	boost::asio::io_service&	m_service;
	ide::NotebookCnt*			m_notebook;
	wxTimer*					m_timer;
	uint						m_timerStep;
	wxPoint						m_initialPos;
	ChatRooms					m_rooms;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_CHATROOMMANAGER_H
