#include "threads.h"
#include "ioutil.h"

int loaderThread;



Noise::NoiseGenerator2D ChunkLoadThread::minHeightNoise(8, 12, 2.0f, time(0));
Noise::NoiseGenerator2D ChunkLoadThread::maxHeightNoise(8, 12, 2.0f, time(0) + 1);
Noise::NoiseGenerator3D ChunkLoadThread::smoothnessNoise(8, 12, 2.5f, time(0) + 2);
Noise::NoiseGenerator3D ChunkLoadThread::solidNoise(8, 12, 0.0f, time(0) + 3);
Noise::NoiseGenerator3D ChunkLoadThread::stoneNoise(8, 12, 2.0f, time(0) + 4);
Noise::NoiseGenerator2D ChunkLoadThread::temperatureNoise(8, 12, 2.0f, time(0) + 5);
Noise::NoiseGenerator2D ChunkLoadThread::humidityNoise(8, 12, 2.0f, time(0) + 6);

cl::Program ChunkLoadThread::chunkGenProgram;

void ChunkLoadThread::staticInit()
{
	ChunkLoadThread::chunkGenProgram.create(IOUtil::EXE_DIR + L"\\programs\\generation.cl");
	ChunkLoadThread::minHeightNoise.prepareForCL();
	ChunkLoadThread::maxHeightNoise.prepareForCL();
	ChunkLoadThread::smoothnessNoise.prepareForCL();
	ChunkLoadThread::solidNoise.prepareForCL();
	ChunkLoadThread::stoneNoise.prepareForCL();
	ChunkLoadThread::temperatureNoise.prepareForCL();
	ChunkLoadThread::humidityNoise.prepareForCL();
}

void ChunkLoadThread::preStart()
{
	queue.create();
	blockBuffer.create(sizeof(unsigned short) * 16 * 16 * 16, CL_MEM_READ_WRITE);
	minHeightNoiseBuffer.create(sizeof(float) * 18 * 18, CL_MEM_READ_WRITE);
	maxHeightNoiseBuffer.create(sizeof(float) * 18 * 18, CL_MEM_READ_WRITE);
	smoothnessNoiseBuffer.create(sizeof(float) * 18 * 18 * 18, CL_MEM_READ_WRITE);
	solidNoiseBuffer.create(sizeof(float) * 18 * 18 * 18, CL_MEM_READ_WRITE);
	stoneNoiseBuffer.create(sizeof(float) * 18 * 18 * 18, CL_MEM_READ_WRITE);
	temperatureNoiseBuffer.create(sizeof(float) * 18 * 18, CL_MEM_READ_WRITE);
	humidityNoiseBuffer.create(sizeof(float) * 18 * 18, CL_MEM_READ_WRITE);
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

	smoothnessNoise.fillNoiseBuffer(queue, smoothnessNoiseBuffer, 18, 18, 18, chunkX * 16.0f - 1.0f, chunkY * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 16.0f, 16.0f, 16.0f);
	queue.sync();
	minHeightNoise.fillNoiseBuffer(queue, minHeightNoiseBuffer, 18, 18, 2.0f, chunkX * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 20.0f, 20.0f);
	maxHeightNoise.fillNoiseBuffer(queue, maxHeightNoiseBuffer, 18, 18, 2.0f, chunkX * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 20.0f, 20.0f);
	solidNoise.fillNoiseBufferWithSmoothness(queue, solidNoiseBuffer, 18, 18, 18, smoothnessNoiseBuffer, chunkX * 16.0f - 1.0f, chunkY * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 20.0f, 25.0f, 20.0f);
	stoneNoise.fillNoiseBuffer(queue, stoneNoiseBuffer, 18, 18, 18, 2.0f, chunkX * 16.0f - 1.0f, chunkY * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 20.0f, 20.0f, 20.0f);
	temperatureNoise.fillNoiseBuffer(queue, temperatureNoiseBuffer, 18, 18, 2.0f, chunkX * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 4.0f, 4.0f);
	humidityNoise.fillNoiseBuffer(queue, humidityNoiseBuffer, 18, 18, 2.0f, chunkX * 16.0f - 1.0f, chunkZ * 16.0f - 1.0f, 4.0f, 4.0f);
	queue.sync();

	const size_t local_ws[] = {1, 1, 1};
	const size_t global_ws[] = {16, 16, 16};

	if(!chunkGenProgram.prepare("generateTerrain_1")) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &blockBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(const int), &chunkX)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(const int), &chunkY)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(const int), &chunkZ)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &minHeightNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &maxHeightNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &smoothnessNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &solidNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &stoneNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &temperatureNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.setArgument(sizeof(cl_mem), &humidityNoiseBuffer)) GlobalThread::stop = true;
	if(!chunkGenProgram.invoke(queue, 3, global_ws, local_ws)) GlobalThread::stop = true;
	queue.sync();

	if(!blockBuffer.read(queue, chunk->dataPtr())) GlobalThread::stop = true;
	queue.sync();

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