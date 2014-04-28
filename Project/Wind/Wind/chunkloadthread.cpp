#include "threads.h"
#include "ioutil.h"

int loaderThread;

Noise::NoiseGenerator3D ChunkLoadThread::noise(4, 8, 2.5, time(0));
Noise::NoiseGenerator3D ChunkLoadThread::noise2(4, 8, 2.5, time(0) + 50);

cl::Program ChunkLoadThread::program;
cl::Buffer ChunkLoadThread::noiseBuffer;
cl::Buffer ChunkLoadThread::noise2Buffer;

void ChunkLoadThread::preStart()
{
	queue.create();
	blockBuffer.create(sizeof(unsigned short) * 16 * 16 * 16, CL_MEM_READ_WRITE);
}

bool ChunkLoadThread::tick()
{
	if(ChunkLoadThread::loadQueue.empty())
	{
		return true;
	}

	std::shared_ptr<ChunkBase> chunk = ChunkLoadThread::loadQueue.front();
	ChunkLoadThread::loadQueue.pop();

	const int chunkX = chunk->pos.x;
	const int chunkY = chunk->pos.y;
	const int chunkZ = chunk->pos.z;

	const size_t local_ws[] = {1, 1, 1};
	const size_t global_ws[] = {16, 16, 16};

	if(!program.prepare("generateTerrain_p1")) GlobalThread::stop = true;
	if(!program.setArgument(0, sizeof(cl_mem), &blockBuffer)) GlobalThread::stop = true;
	if(!program.setArgument(1, sizeof(const int), &chunkX)) GlobalThread::stop = true;
	if(!program.setArgument(2, sizeof(const int), &chunkY)) GlobalThread::stop = true;
	if(!program.setArgument(3, sizeof(const int), &chunkZ)) GlobalThread::stop = true;
	if(!program.setArgument(4, sizeof(cl_mem), &noiseBuffer)) GlobalThread::stop = true;
	if(!program.setArgument(5, sizeof(cl_mem), &noise2Buffer)) GlobalThread::stop = true;
	if(!program.invoke(queue, 3, global_ws, local_ws)) GlobalThread::stop = true;

	if(!blockBuffer.read(queue, chunk->dataPtr())) GlobalThread::stop = true;

	/*for(unsigned int i = 0; i < 16; i++)
	{
		int x = i + chunk->pos.x * 16;
		for(unsigned int k = 0; k < 16; k++)
		{
			int z = k + chunk->pos.z * 16;
			float n1 = 2.0f + noise2.getNoise(x, z);
			for(unsigned int j = 0; j < 16; j++)
			{
				int y = j + chunk->pos.y * 16;
				if(y < -32)
				{
					chunk->setBlockRaw(i, j, k, 1);
				}
				else if(y >= 64)
				{
					chunk->setBlockRaw(i, j, k, 0);
				}
				else
				{
					float n2 = noise.getNoise(x, y * 2.0, z, n1);
					if((y + 32) / 96.0f < n2)
					{
						chunk->setBlockRaw(i, j, k, 1);
					}
					else
					{
						if(y < 0)
						{
							chunk->setBlockRaw(i, j, k, 0);
						}
						else
						{
							chunk->setBlockRaw(i, j, k, 0);
						}
					}
				}
			}
		}
	}*/

	chunk->mutex.lock();
	chunk->setLoaded();
	chunk->setRenderUpdateNeeded(true);
	chunk->mutex.unlock();
	requestChunkDraw(chunk);
		
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			for(int k = -1; k <= 1; k++)
			{
				if(i | j | k)
				{
					std::shared_ptr<ChunkBase> c = GlobalThread::world.getChunk(i + chunk->pos.x, j + chunk->pos.y, k + chunk->pos.z);
					if(!c->isEmpty() && c->isLoaded() && !c->isRenderUpdateNeeded())
					{
						c->setRenderUpdateNeeded(true);
						requestLateChunkDraw(c);
					}
				}
			}
		}
	}

	return true;
}

void requestChunkLoad(std::shared_ptr<ChunkBase> chunk)
{
	ChunkLoadThread* thread = &chunkLoadThreads[(loaderThread++) % LOAD_THREAD_AMOUNT];

	thread->queueMut.lock();
	thread->loadQueue.push(chunk);
	thread->queueMut.unlock();
}