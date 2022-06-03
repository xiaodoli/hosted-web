#include "FileUtil.h"

#include <Windows.h>
#include <vector>
#include <filesystem>
#include <fstream>

std::string FilePathUtil::GetExecutableFilePath()
{
	typedef std::vector<char> char_vector;
	typedef std::vector<char>::size_type size_type;
	DWORD size = 1024;
	char_vector buf(size, 0);
	bool havePath = false;
	bool shouldContinue = true;

	while (shouldContinue)
	{
		DWORD result = GetModuleFileNameA(nullptr, &buf[0], size);
		const DWORD lastError = GetLastError();
		if (result == 0)
		{
			shouldContinue = false;
		}
		else if (result < size)
		{
			havePath = true;
			shouldContinue = false;
		}
		else if (result == size && (lastError == ERROR_INSUFFICIENT_BUFFER || lastError == ERROR_SUCCESS))
		{
			size *= 2;
			buf.resize(size);
		}
		else
		{
			shouldContinue = false;
		}
	}

	if (havePath)
	{
		return &buf[0];
	}

	return "";
}

std::string FilePathUtil::GetCurrentFolder()
{
	const std::string currentExecutableFilePath = GetExecutableFilePath();
	const std::string currentFolder = currentExecutableFilePath.substr(0, currentExecutableFilePath.find_last_of('\\'));
	return currentFolder;
}
