#include "world.h"
#include "threads.h"

ChunkPosition::ChunkPosition(int x, int y, int z)
	: x(x), y(y), z(z)
{

}

ChunkBase::ChunkBase(World& world, int xPos, int yPos, int zPos)
	: world(&world), pos(ChunkPosition(xPos, yPos, zPos)), loaded(false), renderUpdateNeeded(false), renderStream(new gfxu::VertexStream()), drawStream(new gfxu::VertexStream())
{

}

ChunkBase::~ChunkBase()
{
	delete renderStream;
	delete drawStream;
}

void ChunkBase::setLoaded()
{
	loaded = true;
}

bool ChunkBase::isLoaded()
{
	return loaded;
}

void ChunkBase::setRenderUpdateNeeded(bool flag)
{
	renderUpdateNeeded = flag;
}

bool ChunkBase::isRenderUpdateNeeded()
{
	return renderUpdateNeeded;
}

void ChunkBase::swapStreams()
{
	gfxu::VertexStream* temp = renderStream;
	renderStream = drawStream;
	drawStream = temp;
}



EmptyChunk::EmptyChunk(World& world, int xPos, int yPos, int zPos)
	: ChunkBase(world, xPos, yPos, zPos)
{

}

EmptyChunk::~EmptyChunk()
{

}

short EmptyChunk::getBlock(int x, int y, int z)
{
	return 0;
}

void EmptyChunk::setBlock(int x, int y, int z, short id)
{

}

void EmptyChunk::setBlockRaw(int x, int y, int z, short id)
{

}



Chunk::Chunk(World& world, int xPos, int yPos, int zPos)
	: ChunkBase(world, xPos, yPos, zPos)
{

}

Chunk::~Chunk()
{

}

short Chunk::getBlock(int x, int y, int z)
{
	return data[x][y][z];
}

void Chunk::setBlock(int x, int y, int z, short id)
{
	setBlockRaw(x, y, z, id);
}

void Chunk::setBlockRaw(int x, int y, int z, short id)
{
	data[x][y][z] = id;
}



World::World()
{

}

World::~World()
{

}

std::shared_ptr<ChunkBase> World::getChunk(int x, int y, int z)
{
	ChunkPosition cp = ChunkPosition(x, y, z);
	if(chunkMap.find(cp) == chunkMap.end())
	{
		return nullptr;
	}
	else
	{
		return chunkMap[cp];
	}
}

std::shared_ptr<ChunkBase> World::getChunkFromBlockCoordinate(int x, int y, int z)
{
	return getChunk(x >> 4, y >> 4, z >> 4);
}

bool World::isChunkLoaded(int x, int y, int z)
{
	std::shared_ptr<ChunkBase> c = getChunk(x, y, z);
	return !(c == nullptr || !c->isLoaded());
}

bool World::isChunkAtBlockCoordinateLoaded(int x, int y, int z)
{
	std::shared_ptr<ChunkBase> c = getChunkFromBlockCoordinate(x, y, z);
	return !(c == nullptr || !c->isLoaded());
}

short World::getBlock(int x, int y, int z)
{
	std::shared_ptr<ChunkBase> chunk = getChunkFromBlockCoordinate(x, y, z);

	if(chunk == nullptr || !chunk->isLoaded())
	{
		return 0;
	}

	return chunk->getBlock(x & 0xf, y & 0xf, z & 0xf);
}

void World::setBlock(int x, int y, int z, short id)
{
	std::shared_ptr<ChunkBase> chunk = getChunkFromBlockCoordinate(x, y, z);

	if(chunk != nullptr && chunk->isLoaded())
	{
		bool sameBlock = chunk->getBlock(x & 0xf, y & 0xf, z & 0xf) == id;
		chunk->setBlock(x & 0xf, y & 0xf, z & 0xf, id);
		if(!sameBlock)
		{
			ChunkDrawThread::queueMut.lock();
			for(int i = x - 1; i <= x + 1; i++)
			{
				for(int j = y - 1; j <= y + 1; j++)
				{
					for(int k = z - 1; k <= z + 1; k++)
					{
						std::shared_ptr<ChunkBase> c = getChunkFromBlockCoordinate(i, j, k);
						if(c != nullptr)
						{
							c->setRenderUpdateNeeded(true);
							ChunkDrawThread::drawFirstQueue.push(c);
						}
					}
				}
			}
			ChunkDrawThread::queueMut.unlock();
		}
	}
}

bool operator<(const ChunkPosition& cp1, const ChunkPosition& cp2)
{
	if(cp1.x < cp2.x)
	{
		return true;
	}
	else if(cp1.x == cp2.x)
	{
		if(cp1.y < cp2.y)
		{
			return true;
		}
		else if(cp1.y == cp2.y)
		{
			return cp1.z < cp2.z;
		}
	}

	return false;
}

bool operator==(const ChunkPosition& cp1, const ChunkPosition& cp2)
{
	return (cp1.x < cp2.x) & (cp1.y < cp2.y) & (cp1.z < cp2.z);
}