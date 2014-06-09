#include <iostream>
#include <fstream>
#include "lodepng.h"

namespace IOUtil
{
	extern std::wstring EXE_DIR;

	void staticInit();

	std::string readEntireFile(std::ifstream file);

	unsigned char* readPNG(std::wstring file, unsigned int &width, unsigned int &height);
}