#include "pch.h"
#include "QueueUnit.h"

QueueUnit::QueueUnit(Message msg, SOCKET hSock)
{
	this->Msg = msg;
	this->hSockTo = hSock;
}
