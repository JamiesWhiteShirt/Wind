#pragma once

#include <thread>
#include <queue>
#include "world.h"
#include "list.h"
#include "task.h"
#include <mutex>
#include <memory>

namespace GlobalThread
{
	extern bool stop;
	extern World world;
};

namespace ChunkDrawThread
{
	extern std::thread thread;
	extern std::queue<std::shared_ptr<ChunkBase>> drawFirstQueue;
	extern std::queue<std::shared_ptr<ChunkBase>> drawQueue;
	extern std::mutex queueMut;
	extern VirtualList<Tasks::Task, 256, 4098> taskList;

	void loop();
};

namespace ChunkLoadThread
{
	extern std::thread thread;
	extern std::queue<std::shared_ptr<ChunkBase>> loadQueue;
	extern std::mutex queueMut;
	extern VirtualList<Tasks::Task, 256, 4098> taskList;

	void loop();
};

namespace RenderThread
{
	extern bool skipRender;
	extern std::thread thread;
	extern VirtualList<Tasks::Task, 256, 4098> taskList;

	extern gfxu::VertexStream grid;
	extern gfxu::VertexStream square;

	void loop();
};