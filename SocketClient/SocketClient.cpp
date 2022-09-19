#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "pch.h"
#include <afxsock.h>
#include <iostream>
#include <thread>
#include <string>
#include "framework.h"
#include "SockOperations.h"
#include "Message.h"

using namespace std;



void Client()
{
	AfxSocketInit();
	CSocket s;
	s.Create();
	string userName;
	cout << "Enter your name: ";
	cin >> userName;

	cout << "Connecting to server..\n";
		
	if (!s.Connect("127.0.0.1", 12345))
	{
		cout << "Connection of socket failed.\n";
	}

	cout << "Sending request to initialize socket..\n";
	Message initMsg(MR_BROKER, -1, MT_INIT, userName);
	SockOperations::Send(s, initMsg);
	Message response = SockOperations::Receive(s);
	if (response.MsgHeader.Type == MT_CONFIRM)
	{
		cout << "Successfully connected to server [127.0.0.1:12345]\n";
	}
	else
	{
		cout << "Authorization of socket failed.\n";
	}

	while (true)
	{

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
		{
			Client();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
