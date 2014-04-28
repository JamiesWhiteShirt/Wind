#include "threads.h"

int drawerThread = 0;

bool renderChunk(std::shared_ptr<ChunkBase> chunk)
{
	World* world = &GlobalThread::world;

	chunk->mutex.lock();
	if(!chunk->shouldRender() || !chunk->isRenderUpdateNeeded() || chunk->isUnloaded())
	{
		chunk->mutex.unlock();
		return true;
	}
	chunk->setRenderUpdateNeeded(false);
	chunk->mutex.unlock();

	for(int i = -1; i < 2; i++)
	{
		for(int j = -1; j < 2; j++)
		{
			for(int k = -1; k < 2; k++)
			{
				std::shared_ptr<ChunkBase> c = GlobalThread::world.getChunk(chunk->pos.x + i, chunk->pos.y + j, chunk->pos.z + k);
				if(!c->isEmpty() && !c->isLoaded())
				{
					return false;
				}
			}
		}
	}

	const int cx = chunk->pos.x * 16;
	const int cy = chunk->pos.y * 16;
	const int cz = chunk->pos.z * 16;

	gfxu::VertexStream* firstPass = new gfxu::VertexStream(8192);
	gfxu::VertexStream* secondPass = new gfxu::VertexStream(8192);
	for(unsigned int i = 0; i < 16; i++)
	{
		for(unsigned int j = 0; j < 16; j++)
		{
			for(unsigned int k = 0; k < 16; k++)
			{
				firstPass->setTranslation(i, j, k);
				firstPass->setColor(255, 255, 255, 255);

				secondPass->setTranslation(i, j, k);
				secondPass->setColor(255, 255, 255, 255);

				const int x = i | cx;
				const int y = j | cy;
				const int z = k | cz;

				Block* block = GlobalThread::world.getBlock(x, y, z);
				
				block->firstPassRenderer(world, x, y, z, block, firstPass);
				block->secondPassRenderer(world, x, y, z, block, secondPass);
			}
		}
	}
	firstPass->compress();
	secondPass->compress();
	
	chunk->renderMutex.lock();
	std::shared_ptr<gfxu::VertexStream> firstPassPtr(firstPass);
	std::shared_ptr<gfxu::VertexStream> secondPassPtr(secondPass);
	chunk->firstPass = firstPassPtr;
	chunk->secondPass = secondPassPtr;
	chunk->renderMutex.unlock();

	return true;
}

bool renderFirstPossible(std::queue<std::shared_ptr<ChunkBase>>& q)
{
	for(unsigned int i = 0; !GlobalThread::stop && (i < q.size()); i++)
	{
		std::shared_ptr<ChunkBase> chunk = q.front();
		q.pop();
		if(renderChunk(chunk))
		{
			return true;
		}
		else
		{
			q.push(chunk);
		}
	}
	return false;
}

bool ChunkDrawThread::tick()
{
	ChunkDrawThread::queueMut.lock();
	bool rendered = renderFirstPossible(ChunkDrawThread::drawFirstQueue) || renderFirstPossible(ChunkDrawThread::drawQueue) || renderFirstPossible(ChunkDrawThread::drawLaterQueue);
	ChunkDrawThread::queueMut.unlock();

	return true;
}

void requestQuickChunkDraw(std::shared_ptr<ChunkBase> chunk)
{
	ChunkDrawThread* thread = &chunkDrawThreads[(drawerThread++) % DRAW_THREAD_AMOUNT];

	thread->queueMut.lock();
	thread->drawFirstQueue.push(chunk);
	thread->queueMut.unlock();
}

void requestChunkDraw(std::shared_ptr<ChunkBase> chunk)
{
	ChunkDrawThread* thread = &chunkDrawThreads[(drawerThread++) % DRAW_THREAD_AMOUNT];

	thread->queueMut.lock();
	thread->drawQueue.push(chunk);
	thread->queueMut.unlock();
}

void requestLateChunkDraw(std::shared_ptr<ChunkBase> chunk)
{
	ChunkDrawThread* thread = &chunkDrawThreads[(drawerThread++) % DRAW_THREAD_AMOUNT];

	thread->queueMut.lock();
	thread->drawLaterQueue.push(chunk);
	thread->queueMut.unlock();
}