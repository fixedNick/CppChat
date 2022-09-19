#include "Message.h"
#ifndef SOCK_OPERATION_H
#define SOCK_OPERATION_H

class SockOperations
{
public:
	static Message Receive(CSocket& s);
	static bool Send(CSocket& s, Message msg);
};

#endif