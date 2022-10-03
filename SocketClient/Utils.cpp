#include "pch.h"
#include "Utils.h"

vector<string> Utils::Split(string baseString, string delimiter)
{
	vector<string> result;

	string splittedString = "";
	for (int i = 0; i < baseString.length(); i++)
	{
		if (baseString[i] == delimiter[0])
		{
			result.push_back(splittedString);
			splittedString = "";
			continue;
		}
		splittedString += baseString[i];
	}

	if (splittedString.length() > 0)
		result.push_back(splittedString);

	return result;
}

vector<string> Utils::SplitCommand(string cmd, string delimiter)
{
	vector<string> result;
	string currentString;
	int messageStartIndex = -1; // Индекс в строке 'cmd', с которого начинается сообщение для отправки
	for (int i = 0; i < cmd.length(); i++)
	{
		if (cmd.rfind(delimiter, i) != std::string::npos)
		{
			result.push_back(currentString);
			currentString = "";
			messageStartIndex = i + 1;
			break;
		}
		currentString += cmd[i];
	}

	for (int k = messageStartIndex; k < cmd.length(); k++)
		currentString += cmd[k];

	result.push_back(currentString);

	return result;
}

long long Utils::GetCurrentTimeSec()
{
	std::time_t result = std::time(nullptr);
	return result;
}
