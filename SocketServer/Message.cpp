#include "pch.h"
#include "Message.h"

Message::Message() {}
Message::Message(int to, int from, int type, string data)
{
	MsgHeader.To = to;
	MsgHeader.From = from;
	MsgHeader.Type = type;
	MsgHeader.Size = int(data.length());

	this->data = data;
}