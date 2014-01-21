#include "threads.h"

bool GlobalThread::stop = false;
World GlobalThread::world;

std::thread ChunkDrawThread::thread;
std::queue<std::shared_ptr<ChunkBase>> ChunkDrawThread::drawFirstQueue;
std::queue<std::shared_ptr<ChunkBase>> ChunkDrawThread::drawQueue;
std::mutex ChunkDrawThread::queueMut;
VirtualList<Tasks::Task, 256, 4098> ChunkDrawThread::taskList;

void ChunkDrawThread::loop()
{
	while(!GlobalThread::stop)
	{
		std::shared_ptr<ChunkBase> chunk = nullptr;
		ChunkDrawThread::queueMut.lock();
		if(!ChunkDrawThread::drawFirstQueue.empty())
		{
			chunk = ChunkDrawThread::drawFirstQueue.front();
			ChunkDrawThread::drawFirstQueue.pop();
		}
		else if(!ChunkDrawThread::drawQueue.empty())
		{
			chunk = ChunkDrawThread::drawQueue.front();
			ChunkDrawThread::drawQueue.pop();
		}
		ChunkDrawThread::queueMut.unlock();

		if(chunk != nullptr && chunk->isLoaded() && chunk->isRenderUpdateNeeded())
		{
			/*bool flag = true;
			for(int i = -1; (i < 2) & flag; i++)
			{
				for(int j = -1; (j < 2) & flag; j++)
				{
					for(int k = -1; (k < 2) & flag; k++)
					{
						if(!GlobalThread::world.isChunkLoaded(chunk->pos.x + i, chunk->pos.y + j, chunk->pos.z + k))
						{
							flag = false;
						}
					}
				}
			}

			if(!flag)
			{
				continue;
			}*/

			gfxu::VertexStream& vStream = *(new gfxu::VertexStream(8192));
			int cx = chunk->pos.x * 16;
			int cy = chunk->pos.y * 16;
			int cz = chunk->pos.z * 16;
				
			for(unsigned int i = 0; i < 16; i++)
			{
				for(unsigned int j = 0; j < 16; j++)
				{
					for(unsigned int k = 0; k < 16; k++)
					{
						if(GlobalThread::world.getBlock(i | cx, j | cy, k | cz))
						{
							if(!GlobalThread::world.getBlock(i | cx, j | cy, (k | cz) - 1))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(GlobalThread::world.getBlock((i | cx) - 1, j | cy, (k | cz) - 1))
								{
									i1++;
									i4++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, j | cy, (k | cz) - 1))
								{
									i2++;
									i3++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) + 1, (k | cz) - 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) - 1))
								{
									i3++;
								}
								
								if(i4 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) - 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
							}
					
							if(!GlobalThread::world.getBlock(i | cx, j | cy, (k | cz) + 1))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(GlobalThread::world.getBlock((i | cx) - 1, j | cy, (k | cz) + 1))
								{
									i1++;
									i4++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) - 1, (k | cz) + 1))
								{
									i1++;
									i2++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, j | cy, (k | cz) + 1))
								{
									i2++;
									i3++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) + 1))
								{
									i1++;
								}

								if(i2 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) + 1))
								{
									i2++;
								}
								
								if(i3 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
							}

							if(!GlobalThread::world.getBlock(i | cx, (j | cy) - 1, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) - 1, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
							}

							if(!GlobalThread::world.getBlock(i | cx, (j | cy) + 1, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) + 1, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(GlobalThread::world.getBlock(i | cx, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
							}

							if(!GlobalThread::world.getBlock((i | cx) - 1, j | cy, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(GlobalThread::world.getBlock((i | cx) - 1, j | cy, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(GlobalThread::world.getBlock((i | cx) - 1, j | cy, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && GlobalThread::world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 0.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 0.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, f4, f4, f4, 1.0f);
							}

							if(!GlobalThread::world.getBlock((i | cx) + 1, j | cy, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, j | cy, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(GlobalThread::world.getBlock((i | cx) + 1, j | cy, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && GlobalThread::world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 0.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 0.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << gfxu::VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, f1, f1, f1, 1.0f);
							}
						}
					}
				}
			}

			vStream.compress();
			
			chunk->mut.lock();
			chunk->setRenderUpdateNeeded(false);
			RenderThread::taskList.put(Tasks::TaskDelete<gfxu::VertexStream>(chunk->drawStream));
			chunk->drawStream = &vStream;
			chunk->swapStreams();
			chunk->mut.unlock();
		}
	}
}

std::thread ChunkLoadThread::thread;
std::queue<std::shared_ptr<ChunkBase>> ChunkLoadThread::loadQueue;
std::mutex ChunkLoadThread::queueMut;
VirtualList<Tasks::Task, 256, 4098> ChunkLoadThread::taskList;

void ChunkLoadThread::loop()
{
	Noise::NoiseGenerator3D noise(4, 8, 2.5f, 3);
	Noise::NoiseGenerator2D noise2(4, 8, 2.5f, 2);

	while(!GlobalThread::stop)
	{
		if(ChunkLoadThread::loadQueue.empty())
		{
			continue;
		}

		std::shared_ptr<ChunkBase> chunk = ChunkLoadThread::loadQueue.front();
		ChunkLoadThread::loadQueue.pop();

		for(unsigned int i = 0; i < 16; i++)
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
						float n2 = noise.getNoise(x / 2.0, y / 4.0f, z / 2.0, n1);
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
		}

		chunk->setLoaded();
		
		for(int i = -1; i <= 1; i++)
		{
			for(int j = -1; j <= 1; j++)
			{
				for(int k = -1; k <= 1; k++)
				{
					std::shared_ptr<ChunkBase> c = GlobalThread::world.getChunk(i + chunk->pos.x, j + chunk->pos.y, k + chunk->pos.z);
					if(c != nullptr && c->isLoaded())
					{
						c->setRenderUpdateNeeded(true);
						ChunkDrawThread::queueMut.lock();
						ChunkDrawThread::drawQueue.push(c);
						ChunkDrawThread::queueMut.unlock();
					}
				}
			}
		}
	}
}