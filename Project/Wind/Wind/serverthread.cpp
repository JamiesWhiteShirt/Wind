#include "threads.h"

using namespace ServerThread;

std::thread ServerThread::thread;
VirtualList<Tasks::Task, 256, 4098> ServerThread::taskList;

bool ServerThread::stop;

World ServerThread::world;

void ServerThread::loop()
{
	
}

bool ServerThread::start()
{
	thread = std::thread(loop);

	return thread.joinable();
}