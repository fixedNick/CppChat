#include "pch.h"
#include "SockOperations.h"

Message SockOperations::Receive(CSocket& s)
{
	Message response;
	if (!s.Receive(&response.MsgHeader, sizeof(MessageHeader)))
	{
		response.MsgHeader.Type = MT_NO_DATA;
		return response;
	}

	if (response.MsgHeader.Size)
	{
		vector<char> vMsg(response.MsgHeader.Size);
		s.Receive(&vMsg[0], (int)response.MsgHeader.Size);
		response.data = string(&vMsg[0], response.MsgHeader.Size);
	}

	return response;
}

// ���������� TRUE - ���� � ������������ ��������� ���� ���� �� 1 ������
// ���������� FALSE - ���� ��������� ������ ���������, � ���� data ������
bool SockOperations::Send(CSocket& s, Message msg)
{
	// ������ ���������� ��������� [�� ����, ����, ��� ���������, ������ ������]
	s.Send(&msg.MsgHeader, sizeof(MessageHeader));
	if (msg.MsgHeader.Size) // ���� ���� �����-�� ����� ��� ��������, �� ���������� �����
		s.Send(msg.data.c_str(), (int) msg.MsgHeader.Size);

	return msg.MsgHeader.Size > 0 ? true : false; // ���������� ���������� ��������
}