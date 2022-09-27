#ifndef UTILS_H
#define UTILS_H

#include "pch.h"
#include <vector>
#include <string>

using namespace std;

class Utils
{
public:
	/// Метод для разбива строки с сообщением на состовляющие
	/// Предполагается получение команды по шаблону: userID:message
	/// Результатом метода будет вектор строк:
	/// [1] userID
	/// [2] message
	static vector<string> SplitCommand(string cmd, string delimiter);
};

#endif