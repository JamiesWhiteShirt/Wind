#include "world.h"
#include "threads.h"

inline unsigned int toIndex(unsigned int &x, unsigned int &y, unsigned int &z)
{
	return x << 8 | y << 4 | z;
}

ChunkPosition::ChunkPosition(int x, int y, int z)
	: x(x), y(y), z(z)
{

}

BlockCount::BlockCount(unsigned int size)
	: size(size), solids(0), nonsolids(0)
{

}

bool BlockCount::isAllAir()
{
	return (solids == 0) & (nonsolids == 0);
}

bool BlockCount::isAllSolid()
{
	return solids == size;
}

bool BlockCount::isVaried()
{
	return !(((solids == 0) & (nonsolids == 0)) | (solids == 16 * 16 * 16));
}

void BlockCount::remove(unsigned short oldBlock)
{
	if(oldBlock) solids--;
}

void BlockCount::place(unsigned short newBlock)
{
	if(newBlock) solids++;
}

void BlockCount::replace(unsigned short oldBlock, unsigned short newBlock)
{
	remove(oldBlock);
	place(newBlock);
}

ChunkBase::ChunkBase(World& world, int xPos, int yPos, int zPos)
	: world(&world), pos(ChunkPosition(xPos, yPos, zPos)), loaded(false), unloaded(false), renderUpdateNeeded(false), firstPass(nullptr), secondPass(nullptr)
{

}

ChunkBase::~ChunkBase()
{

}

unsigned short* ChunkBase::dataPtr()
{
	return nullptr;
}

void ChunkBase::setLoaded()
{
	loaded = true;
}

bool ChunkBase::isLoaded()
{
	return loaded;
}

void ChunkBase::setUnloaded()
{
	unloaded = true;
}

bool ChunkBase::isUnloaded()
{
	return unloaded;
}

bool ChunkBase::shouldRender()
{
	return false;
}

void ChunkBase::setRenderUpdateNeeded(bool flag)
{
	renderUpdateNeeded = flag;
}

bool ChunkBase::isRenderUpdateNeeded()
{
	return renderUpdateNeeded;
}

geom::AxisAlignedCube ChunkBase::getBoundingBox()
{
	return geom::AxisAlignedCube(geom::Vector(pos.x, pos.y, pos.z) * 16.0f, geom::Vector(16.0f, 16.0f, 16.0f));
}



EmptyChunk::EmptyChunk(World& world, int xPos, int yPos, int zPos)
	: ChunkBase(world, xPos, yPos, zPos)
{

}

EmptyChunk::~EmptyChunk()
{

}

Block* EmptyChunk::getBlock(unsigned int x, unsigned int y, unsigned int z)
{
	return Blocks::air;
}

void EmptyChunk::setBlock(unsigned int x, unsigned int y, unsigned int z, Block* block)
{

}

void EmptyChunk::setBlockRaw(unsigned int x, unsigned int y, unsigned int z, Block* block)
{

}

bool EmptyChunk::isEmpty()
{
	return true;
}

bool EmptyChunk::shouldRender()
{
	return false;
}



Chunk::Chunk(World& world, int xPos, int yPos, int zPos)
	: ChunkBase(world, xPos, yPos, zPos), solids(0), nonSolids(0)
{

}

Chunk::~Chunk()
{

}

Block* Chunk::getBlock(unsigned int x, unsigned int y, unsigned int z)
{
	return Blocks::blockArray[data[toIndex(x, y, z)]];
}

void Chunk::setBlock(unsigned int x, unsigned int y, unsigned int z, Block* block)
{
	data[toIndex(x, y, z)] = block->id;
}

void Chunk::setBlockRaw(unsigned int x, unsigned int y, unsigned int z, Block* block)
{
	data[toIndex(x, y, z)] = block->id;
}

bool Chunk::isEmpty()
{
	return false;
}

unsigned short* Chunk::dataPtr()
{
	return data;
}

bool Chunk::shouldRender()
{
	return true;
}



World::World(unsigned int seed)
	: seed(seed)
{

}

World::~World()
{

}

void World::tick()
{
	for(auto iter = redrawQuicklyAfterTick.begin(); iter != redrawQuicklyAfterTick.end(); ++iter)
	{
		std::shared_ptr<ChunkBase> c = iter->second;
		if(!c->isEmpty())
		{
			c->setRenderUpdateNeeded(true);
			requestQuickChunkDraw(c);
		}
	}

	redrawQuicklyAfterTick.clear();
}

std::shared_ptr<ChunkBase> World::getChunk(int x, int y, int z)
{
	ChunkPosition cp = ChunkPosition(x, y, z);
	auto chunk = chunkMap.find(cp);
	if(chunk == chunkMap.end())
	{
		return shared_ptr<ChunkBase>(new EmptyChunk(GlobalThread::world, x, y, z));
	}
	else
	{
		return chunk->second;
	}
}

std::shared_ptr<ChunkBase> World::getChunkFromBlockCoordinate(int x, int y, int z)
{
	return getChunk(x >> 4, y >> 4, z >> 4);
}

bool World::isChunkLoaded(int x, int y, int z)
{
	std::shared_ptr<ChunkBase> c = getChunk(x, y, z);
	return !c->isEmpty() && c->isLoaded();
}

bool World::isChunkAtBlockCoordinateLoaded(int x, int y, int z)
{
	std::shared_ptr<ChunkBase> c = getChunkFromBlockCoordinate(x, y, z);
	return !c->isEmpty() && c->isLoaded();
}

Block* World::getBlock(int x, int y, int z)
{
	std::shared_ptr<ChunkBase> chunk = getChunkFromBlockCoordinate(x, y, z);

	if(chunk == nullptr || chunk->isEmpty() || !chunk->isLoaded())
	{
		return Blocks::air;
	}

	return chunk->getBlock(x & 0xf, y & 0xf, z & 0xf);
}

void World::setBlock(int x, int y, int z, Block* block)
{
	std::shared_ptr<ChunkBase> chunk = getChunkFromBlockCoordinate(x, y, z);

	if(!chunk->isEmpty() && chunk->isLoaded())
	{
		bool sameBlock = chunk->getBlock(x & 0xf, y & 0xf, z & 0xf) == block;
		chunk->setBlock(x & 0xf, y & 0xf, z & 0xf, block);
		if(!sameBlock)
		{
			for(int i = x - 1; i <= x + 1; i++)
			{
				for(int j = y - 1; j <= y + 1; j++)
				{
					for(int k = z - 1; k <= z + 1; k++)
					{
						std::shared_ptr<ChunkBase> c = getChunkFromBlockCoordinate(i, j, k);
						redrawQuicklyAfterTick[c->pos] = c;
					}
				}
			}
		}
	}
}

bool World::addChunk(std::shared_ptr<ChunkBase> chunk)
{
	if(chunkMap.find(chunk->pos) != chunkMap.end())
	{
		return false;
	}
	
	chunkMap[chunk->pos] = chunk;

	return true;
}

void World::removeChunk(ChunkPosition cp)
{
	chunkMap.erase(cp);
}

unsigned int World::getSeed()
{
	return seed;
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
	return (cp1.x == cp2.x) & (cp1.y == cp2.y) & (cp1.z == cp2.z);
}