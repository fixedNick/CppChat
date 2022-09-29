#include "pch.h"
#include "framework.h"
#include "SocketServer.h"
#include "Message.h"
#include "SockOperations.h"
#include "QueueUnit.h"
#include "Client.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ProcessClient(SOCKET hSock)
{
	Client* client = nullptr;

	while (true)
	{
		Message msg = SockOperations::Receive(hSock);
		cout << "Received message from user: " << msg.MsgHeader.From << ", type: " << msg.MsgHeader.Type << "\n";
		switch (msg.MsgHeader.Type)
		{
		case MT_GET_ONLINE:
			// TODO
			break;
		case MT_SEND_DATA:
			{
				cout << "User " << msg.MsgHeader.From << ", Sending data: " << msg.data << ", to user: " << msg.MsgHeader.To << "\n";
				bool isClientOnline;
				Client* recipient = Client::GetClient(msg.MsgHeader.To, isClientOnline);

				if (isClientOnline == false)
				{	
					Message m(msg.MsgHeader.From, MR_BROKER, MT_USER_NOT_FOUND, "");
					client->AddMessageToQueue(m);
				}
				else 
				{
					Message msgToSend(msg.MsgHeader.To, msg.MsgHeader.From, MT_RECEIVE_DATA, msg.data);

					int clientId = client->ID;
					Message successMsg(clientId, MR_BROKER, MT_RECEIVE_DATA, "Your message successfully sent");

					client->AddMessageToQueue(successMsg);
					recipient->AddMessageToQueue(msgToSend);
				}
			}
			break;
		case MT_EXIT:
			client->StopClient();
			return;
		case MT_GET_DATA:
			if(client->SendMsg() == false)
			{
				Message emptyMessage(client->ID, MR_BROKER, MT_NO_DATA, "");
				SockOperations::Send(client->ClientSocketHandle, emptyMessage);
			}
			break;
		case MT_INIT:
			client = Client::Create(msg.data, hSock);
			cout << "Registration for new user. Given ID: [" << client->ID << "], user name: [" << msg.data << "]" << "\n";
			
			Message response(client->ID, MR_BROKER, MT_CONFIRM, to_string(client->ID));
			SockOperations::Send(client->ClientSocketHandle, response);
			break;
		}
		
	}
}

void Server()
{
	AfxSocketInit();

	cout << "Server started..." << "\n";
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
