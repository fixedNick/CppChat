#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

using namespace std;

enum MessageTypes
{
	MT_INIT = 1,
	MT_EXIT = 2,
	MT_GET_DATA = 3,
	MT_SEND_DATA = 4,
	MT_NO_DATA = 5,
	MT_RECEIVE_DATA = 6,
	MT_CONFIRM = 7,
	MT_NO_USER = 8
};

enum MessageRecipients
{
	MR_BROKER = 10,
	MR_ALL = 50,
	MR_USER = 100
};

struct MessageHeader
{
	int To;
	int From;
	int Type;
	int Size;
};

class Message
{
public:
	MessageHeader MsgHeader = { 0 };
	string data;

	Message();
	Message(int to, int from, int type, string data);
};

#endif