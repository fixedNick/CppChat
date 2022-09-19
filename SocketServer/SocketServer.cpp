#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "pch.h"
#include "framework.h"
#include "SocketServer.h"
#include "Message.h"
#include "SockOperations.h"

int actualUserID = 100;

void ProcessClient(SOCKET hSock)
{
	CSocket clientSock;
	clientSock.Attach(hSock);

	while (true)
	{
		Message msg = SockOperations::Receive(clientSock);

		cout << "Received message from user." << endl;
		switch (msg.MsgHeader.Type)
		{
		case MT_INIT:
			cout << "Registration for new user. Given ID: [" << ++actualUserID << "], user name: [" << msg.data << "]" << endl;
			Message response(actualUserID, MR_BROKER, MT_CONFIRM, actualUserID + "");
			SockOperations::Send(clientSock, response);
			break;
		}
	}
}

void Server()
{
	AfxSocketInit();

	cout << "Server started..." << endl;
	CSocket Server;
	Server.Create(12345);

	while (true)
	{
		if (!Server.Listen())
			break;
		CSocket s;
		Server.Accept(s);
		thread t(ProcessClient, s.Detach());
		t.detach();
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
			Server();
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
