#include "ioutil.h"
#include <Windows.h>

using namespace IOUtil;

std::wstring IOUtil::EXE_DIR;

void IOUtil::init()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	int pos = std::wstring(buffer).find_last_of(L"\\/");
	buffer[pos] = 0;
	EXE_DIR = buffer;
}

std::string IOUtil::readEntireFile(std::ifstream file)
{
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}