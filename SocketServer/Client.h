#ifndef CLIENT_H
#define CLIENT_H

#include <queue>
#include "QueueUnit.h"

class Client
{
private:
	static inline int ActualClientID = 100; // >= C++ 17, inline оператор позволяет инициализировать статическую переменную и обойтись без явного указания места инициализации
public:
	int ClientID;
	SOCKET ClientSocketHandle;
	queue<QueueUnit> MessagesQueue;
	int LastInteractionTime; // timastamp
};

#endif