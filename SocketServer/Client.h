#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include <string>
#include <vector>
#include "QueueUnit.h"
#include "SockOperations.h"
#include "../SocketClient/Utils.h"

using namespace std;

class Client
{
private:
	static inline int ActualClientID = 100; // >= C++ 17, inline оператор позволяет инициализировать статическую переменную и обойтись без явного указания места инициализации
public:
	static vector<Client*> Clients;
	string Name = "";
	int ID = -1;
	SOCKET ClientSocketHandle = 0;
	long long LastInteractionTime = 0; // timastamp
	bool isDisconnectActivated = false;
	queue<QueueUnit> MessagesQueue;

	static Client* Create(const string& name, SOCKET hSock);
	static Client* GetClient(int clientID, bool& isClientOnline);
	static bool IsClientInList(int clientId);

	void AddMessageToQueue(Message msg);
	void StopClient(Message messageToClient);
	bool SendMsg();
};

#endif