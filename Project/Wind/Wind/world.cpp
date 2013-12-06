#include "world.h"

ChunkPosition::ChunkPosition(int x, int y, int z)
	: x(x), y(y), z(z)
{

}

ChunkBase::ChunkBase(World& world, int xPos, int yPos, int zPos)
	: world(&world), pos(ChunkPosition(xPos, yPos, zPos)), loaded(false), renderUpdateNeeded(false)
{

}

ChunkBase::~ChunkBase()
{

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
	data[x][y][z] = id;
}



World::World()
{

}

World::~World()
{

}

Chunk* World::getChunkFromCoordinate(int x, int y, int z)
{
	if(chunkMap.find(ChunkPosition(x >> 4, y >> 4, z >> 4)) == chunkMap.end())
	{
		return nullptr;
	}
	else
	{
		return chunkMap[ChunkPosition(x >> 4, y >> 4, z >> 4)];
	}
}

short World::getBlock(int x, int y, int z)
{
	Chunk* chunk = getChunkFromCoordinate(x, y, z);

	if(chunk == nullptr || !chunk->isLoaded())
	{
		return 0;
	}

	return chunk->getBlock(x & 0xf, y & 0xf, z & 0xf);
}

bool operator<(const ChunkPosition& cp1, const ChunkPosition& cp2)
{
	if(cp1.x < cp2.x)
	{
		return true;
	}

	if(cp1.y < cp2.y)
	{
		return true;
	}

	return cp1.z < cp2.z;
}

bool operator==(const ChunkPosition& cp1, const ChunkPosition& cp2)
{
	return (cp1.x < cp2.x) & (cp1.y < cp2.y) & (cp1.z < cp2.z);
}