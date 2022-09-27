#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "pch.h"
#include "framework.h"
#include "SocketServer.h"
#include "Message.h"
#include "SockOperations.h"
#include "QueueUnit.h"
#include <vector>

int actualUserID = 100;

map<int, SOCKET> clients;
map<int, queue<QueueUnit>> QueuesCollection;

void AddMessageToQueue(int clientID, QueueUnit unit)
{
	QueuesCollection[clientID].push(unit);
}

void ProcessClient(SOCKET hSock)
{
	CSocket clientSock;
	int clientID;
	clientSock.Attach(hSock);

	while (true)
	{
		Message msg = SockOperations::Receive(clientSock);
		bool isUserOnline;
		cout << "Received message from user: " << msg.MsgHeader.From << ", type: " << (MessageTypes)msg.MsgHeader.Type << "\n";
		switch (msg.MsgHeader.Type)
		{
		case MT_SEND_DATA:
			cout << "User " << msg.MsgHeader.From << ", Sending data: " << msg.data << ", to user: " << msg.MsgHeader.To << "\n";

			isUserOnline = false;

			for (auto& client : clients)
			{
				if (client.first == msg.MsgHeader.To)
				{
					isUserOnline = true;

					msg.MsgHeader.Type = MT_RECEIVE_DATA;
					Message successMsg(clientID, MR_BROKER, MT_RECEIVE_DATA, "Your message successfully sent");

					QueueUnit qSend(msg, client.second);
					AddMessageToQueue(client.first, qSend);

					QueueUnit qResp(successMsg, clients[clientID]);
					AddMessageToQueue(clientID, qResp);
					
					SockOperations::Send(clientSock, successMsg);
				}
			}

			if (isUserOnline == false) {
				Message m(msg.MsgHeader.From, MR_BROKER, MT_USER_NOT_FOUND, "");
				QueueUnit qUnit(m, clientSock);
				AddMessageToQueue(clientID, qUnit);
			}
			break;
		case MT_CONFIRM:
			clientSock.Close();
			return;
		case MT_GET_DATA:
			if (QueuesCollection[clientID].empty() == true)
			{
				Message emptyMessage(clientID, MR_BROKER, MT_NO_DATA, "");
				SockOperations::Send(clientSock, emptyMessage);
				break;
			}

			{
				clientSock.Detach();
				CSocket recipient;
				QueueUnit msgToSend = QueuesCollection[clientID].front();
				recipient.Attach(msgToSend.hSockTo);
				SockOperations::Send(recipient, msgToSend.Msg);
				recipient.Detach();
				clientSock.Attach(hSock);

				QueuesCollection[clientID].pop();
			}

			break;
		case MT_INIT:
			clientID = ++actualUserID;
			cout << "Registration for new user. Given ID: [" << clientID << "], user name: [" << msg.data << "]" << "\n";
			
			string sUserID = to_string(actualUserID);

			Message response(actualUserID, MR_BROKER, MT_CONFIRM, sUserID);
			response.MsgHeader.Size = strlen(sUserID.c_str());
			SockOperations::Send(clientSock, response);
			clients[actualUserID] = hSock;
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
