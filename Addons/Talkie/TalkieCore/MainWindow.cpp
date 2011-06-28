/**
 *	MainWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MainWindow.h"
#include "Adaptors.h"
#include "StringUtils.h"
#include "CommonGUIUtils.h"
#include "ChatRoomManager.h"
#include "ContactsImageList.h"
#include "UserStatusWindow.h"
#include "Streams.h"

#include "LoginWindow.h"
#include "RegistrationWindow.h"
#include "TalkieDialogs.h"
#include "TaskBarIcon.h"

#include "SoundManager.h"
#include "infowindow.h"

#include <boost/lexical_cast.hpp>

#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/button.h>
#include <wx/textdlg.h>
#include <wx/accel.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////

enum ContextMenuCommType {
	CMC_TALK, CMC_BLOCK, CMC_ADD, CMC_REMOVE, CMC_INFO,
	CMC_ADD_SERVER, CMC_ABOUT, CMC_EXIT
};

enum ButtonCommType {
	BUTTON_ADD,
	BUTTON_ACTIVATE_CRM,
};

////////////////////////////////////////////////////////////////////////
// MainWindow class
//
BEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_COMBOBOX(wxID_ANY, MainWindow::onStatusChanged)
	EVT_MENU(wxID_ANY, MainWindow::onMenuItemSelected)
	EVT_BUTTON(BUTTON_ADD, MainWindow::onAddButtonPressed)
	EVT_BUTTON(BUTTON_ACTIVATE_CRM, MainWindow::onActivateCRMButtonPressed)
	EVT_CLOSE(MainWindow::onClose)
END_EVENT_TABLE();

////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow (
		const String&				title,
		boost::asio::io_service&	service,
		DirectoryClient&			directory
	) :
	wxFrame(0, wxID_ANY, title, wxDefaultPosition, wxSize(260, 400),
		wxMINIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION | wxSYSTEM_MENU |
			0 /*wxSTAY_ON_TOP*/),
	m_service(service), m_directory(directory), m_friendService(service),
	m_userStatus(0), m_login(0), m_register(0), m_friendListView(0)
{
	ContactsImageList::Initialize();

	////////////////////////////////////////////////////////////////////
	// Set directory reply functions
	//
	m_directory.funcRegisterReply =
		boost::bind(&MainWindow::dirRegisterReply, this, _1);

	m_directory.funcLoginReply =
		boost::bind(&MainWindow::dirLoginReply, this, _1, _2);

	m_directory.funcPendingOperationReply =
		boost::bind(&MainWindow::dirPendingOperationReply, this, _1);

	m_directory.funcGetUserInfoReply =
		boost::bind(&MainWindow::dirGetUserInfoReply, this, _1, _2);

	m_directory.funcRequestAddFriendReply =
		boost::bind(&MainWindow::dirRequestAddFriendReply, this, _1, _2, _3, _4);

	m_directory.funcRequestRemoveFriendReply =
		boost::bind(&MainWindow::dirRequestRemoveFriendReply, this, _1, _2);

	m_directory.funcAuthorizeFriendReply =
		boost::bind(&MainWindow::dirAuthorizeFriendReply, this, _1, _2, _3, _4);

	m_directory.funcRequestMoreServersReply =
		boost::bind(&MainWindow::dirRequestMoreServersReply, this, _1);

	m_directory.funcSendOfflineMsgReply =
		boost::bind(&MainWindow::dirSendOfflineMsgReply, this, _1, _2);

	////////////////////////////////////////////////////////////////////
	// Set friend entry signals
	//
	m_friendService.sigPendingInvitations.connect(
		boost::bind(&MainWindow::pendingInvitations, this)
	);
	m_friendService.sigAuthorized.connect(boost::bind(&MainWindow::authorized, this, _1));
	m_friendService.sigUnknownUser.connect(boost::bind(&MainWindow::unknownUser, this, _1));
	m_friendService.sigRequestChat.connect(boost::bind(&MainWindow::joinChatRoom, this, _1));
	m_friendService.sigUserLoggedIn.connect(boost::bind(&MainWindow::userLoggedIn, this, _1));

	////////////////////////////////////////////////////////////////////
	// Initialize basic stuff
	//
	this->SetIcon(wxIcon(_T("talkie_icon")));
	this->SetMinSize(wxSize(250, 300));

	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY, wxST_SIZEGRIP);

	m_addButton = new wxButton(this, BUTTON_ADD, _("Add"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxNO_BORDER);
	m_addButton->Hide();

	m_activateCRMButton = new wxButton(this, BUTTON_ACTIVATE_CRM, _("Show chat"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxNO_BORDER);
	m_activateCRMButton->Hide();

	m_statusSizer = new wxBoxSizer(wxHORIZONTAL);
	m_statusSizer->Add(m_addButton);
	m_statusSizer->Add(m_activateCRMButton);
	m_statusSizer->Add(statusBar, 1, wxEXPAND);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_statusSizer, 0, wxEXPAND);
	this->SetSizer(sizer);

	////////////////////////////////////////////////////////////////////
	// Context menu man
	//
	m_contextMenu = new wxMenu;
	m_contextMenu->Append(CMC_TALK, _("Talkie, talkie"), _("Pricing: 1 euro / second"));
	m_contextMenu->AppendSeparator();
	m_contextMenu->Append(CMC_ADD, _("Add a new friend"), _("Are you feeling lonely ?"));
	m_contextMenu->Append(CMC_BLOCK, _("Block"), _("A.K.A, polite ignore"));
	m_contextMenu->Append(CMC_REMOVE, _("Remove"), _("Is it cause they is black ?"));
	m_contextMenu->AppendSeparator();
	m_contextMenu->Append(CMC_INFO, _("Info..."), _("Show user information"));

	////////////////////////////////////////////////////////////////////
	// Set accelarators
	//
	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_CTRL | wxACCEL_ALT, int('S'), CMC_ADD_SERVER);
	entries[1].Set(wxACCEL_CTRL, int('A'), CMC_ABOUT);
	wxAcceleratorTable accel(2, entries);
	this->SetAcceleratorTable(accel);

	////////////////////////////////////////////////////////////////////
	// Set taskbar
	//
	m_taskBar = new TaskBarIcon(this);
	m_taskBar->SetIcon(wxIcon(_T("talkie_icon")), _("Talkie (BETA)"));

	m_taskBar->sigDoubleClick.connect(boost::bind(&MainWindow::slotTaskBarDClick, this));
	m_taskBar->sigCreateMenu.connect(boost::bind(&MainWindow::slotTaskBarCreateMenu, this, _1));

	////////////////////////////////////////////////////////////////////
	// Initialize core components
	//
	this->createLoginInterface(true);

	m_friendService.Start(); // Start friendService
}

//**********************************************************************

MainWindow::~MainWindow (void)
{
	if (m_friendListView)
		this->logout();

	m_friendService.Stop(); // Stop friendService
	m_directory.ClearFunctions();

	delete m_contextMenu;
	m_contextMenu = 0;

	////////////////////////////////////////////////////////////////////
	// Clear taskbar
	//
	m_taskBar->RemoveIcon();
	delete m_taskBar;

	ContactsImageList::CleanUp();
}

//**********************************************************************

void MainWindow::PopupContextMenu (void)
{
	this->setSelectedFriends();

	bool haveActive = std::find_if(
			m_selectedFriends.begin(), m_selectedFriends.end(),
			util::dereference<const FriendEntry*>(FriendEntry::CanConnectPred())
		) != m_selectedFriends.end();

	bool haveSelected = !m_selectedFriends.empty();

	m_contextMenu->Enable(CMC_TALK, haveActive);
	m_contextMenu->Enable(CMC_BLOCK, haveSelected);
	m_contextMenu->Enable(CMC_REMOVE, haveSelected);
	m_contextMenu->Enable(CMC_INFO, m_selectedFriends.size() == 1);

	this->PopupMenu(m_contextMenu);
}

//**********************************************************************

void MainWindow::ActivateItem (int id)
{
	this->talk();
}

//**********************************************************************

void MainWindow::onStatusChanged (wxCommandEvent& event)
{
	FriendEntry::Status status = static_cast<FriendEntry::Status>(event.GetSelection());

	DBGOUT << status << std::endl;

	m_friendService.SetStatus(status);

	if (status == FriendEntry::OFFLINE) {
		this->logout();
		this->createMainInterface(false);
		this->createLoginInterface(true);
	}
}

//**********************************************************************

void MainWindow::onMenuItemSelected (wxCommandEvent& event)
{
	DBGOUT << event.GetId() << std::endl;

	switch (event.GetId()) {
		case CMC_TALK:			this->talk();				break;
		case CMC_ADD:			this->addFriend();			break;
		case CMC_REMOVE:		this->removeFriends();		break;
		case CMC_INFO:			this->displayInfo();		break;
		case CMC_ADD_SERVER:	this->addDirectoryServer();	break;
		case CMC_ABOUT:			this->aboutApplication();	break;
		case CMC_EXIT:			this->exitApplication();	break;
		default: break;
	}	
}

//**********************************************************************

void MainWindow::onAddButtonPressed (wxCommandEvent& event)
{
	this->addFriend();
}

//**********************************************************************

void MainWindow::onActivateCRMButtonPressed (wxCommandEvent& event)
{
	assert(!m_chatRoomManager->IsShown());
	this->activateChatRoomManager();
}

//**********************************************************************

void MainWindow::onClose (wxCloseEvent& event)
{
	if (event.CanVeto()) {
		event.Veto();
		this->Hide();
	}
	else
		this->Destroy();
}

//**********************************************************************

void MainWindow::slotTaskBarDClick (void)
{
	this->Show(!this->IsShown());

	if (this->IsShown())
		this->SetFocus();
}

//**********************************************************************

void MainWindow::slotTaskBarCreateMenu (wxMenu* menu)
{
	menu->SetTitle(_("Talkie"));

	menu->Append(CMC_ABOUT, _("About..."), _("About this application"));
	menu->AppendSeparator();
	menu->Append(CMC_EXIT, _("Exit"), _("Leaving so soon?"));
}

//**********************************************************************

void MainWindow::createLoginInterface (bool create)
{
	GUI_SCOPED_FREEZE(this);

	if (create) {
		m_login = new LoginWindow(this, _T("talkie-login.gif"));
		m_login->sigLogin.connect(boost::bind(&MainWindow::loginStart, this));
		m_login->sigRegister.connect(boost::bind(&MainWindow::registerRequest, this));

		this->GetSizer()->Prepend(m_login, 1, wxEXPAND);
		this->Layout();
	}
	else {
		m_login->Destroy();
		m_login = 0;
	}
}

//**********************************************************************

void MainWindow::createRegisterInterface (bool create)
{
	GUI_SCOPED_FREEZE(this);

	if (create) {
		m_register = new RegistrationWindow(this, _T("talkie-register.gif"));
		m_register->sigRegister.connect(boost::bind(&MainWindow::registerStart, this, _1, _2));
		m_register->sigCancel.connect(boost::bind(&MainWindow::registerCancelRequest, this));

		this->GetSizer()->Prepend(m_register, 1, wxEXPAND);
		this->Layout();
	}
	else {
		m_register->Destroy();
		m_register = 0;
	}
}

//**********************************************************************

void MainWindow::createMainInterface (bool create)
{
	GUI_SCOPED_FREEZE(this);

	// wxBUG:
	// The window cannot be destroyed by the event handler of its child window
	//
	if (!m_userStatus)
		m_userStatus = new UserStatusWindow(this);
	m_userStatus->Show(create);

	m_activateCRMButton->Hide();
	m_addButton->Show(create);

	if (create) {

		m_chatRoomManager = new ChatRoomManager(this, m_service);
		m_chatRoomManager->sigClosed.connect(
			boost::bind(&MainWindow::slotChatRoomManagerClose, this)
		);
		m_chatRoomManager->sigAllChatRoomsClosed.connect(
			boost::bind(&MainWindow::slotAllChatRoomsClosed, this)
		);
		m_chatRoomManager->sigUserTalkedWithoutFocus.connect(
			boost::bind(&MainWindow::slotUserTalkedWithoutFocus, this, _1, _2)
		);

		m_userStatus->SetEntry(m_friendService.GetUserInfo());

		m_friendListView = new FriendListView(this);
		m_friendListView->SetController(this);
		m_friendListView->SetModel(&m_friendService.GetFriendList());

		this->GetSizer()->Prepend(m_friendListView, 1, wxEXPAND);
		this->GetSizer()->Prepend(m_userStatus, 0, wxEXPAND);
		this->Layout();
	}
	else {
		m_friendListView->Destroy();
		m_friendListView = 0;

		m_chatRoomManager->Destroy();
		m_chatRoomManager = 0;

		this->GetSizer()->Detach(m_userStatus);
	}
}

//**********************************************************************

void MainWindow::activateChatRoomManager (void)
{
	if (!m_chatRoomManager->IsShown()) {

		m_chatRoomManager->Show();
		m_activateCRMButton->Hide();
		m_statusSizer->Layout();
	}
}

//**********************************************************************

void MainWindow::slotChatRoomManagerClose (void)
{
	assert(m_chatRoomManager->IsShown());
	m_chatRoomManager->Hide();
	m_activateCRMButton->Show();
	m_statusSizer->Layout();
}

//**********************************************************************

void MainWindow::slotAllChatRoomsClosed (void)
{
	m_chatRoomManager->Hide();
}

//**********************************************************************

void MainWindow::slotUserTalkedWithoutFocus (const FriendEntry& entry, const String& msg)
{
	if (msg.size() > 100) {
		const String shortMsg = msg.substr(0, 100) + _T("...");
		InfoWindow::Display(m_chatRoomManager, entry.GetName(), shortMsg, 3000);
	}
	else
		InfoWindow::Display(m_chatRoomManager, entry.GetName(), msg, 3000);

	sound::SoundManager::GetSound(_T("talkie-message")).Play(false);
	this->activateChatRoomManager();
}

//**********************************************************************

void MainWindow::loginStart (void)
{
	m_login->LoginStart();

	m_directory.SetUserName(m_login->GetUsername());
	m_directory.SetPassword(m_login->GetPasswd());

	m_directory.Login(m_friendService.GetEndpoint());
	m_directory.GetPendingOperations();
}

//**********************************************************************

void MainWindow::registerRequest (void)
{
	this->createLoginInterface(false);
	this->createRegisterInterface(true);
}

//**********************************************************************

void MainWindow::registerCancelRequest (void)
{
	this->createRegisterInterface(false);
	this->createLoginInterface(true);
}

//**********************************************************************

void MainWindow::registerStart (const FriendEntry& info, const String& passwd)
{
	m_register->RegisterStart();

	m_directory.Register(info, passwd);
	m_friendService.SetUserInfo(info);
}

//**********************************************************************

void MainWindow::dirRegisterReply (bool result)
{
	m_register->RegisterStop();

	if (result) {
		this->createRegisterInterface(false);
		this->createLoginInterface(true);
		m_login->SetInfoMessage(_("Registration succeeded!"));
		m_login->SetUsername(m_friendService.GetUserInfo().GetHandle());
	}
	else
		m_register->SetErrorMessage(_("Registration failed (miserably)"));
}

//**********************************************************************

void MainWindow::dirLoginReply (bool result, const FriendEntry& entry)
{
	DBGOUT << "YOOOOOOOOOOOOOOO" << std::endl;
	m_login->LoginStop();

	if (result) {
		m_friendService.SetUserInfo(entry);

		this->createLoginInterface(false);
		this->createMainInterface(true);
	}
	else
		m_login->SetErrorMessage(_("Cannot connect!"));
}

//**********************************************************************

void MainWindow::dirPendingOperationReply (const PendingOp& operation)
{
	comm::decoder dec(operation.get<1>());
	String user;
	dec >> user;

	switch (operation.get<0>()) {
		case PO_AUTHORIZE: {
			bool authorize = getConfirmation(this, _("User authorization"),
				_("Does you want to add your main man,\n") + user);
			m_directory.AuthorizeFriend(user, authorize);
			break;
		}
		case PO_INVITE:
			m_friendService.AddPendingAuthorization(user);
			break;
		case PO_OFFLINE_MSG: {
			String msg;
			dec >> msg;
			DBGOUT << "offline msg: " << user << ": " << msg << std::endl;
			break;
		}
		default:
			assert(false);
	}
}

//**********************************************************************

void MainWindow::dirGetUserInfoReply (bool result, const FriendEntry& entry)
{
	if (result && !m_friendService.ContainsFriend(entry)) {
		m_friendService.AddFriend(entry);
	}
}

//**********************************************************************

void MainWindow::dirRequestAddFriendReply (
		bool					result,
		const String&			handle,
		bool					imediateAddition,
		const net::Endpoint&	peer
	)
{
	if (result) {
		if (imediateAddition)
			m_directory.GetUserInfo(handle);
		else {
			m_friendService.AddPendingAuthorization(handle);
			if (peer)
				m_friendService.NotifyOfPendingInvitation(peer);
		}
	}
}

//**********************************************************************

void MainWindow::dirRequestRemoveFriendReply (bool result, const String& handle)
{
	if (result)
		m_friendService.RemoveFriend(handle);
}

//**********************************************************************

void MainWindow::dirAuthorizeFriendReply (
		bool					result,
		const String&			handle,
		bool					authorize,
		const net::Endpoint&	peer
	)
{
	if (result && authorize) {
		m_directory.GetUserInfo(handle);

		if (peer)
			m_friendService.NotifyOfAuthorization(peer);
	}
}

//**********************************************************************

void MainWindow::dirRequestMoreServersReply (const net::Endpoint& server)
{
	DBGOUT << "added server: " << server << std::endl;
}

//**********************************************************************

void MainWindow::dirSendOfflineMsgReply (bool result, const String& handle)
{
	DBGOUT << "offline message: " << result << std::endl;
}

//**********************************************************************

void MainWindow::talk (void)
{
	this->setSelectedFriends(true);

	if (!m_selectedFriends.empty()) {

		FriendEntryVec participants;
		std::transform(
			m_selectedFriends.begin(), m_selectedFriends.end(),
			std::back_inserter(participants),
			util::dereference<const FriendEntry*>(util::identity<const FriendEntry>())
		);
		m_chatRoomManager->CreateChatRoom(participants, m_friendService.GetUserInfo());
		this->activateChatRoomManager();
	}
}

//**********************************************************************

void MainWindow::addFriend (void)
{
	const String handle = util::strip(
		getInput(this, _("Add friend"), _("Type your friends username")), _T(" ")
	);
	if (!handle.empty()) {
		m_directory.RequestAddFriend(handle);
	}
}

//**********************************************************************

void MainWindow::removeFriends (void)
{
	this->setSelectedFriends(false);

	util::for_all(m_selectedFriends,
		boost::bind(&DirectoryClient::RequestRemoveFriend, boost::ref(m_directory),
			boost::bind(&FriendEntry::GetHandle, _1))
	);
}

//**********************************************************************

void MainWindow::displayInfo (void)
{
	this->setSelectedFriends(false);
	displayUserInfo(this, _("User information"), *m_selectedFriends.front());
}

//**********************************************************************

void MainWindow::addDirectoryServer (void)
{
	const String serverString = util::strip(getInput(this, _("Add server"),
		_("Type the directory server in the form HOST:PORT")), _T(" "));

	if (!serverString.empty()) {
		String::size_type pos = serverString.find_last_of(_T(':'));
		const String host = serverString.substr(0, pos);
		uint16 portNum = 2123;
		try {
			if (pos != String::npos && pos + 1 < serverString.size())
				portNum = boost::lexical_cast<uint16>(serverString.substr(pos + 1));
			m_directory.AddDirectory(net::Endpoint(host, portNum));
		}
		catch (std::exception&) {}
	}
}

//**********************************************************************

void MainWindow::aboutApplication (void)
{
	String aboutMsg = _T("Talkie (BETA)\n\n");
	aboutMsg += _("Giannis Georgalis <jgeorgal@ics.forth.gr>\n\n");
	aboutMsg += _("Themis Bourdenas <themis@ics.forth.gr>\n");
	displayMessage(this, _("About talkie..."), aboutMsg);
}

//**********************************************************************

void MainWindow::exitApplication (void)
{
	this->Close(true);
}

//**********************************************************************

void MainWindow::joinChatRoom (net::connection_ptr conn)
{
	m_chatRoomManager->JoinChatRoom(conn, m_friendService.GetUserInfo());
	this->activateChatRoomManager();
}

//**********************************************************************

void MainWindow::pendingInvitations (void)
	{ m_directory.GetPendingOperations(); }

void MainWindow::authorized (const String& handle)
	{ m_directory.GetUserInfo(handle); }

void MainWindow::unknownUser (const String& handle)
	{ m_directory.GetUserInfo(handle); }

//**********************************************************************

void MainWindow::userLoggedIn (const FriendEntry& entry)
{
	InfoWindow::Display(
		this, entry.GetName(),
		String::Format(_("User %s is online"), entry.GetHandle().c_str()),
		3000
	);
}

//**********************************************************************

void MainWindow::logout (void)
{
	m_directory.Logout();
	m_friendService.SetStatus(FriendEntry::OFFLINE);
	m_friendService.Clear();
}

//**********************************************************************

void MainWindow::setSelectedFriends (bool onlyActive)
{
	IntList ids;
	this->GetMonitoredView()->GetSelectedItems(ids);

	m_selectedFriends.clear();

	for (IntList::iterator i = ids.begin(); i != ids.end(); ++i) {
		const FriendEntry& entry = m_friendService.GetFriendList().GetFriend(*i);
		if (!onlyActive || FriendEntry::CanConnectPred()(entry))
			m_selectedFriends.push_back(&entry);
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
