#pragma once

#define DRAW_THREAD_AMOUNT 1
#define LOAD_THREAD_AMOUNT 1

#include <thread>
#include <queue>
#include "world.h"
#include "list.h"
#include "task.h"
#include <mutex>
#include <memory>
#include <chrono>
#include "opencl.h"

class Thread
{
private:
	std::thread thread;
	bool started;
protected:
	static void loop(Thread* theThread);
	virtual bool shouldTick();
public:
	bool quit;
	VirtualList<Tasks::Task, 256, 4098> taskList;

	virtual void preStart();
	virtual void postStop();
	virtual bool start() final;
	virtual bool stop() final;
	virtual bool tick() = 0;
};

class LimitedThread : public Thread
{
private:
	std::chrono::system_clock::time_point startTime;
	__int64 lastTick;
	int tickTime;
protected:
	virtual bool shouldTick();
public:
	LimitedThread(int tickTime);
	virtual bool tick() = 0;
};

class ChunkDrawThread : public Thread
{
protected:
	virtual bool tick();
public:
	static void staticInit();

	std::queue<std::shared_ptr<ChunkBase>> drawFirstQueue;
	std::queue<std::shared_ptr<ChunkBase>> drawQueue;
	std::queue<std::shared_ptr<ChunkBase>> drawLaterQueue;
	std::mutex queueMut;
};

class ChunkLoadThread : public Thread
{
protected:
	cl::CommandQueue queue;
	cl::Buffer blockBuffer;
	cl::Buffer minHeightNoiseBuffer;
	cl::Buffer maxHeightNoiseBuffer;
	cl::Buffer smoothnessNoiseBuffer;
	cl::Buffer solidNoiseBuffer;
	cl::Buffer stoneNoiseBuffer;
	cl::Buffer temperatureNoiseBuffer;
	cl::Buffer humidityNoiseBuffer;

	virtual void preStart();
	virtual bool tick();
public:
	static void staticInit();

	static Noise::NoiseGenerator2D minHeightNoise;
	static Noise::NoiseGenerator2D maxHeightNoise;
	static Noise::NoiseGenerator3D smoothnessNoise;
	static Noise::NoiseGenerator3D solidNoise;
	static Noise::NoiseGenerator3D stoneNoise;
	static Noise::NoiseGenerator2D temperatureNoise;
	static Noise::NoiseGenerator2D humidityNoise;
	
	static cl::Program chunkGenProgram;

	std::queue<std::shared_ptr<ChunkBase>> loadQueue;
	std::mutex queueMut;
};

class RenderThread : public Thread
{
private:
	virtual bool tick();
public:
	static gfxu::TiledTexture* blocksTexture;

	static bool skipRender;

	virtual void preStart();
	virtual void postStop();
};

extern ChunkDrawThread chunkDrawThreads[DRAW_THREAD_AMOUNT];
extern ChunkLoadThread chunkLoadThreads[LOAD_THREAD_AMOUNT];
extern RenderThread renderThread;

extern void requestQuickChunkDraw(std::shared_ptr<ChunkBase> chunk);
extern void requestChunkDraw(std::shared_ptr<ChunkBase> chunk);
extern void requestLateChunkDraw(std::shared_ptr<ChunkBase> chunk);

extern void requestChunkLoad(std::shared_ptr<ChunkBase> chunk);

namespace GlobalThread
{
	extern bool stop;
	extern World world;
};

namespace ServerThread
{
	extern std::thread thread;
	extern VirtualList<Tasks::Task, 256, 4098> taskList;
	
	extern bool stop;

	extern World world;

	void loop();
	bool start();
};