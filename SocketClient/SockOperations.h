#include "Message.h"
#ifndef SOCK_OPERATION_H
#define SOCK_OPERATION_H

class SockOperations
{
public:
	static Message Receive(SOCKET& s);
	static bool Send(SOCKET& s, Message msg);
};

#endif