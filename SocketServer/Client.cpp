#include "pch.h"
#include "Client.h"

vector<Client*> Client::Clients;

// Pattern: Factory Method 
Client* Client::Create(const string& name, SOCKET hSock)
{
	Client* client = new Client();
	client->Name = name;
	client->ID = ++ActualClientID;
	client->ClientSocketHandle = hSock;
	client->LastInteractionTime = Utils::GetCurrentTimeSec();
	Clients.push_back(client);
	return client;
}

Client* Client::GetClient(int clientID, bool& isClientOnline)
{
	for (auto& client : Clients)
	{
		if (client->ID == clientID) 
		{
			isClientOnline = true;
			return client;
		}
	}
	isClientOnline = false;
	return new Client();
}

void Client::AddMessageToQueue(Message msg)
{
	QueueUnit qUnit(msg, ClientSocketHandle);
	MessagesQueue.push(qUnit);
}

void Client::StopClient()
{
	for (int i = 0; i < Clients.size(); i++)
	{
		if (Clients[i]->ID == ID)
		{
			Clients.erase(Clients.begin() + i);
			return;
		}
	}
}

bool Client::SendMsg()
{
	if (MessagesQueue.empty() == true)
		return false;

	SockOperations::Send(MessagesQueue.front().hSockTo, MessagesQueue.front().Msg);
	MessagesQueue.pop();
	return true;
}
