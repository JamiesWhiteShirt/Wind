#pragma once

#include <unordered_map>
#include "graphics.h"
#include <mutex>
#include <queue>
#include <mutex>
#include <memory>
#include "blocks.h"

class World;

class ChunkPosition
{
public:
	int x, y, z;
	ChunkPosition(int x, int y, int z);
};

namespace std
{
	template <>
	struct hash<ChunkPosition>
	{
		std::size_t operator()(const ChunkPosition& key) const
		{
			using std::size_t;
			using std::hash;

			return (hash<int>()(key.x) >> 8) ^ hash<int>()(key.y) ^ (hash<int>()(key.z) << 8);
		}
	};
};

class BlockCount
{
private:
	unsigned int size;
	unsigned int solids;
	unsigned int nonsolids;
public:
	BlockCount(unsigned int size);
	bool isAllAir();
	bool isAllSolid();
	bool isVaried();
	void remove(unsigned short oldBlock);
	void place(unsigned short newBlock);
	void replace(unsigned short oldBlock, unsigned short newBlock);
};

class ChunkBase
{
private:
	bool renderUpdateNeeded;
	bool loaded;
	bool unloaded;
protected:
	World* world;
public:
	ChunkPosition pos;
	std::mutex mutex;
	std::mutex renderMutex;
	std::shared_ptr<gfxu::VertexStream> firstPass;
	std::shared_ptr<gfxu::VertexStream> secondPass;

	ChunkBase(World& world, int xPos, int yPos, int zPos);
	~ChunkBase();

	virtual Block* getBlock(unsigned int x, unsigned int y, unsigned int z) = 0;
	virtual void setBlock(unsigned int x, unsigned int y, unsigned int z, Block* block) = 0;
	virtual void setBlockRaw(unsigned int x, unsigned int y, unsigned int z, Block* block) = 0;
	virtual bool isEmpty() = 0;
	virtual unsigned short* dataPtr();

	void setLoaded();
	bool isLoaded();
	void setUnloaded();
	bool isUnloaded();
	virtual bool shouldRender();

	void setRenderUpdateNeeded(bool flag);
	bool isRenderUpdateNeeded();

	geom::AxisAlignedCube getBoundingBox();
};

class EmptyChunk : public ChunkBase
{
public:
	EmptyChunk(World& world, int xPos, int yPos, int zPos);
	~EmptyChunk();
	Block* getBlock(unsigned int x, unsigned int y, unsigned int z);
	void setBlock(unsigned int x, unsigned int y, unsigned int z, Block* block);
	void setBlockRaw(unsigned int x, unsigned int y, unsigned int z, Block* block);
	bool isEmpty();
	bool shouldRender();
};

class Chunk : public ChunkBase
{
private:
	unsigned int solids;
	unsigned int nonSolids;
public:
	unsigned short data[16 * 16 * 16];

	Chunk(World& world, int xPos, int yPos, int zPos);
	~Chunk();
	Block* getBlock(unsigned int x, unsigned int y, unsigned int z);
	void setBlock(unsigned int x, unsigned int y, unsigned int z, Block* block);
	void setBlockRaw(unsigned int x, unsigned int y, unsigned int z, Block* block);
	bool isEmpty();
	virtual unsigned short* dataPtr();
	bool shouldRender();
};

class World
{
private:
	std::unordered_map<ChunkPosition, std::shared_ptr<ChunkBase>> redrawQuicklyAfterTick;
	unsigned int seed;
public:
	std::mutex chunkMapLock;
	std::unordered_map<ChunkPosition, std::shared_ptr<ChunkBase>> chunkMap;

	std::mutex additionQueueLock;
	std::queue<std::shared_ptr<ChunkBase>> additionQueue;
	std::mutex removalQueueLock;
	std::queue<std::shared_ptr<ChunkBase>> removalQueue;

	World(unsigned int seed);
	~World();

	void tick();

	std::shared_ptr<ChunkBase> getChunk(int x, int y, int z);
	std::shared_ptr<ChunkBase> getChunkFromBlockCoordinate(int x, int y, int z);
	bool isChunkLoaded(int x, int y, int z);
	bool isChunkAtBlockCoordinateLoaded(int x, int y, int z);
	Block* getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block* block);

	bool addChunk(std::shared_ptr<ChunkBase> chunk);
	void removeChunk(ChunkPosition cp);

	unsigned int getSeed();
};

bool operator<(const ChunkPosition& cp1, const ChunkPosition& cp2);
bool operator==(const ChunkPosition& cp1, const ChunkPosition& cp2);