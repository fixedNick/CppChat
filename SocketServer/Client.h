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
	static vector<Client*> Clients;
	static inline int ActualClientID = 100; // >= C++ 17, inline оператор позволяет инициализировать статическую переменную и обойтись без явного указания места инициализации
public:
	string Name = "";
	int ID = -1;
	SOCKET ClientSocketHandle = 0;
	queue<QueueUnit> MessagesQueue;
	long long LastInteractionTime = 0; // timastamp

	static Client* Create(const string& name, SOCKET hSock);
	static Client* GetClient(int clientID, bool& isClientOnline);

	void AddMessageToQueue(Message msg);
	void StopClient();
	bool SendMsg();
};

#endif