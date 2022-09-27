#ifndef QUEUE_UNIT_H
#define QUEUE_UNIT_H

#include "pch.h"
#include "Message.h"

class QueueUnit
{
public:
	Message Msg;
	SOCKET hSockTo;

	QueueUnit(Message msg, SOCKET hSock);
};

#endif