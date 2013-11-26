#include <iostream>
#include <fstream>
#include "lodepng.h"

namespace IOUtil
{
	extern std::wstring EXE_DIR;

	void init();

	std::string readEntireFile(std::ifstream file);
}