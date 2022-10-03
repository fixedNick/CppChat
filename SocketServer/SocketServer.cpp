#include "pch.h"
#include "framework.h"
#include "SocketServer.h"
#include "Message.h"
#include "SockOperations.h"
#include "QueueUnit.h"
#include "Client.h"
#include <vector>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

const long long ACTIVE_TIMEOT = 3; // 15 seconds of inactive = disconnect

void ProcessClient(SOCKET hSock)
{
	Client* client = nullptr;

	while (true)
	{
		Message msg = SockOperations::Receive(hSock);

		if (msg.MsgHeader.Type != MT_INIT)
			if (client->isDisconnectActivated)
				if (msg.MsgHeader.Type != MT_GET_DATA && msg.MsgHeader.Type != MT_CONFIRM_DISCONNECT)
					continue;


		cout << "Received message from user: " << msg.MsgHeader.From << ", type: " << msg.MsgHeader.Type << "\n";
		switch (msg.MsgHeader.Type)
		{
		case MT_CONFIRM_DISCONNECT:
			for (int i = 0; i < Client::Clients.size(); i++)
			{
				if (Client::Clients[i]->ID == client->ID)
				{
					cout << "[Disconnect] Client with id: " << client->ID << " has been confirm his disconnect, closing socket\n";
					CSocket s; 
					s.Attach(hSock);
					s.Close();
					Client::Clients.erase(Client::Clients.begin() + i);
					break;
				}
			}
			return;
		case MT_GET_ONLINE:
			{
				string response = "";
				for (auto& c : Client::Clients)
				{
					response += c->Name;
					response += "/";
					response += to_string(c->ID);
					response += ":";
				}

				response[response.length() - 1] = '\0';
				
				Message mResponse(client->ID, MR_BROKER, MT_GET_ONLINE, response);
				client->AddMessageToQueue(mResponse);
			}
			break;
		case MT_SEND_DATA:
			{
				cout << "User " << msg.MsgHeader.From << ", Sending data: " << msg.data << ", to user: " << msg.MsgHeader.To << "\n";

				if (msg.MsgHeader.To == MR_ALL)
				{
					msg.MsgHeader.Type = MT_RECEIVE_DATA;
					for (auto c : Client::Clients) 
					{
						if (c->ID == client->ID)
							continue;

						c->AddMessageToQueue(msg);
					}

					break;
				}

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
		{
			cout << "Client [" << client->ID << "] closed connection\n";
			Message exitMsg(client->ID, MR_BROKER, MT_DISCONNECT, "You have been disconnected successfully");
			client->StopClient(exitMsg);
			return; 
		}
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

void CheckActiveConnections()
{
	int deletedClientIndex = 0;
	bool isClientDeleted = false;
	while (true)
	{
		if (isClientDeleted == false) 
		{
			Sleep(5000);
			deletedClientIndex = 0;
		}
		else isClientDeleted = false;

		for (int i = deletedClientIndex; i < Client::Clients.size(); i++)
		{
			long long currentTime = Utils::GetCurrentTimeSec(); // 150
			
			if (Client::Clients[i]->isDisconnectActivated == false && currentTime - Client::Clients[i]->LastInteractionTime >= ACTIVE_TIMEOT)
			{
				// disconnect client
				int clientId = Client::Clients[i]->ID;
				Message msg(Client::Clients[i]->ID, MR_BROKER, MT_DISCONNECT, "Disconnect. You weren't active for some time");
				Client::Clients[i]->StopClient(msg);
				deletedClientIndex = i;
				isClientDeleted = true;
				
				cout << "Client [" << clientId << "] has been disconnected. Reason: Inactive Timeout\n";
				break;
			}
		}
	}
}

void Server()
{
	AfxSocketInit();

	cout << "Server started..." << "\n";
	CSocket Server;
	Server.Create(12345);

	thread tCheckConnections(CheckActiveConnections);
	tCheckConnections.detach();

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
