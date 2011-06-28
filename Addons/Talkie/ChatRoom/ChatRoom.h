/**
 *	ChatRoom.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_CHATROOM_H
#define TALKIE_CHATROOM_H

#include "Common.h"
#include "FriendListMVC.h"
#include "ChatRoomService.h"

#include <boost/signal.hpp>

#include <wx/splitter.h>
class wxTextAttr;

namespace ide {
class TextCtrl;
}

namespace talkie {

////////////////////////////////////////////////////////////////////////

class ChatRoomManager;

////////////////////////////////////////////////////////////////////////

class ChatRoom : public wxSplitterWindow, public FriendListController {
public:
	enum ViewCommand { VC_NUDGE };

	typedef boost::signal<void (ChatRoom*)>										Sig1;
	typedef boost::signal<void (ChatRoom*, const FriendEntry&, const String&)>	Sig2;
	typedef boost::signal<void (ChatRoom*, ViewCommand)>						Sig3;

	const String GetRoomTitle (void) const;
	const String GetRoomLabel (void) const;

	void AppendSystemMessage (const String& message, wxColour color);

	//******************************************************************
	// Overrided wx functions
	//
	void SetFocus (void);

	//******************************************************************

	Sig1 sigChatRoomConnected;
	Sig1 sigChatRoomDisconnected;
	Sig2 sigReceivedNewUserMessage;
	Sig3 sigReceivedViewCommand;

protected:
	friend class ChatRoomManager;

	ChatRoom (wxWindow* parent, boost::asio::io_service& service, const FriendEntry& me);
	~ChatRoom (void);

	ChatRoomService& GetService (void) { return m_service; }

	// For controller
	//
	virtual bool DropItems (const FriendEntryVec& items);

private:
	DECLARE_EVENT_TABLE();

	typedef std::map<const String, ViewCommand> ViewCommandMap;
	typedef std::map<String::char_type, wxFont>	DecoratorMap;

	enum StyleType { STYLE_SYSTEM, STYLE_HANDLE, STYLE_MSG };

	void newMessage (const FriendEntry& entry, const String& message);
	void userDisconnected (const FriendEntry& entry, const String& message);
	void userConnected (const FriendEntry& entry, const String& message);
	void error (const String& reason);

	void onLink (wxTextUrlEvent& event);
	void onEnter (wxCommandEvent& event);

	//******************************************************************

	const wxTextAttr& getStyle (StyleType type = STYLE_MSG, wxColour color = *wxBLACK) const;

	//******************************************************************

	ide::TextCtrl*			m_input;
	ide::TextCtrl*			m_output;

	ChatRoomService			m_service;

	static ViewCommandMap	s_viewCommands;
	static DecoratorMap		s_decorators;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_CHATROOM_H
