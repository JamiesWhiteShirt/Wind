#include "threads.h"

ChunkDrawThread chunkDrawThreads[DRAW_THREAD_AMOUNT];
ChunkLoadThread chunkLoadThreads[LOAD_THREAD_AMOUNT];
RenderThread renderThread;

void Thread::loop(Thread* theThread)
{
	theThread->preStart();
	while(!theThread->quit)
	{
		for(int i = 0; i < theThread->taskList.getSize(); i++)
		{
			if(!theThread->taskList[i]->invoke())
			{
				GlobalThread::stop = true;
			}
		}

		if(theThread->shouldTick())
		{
			if(!theThread->tick()) break;
		}
	}
	theThread->postStop();
}

bool Thread::shouldTick()
{
	return true;
}

bool Thread::start()
{
	started = true;
	quit = false;
	Thread* theThread = this;
	thread = std::thread(&Thread::loop, std::ref(theThread));
	return thread.joinable();
}

bool Thread::stop()
{
	quit = true;
	if(started) thread.join();
	return true;
}

void Thread::preStart()
{

}

void Thread::postStop()
{

}

LimitedThread::LimitedThread(int tickTime)
	: tickTime(tickTime), lastTick(0)
{
	startTime = std::chrono::high_resolution_clock::now();
}

bool LimitedThread::shouldTick()
{
	std::chrono::system_clock::time_point now = std::chrono::high_resolution_clock::now();
	long long difference = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime).count();

	__int64 tick = difference / tickTime;
	__int64 ticksToProcess = tick - lastTick;
	lastTick = tick;

	if(ticksToProcess > 0)
	{
		lastTick++;
		return true;
	}
	return false;
}



bool GlobalThread::stop = false;
World GlobalThread::world(time(0));