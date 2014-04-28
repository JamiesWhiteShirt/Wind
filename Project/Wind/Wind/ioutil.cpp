#include "ioutil.h"
#include <Windows.h>
#include "window.h"

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

unsigned char* IOUtil::readPNG(std::wstring file, unsigned int &width, unsigned int &height)
{
	char* nFileName = new char[file.length() + 1];
	std::wcstombs(nFileName, file.c_str(), sizeof(char) * (file.length() + 1));
	std::vector<unsigned char> image;
	if(!lodepng::decode(image, width, height, nFileName))
	{
		unsigned char* data = new unsigned char[width * height * 4];

		for(unsigned int i = 0; i < height; i++)
		{
			memcpy(&data[i * width * 4], &image[(height - 1 - i) * width * 4], sizeof(unsigned char) * width * 4);
		}

		return data;
	}

	GLWindow::instance->postError("PNG file not found.", "PNG load error");

	return nullptr;
}