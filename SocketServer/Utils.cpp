#include "pch.h"
#include "Utils.h"

vector<string> Utils::SplitCommand(string cmd, string delimiter)
{
	vector<string> result;
	string currentString;
	for (int i = 0; i < cmd.length(); i++)
	{
		if (cmd.rfind(delimiter, i) != std::string::npos)
		{
			result.push_back(currentString);
			int offset = i + 1;
			string text = cmd.substr(offset, cmd.length() - i);
			result.push_back(text);
			break;
		}
		else currentString.append(&cmd[i]);
	}
	return result;
}

long long Utils::GetCurrentTimeSec()
{
	std::time_t result = std::time(nullptr);
	return result;
}
