/**
 *	MainWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_MAINWINDOW_H
#define TALKIE_MAINWINDOW_H

#include "Common.h"
#include "FriendListMVC.h"
#include "DirectoryClient.h"
#include "FriendListService.h"
#include "UndefineAllWin32Shit.h"

#include <wx/frame.h>
#include <wx/taskbar.h>
class wxButton;
class wxBoxSizer;
class wxMenu;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class ChatRoomManager;
class UserStatusWindow;
class LoginWindow;
class RegistrationWindow;
class TaskBarIcon;

////////////////////////////////////////////////////////////////////////

class MainWindow : public wxFrame, public FriendListController {
public:
	MainWindow (
		const String&				title,
		boost::asio::io_service&	service,
		DirectoryClient&			directory
	);
	~MainWindow (void);

	////////////////////////////////////////////////////////////////////
	// Controller interface
	//
	virtual void PopupContextMenu (void);
	virtual void ActivateItem (int id);

private:
	typedef std::list<const FriendEntry*> FriendEntryPtrList;

	DECLARE_EVENT_TABLE();

	void onStatusChanged (wxCommandEvent& event);
	void onMenuItemSelected (wxCommandEvent& event);
	void onAddButtonPressed (wxCommandEvent& event);
	void onActivateCRMButtonPressed (wxCommandEvent& event);
	void onClose (wxCloseEvent& event);

	//******************************************************************
	// Task bar signals
	//
	void slotTaskBarDClick (void);
	void slotTaskBarCreateMenu (wxMenu* menu);

	//******************************************************************

	void createLoginInterface (bool create = true);
	void createRegisterInterface (bool create = true);
	void createMainInterface (bool create = true);

	void activateChatRoomManager (void);
	void slotChatRoomManagerClose (void);
	void slotAllChatRoomsClosed (void);
	void slotUserTalkedWithoutFocus (const FriendEntry& entry, const String& msg);

	//******************************************************************

	void loginStart (void);
	void registerRequest (void);
	void registerCancelRequest (void);
	void registerStart (const FriendEntry& info, const String& passwd);

	//******************************************************************

	void dirRegisterReply (bool result);
	void dirLoginReply (bool result, const FriendEntry& entry);
	void dirPendingOperationReply (const PendingOp& operation);
	void dirGetUserInfoReply (bool result, const FriendEntry& entry);

	void dirRequestAddFriendReply (
		bool					result,
		const String&			handle,
		bool					imediateAddition,
		const net::Endpoint&	peer
	);
	void dirRequestRemoveFriendReply (bool result, const String& handle);

	void dirAuthorizeFriendReply (
		bool					result,
		const String&			handle,
		bool					authorize,
		const net::Endpoint&	peer
	);
	void dirRequestMoreServersReply (const net::Endpoint& server);
	void dirSendOfflineMsgReply (bool result, const String& handle);

	//******************************************************************

	void talk (void);
	void addFriend (void);
	void removeFriends (void);
	void displayInfo (void);
	void addDirectoryServer (void);
	void aboutApplication (void);
	void exitApplication (void);

	//******************************************************************

	void joinChatRoom (net::connection_ptr conn);
	void pendingInvitations (void);
	void authorized (const String& handle);
	void unknownUser (const String& handle);
	void userLoggedIn (const FriendEntry& handle);
	void logout (void);

	//******************************************************************

	void setSelectedFriends (bool onlyActive = false);

	//******************************************************************

	boost::asio::io_service&	m_service;
	DirectoryClient&			m_directory;
	FriendListService			m_friendService;

	TaskBarIcon*				m_taskBar;
	ChatRoomManager*			m_chatRoomManager;
	UserStatusWindow*			m_userStatus;

	LoginWindow*				m_login;
	RegistrationWindow*			m_register;
	FriendListView*				m_friendListView;

	FriendEntryPtrList			m_selectedFriends;
	wxMenu*						m_contextMenu;
	wxBoxSizer*					m_statusSizer;
	wxButton*					m_addButton;
	wxButton*					m_activateCRMButton;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_MAINWINDOW_H
