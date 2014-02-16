#pragma once

#include <mutex>
#include <map>

namespace MutexPool
{
	extern std::map<std::string, std::mutex> pool;


}