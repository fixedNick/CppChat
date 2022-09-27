#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "pch.h"
#include "framework.h"
#include <afxsock.h>
#include <iostream>
#include <thread>
#include <string>
#include "Utils.h"
#include "../SocketServer/SockOperations.h"
#include "../SocketServer/Message.h"

using namespace std;

static int ClientID = 0;

SOCKET sock;

Message SendMsg(Message msg, bool isReceive = false)
{
	CSocket s;
	if (!sock) {
		s.Create();
		if (!s.Connect("127.0.0.1", 12345))
			throw runtime_error("Connection to server failed");
	}
	else s.Attach(sock);

	SockOperations::Send(s, msg);

	if (isReceive) {
		Message resp = SockOperations::Receive(s);
		sock = s.Detach();
		return resp;
	}
	else
	{
		sock = s.Detach();
		return Message();
	}
}

void CheckForNewMessages()
{
	while (true)
	{
		Sleep(7000);
		Message msg(MR_BROKER, ClientID, MT_GET_DATA, "");
		auto response = SendMsg(msg, true);
		switch (response.MsgHeader.Type)
		{
		case MT_NO_DATA:
			break;
		case MT_USER_NOT_FOUND:
			cout << "[SERVER]: User not found\n";
			break;	
		case MT_RECEIVE_DATA:
			cout << "You got new messages!\n";
			cout << "[" << (response.MsgHeader.From == MR_BROKER ? "SERVER" : to_string(response.MsgHeader.From)) << "]: " << response.data << "\n";
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

		if (cmd.rfind("/", 0) != std::string::npos)
		{
			cout << "1.To send message enter [clientID:message]\n";
			cout << "2.To get list of users online: /online\n";
			continue;
		}
		else if (cmd.find(":") != std::string::npos)
		{
			vector<string> cmdParts = Utils::SplitCommand(cmd, ":");
			Message msg(atoi(cmdParts.at(0).c_str()), ClientID, MT_SEND_DATA, cmdParts.at(1));
			SendMsg(msg, false);
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
