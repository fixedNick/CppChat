#ifndef UTILS_H
#define UTILS_H

#include "pch.h"
#include <vector>
#include <string>

using namespace std;

class Utils
{
public:
	/// ����� ��� ������� ������ � ���������� �� ������������
	/// �������������� ��������� ������� �� �������: userID:message
	/// ����������� ������ ����� ������ �����:
	/// [1] userID
	/// [2] message
	static vector<string> SplitCommand(string cmd, string delimiter);
};

#endif