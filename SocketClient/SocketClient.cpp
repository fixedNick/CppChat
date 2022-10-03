#include "pch.h"
#include "framework.h"
#include <afxsock.h>
#include <iostream>
#include <thread>
#include <string>
#include "Utils.h"
#include "SockOperations.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

static int ClientID = 0;

SOCKET sock;
bool isClientDisconnected = false;

Message SendMsg(Message msg, bool isReceive = false)
{
	if (!sock) {
		CSocket s;
		s.Create();
		if (s.Connect("127.0.0.1", 12345) == false)
			throw runtime_error("Connection to server failed");
		else sock = s.Detach();
	}

	SockOperations::Send(sock, msg);

	if (isReceive)
	{
		auto msg = SockOperations::Receive(sock);
		return msg;
	}
	else
		return Message();
}


void CheckForNewMessages()
{
	while (true)
	{
		Sleep(2333);
		Message msg(MR_BROKER, ClientID, MT_GET_DATA, "");
		auto response = SendMsg(msg, true);

		switch (response.MsgHeader.Type)
		{
		case MT_DISCONNECT:
			{
				cout << "Send disconnect confirm\n";
				isClientDisconnected = true;
				Message confMessage(MR_BROKER, ClientID, MT_CONFIRM_DISCONNECT, "");
				SendMsg(confMessage);
				return;
			}
		case MT_NO_DATA:
			break;
		case MT_USER_NOT_FOUND:
			cout << "[SERVER]: User not found\n";
			break;	
		case MT_RECEIVE_DATA:
			cout << "You got new messages!\n";
			cout << "[" << (response.MsgHeader.From == MR_BROKER ? "SERVER" : to_string(response.MsgHeader.From)) << "]: " << response.data << "\n";
			break;
		case MT_GET_ONLINE:
			{
				vector<string> usersOnline = Utils::Split(response.data, ":");
				cout << "Users online:\n";
				for (string& user : usersOnline)
				{
					vector<string> userInfo = Utils::Split(user, "/");
					cout << "[" << userInfo[0] << "] " << userInfo[1] << "\n";
				}
			}
			break;
		default:
			break;
		}

	}
}

void Client()
{
	AfxSocketInit();
	string userName;
	cout << "Enter your name: ";
	getline(cin, userName);

	cout << "Connecting to server..\n";
	cout << "Sending request to initialize socket..\n";
	Message initMsg(MR_BROKER, MR_NO_UID, MT_INIT, userName);
	Message response = SendMsg(initMsg, true);
	if (response.MsgHeader.Type == MT_CONFIRM)
	{
		ClientID = atoi(response.data.c_str());
		cout << "Successfully connected to server [127.0.0.1:12345], your id [" << ClientID << "]\n";
		cout << "Enter /help to see all instructions\n";
		thread t(CheckForNewMessages);
		t.detach();
	}
	else
		cout << "Authorization of socket failed.\n";

	while (true)
	{
		string cmd;
		cout << "Enter command: ";
		getline(cin, cmd);

		if (isClientDisconnected)
		{
			cout << "You've been disconnected from the server..\nPress enter to close your client!";
			getline(cin,cmd);
			Sleep(2000);
			CSocket s;
			s.Attach(sock);
			s.Close();
			return;
		}

		if (cmd.rfind("/", 0) != std::string::npos)
		{
			if (strcmp(cmd.c_str(), "/help") == 0) 
			{
				cout << "1.To send message enter [clientID:message]\n";
				cout << "1.To send message for everyone [all:message]\n";
				cout << "3.To get list of users online: /online\n";
				cout << "4.To exit from chat: /exit\n";
			}
			else if (strcmp(cmd.c_str(), "/online") == 0)
			{
				Message getOnlineMsg(MR_BROKER, ClientID, MT_GET_ONLINE, "");
				SendMsg(getOnlineMsg);
			}
			else if (strcmp(cmd.c_str(), "/exit") == 0)
			{
				Message getOnlineMsg(MR_BROKER, ClientID, MT_EXIT, "");
				SendMsg(getOnlineMsg);
				break;
			}
			continue;
		}
		else if (cmd.find(":") != std::string::npos)
		{
			vector<string> cmdParts = Utils::SplitCommand(cmd, ":");

			Message msg;
			if (cmdParts[0] == "all")
				msg = Message(MR_ALL, ClientID, MT_SEND_DATA, cmdParts.at(1));
			else
				msg = Message(atoi(cmdParts.at(0).c_str()), ClientID, MT_SEND_DATA, cmdParts.at(1));

			cout << "Message to everyone successfully send!\n";
			SendMsg(msg);
		}
		else cout << "Unknown command\n";
	}
}

CWinApp theApp;

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: code your application's behavior here.
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
			Client();
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
