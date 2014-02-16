#pragma once

#include <map>
#include "graphics.h"
#include <mutex>
#include <queue>
#include <mutex>
#include <memory>

class World;

class ChunkPosition
{
public:
	int x, y, z;
	ChunkPosition(int x, int y, int z);
};

class ChunkBase
{
private:
	World* world;
	bool renderUpdateNeeded;
	bool loaded;
	bool unloaded;
public:
	ChunkPosition pos;
	std::mutex mut;
	gfxu::VertexStream* renderStream;
	gfxu::VertexStream* drawStream;
	int surroundingExistingChunks;

	ChunkBase(World& world, int xPos, int yPos, int zPos);
	~ChunkBase();

	virtual short getBlock(int x, int y, int z) = 0;
	virtual void setBlock(int x, int y, int z, short id) = 0;
	virtual void setBlockRaw(int x, int y, int z, short id) = 0;
	virtual bool isEmpty() = 0;

	void setLoaded();
	bool isLoaded();
	void setUnloaded();
	bool isUnloaded();

	void setRenderUpdateNeeded(bool flag);
	bool isRenderUpdateNeeded();
	void swapStreams();
};

class EmptyChunk : public ChunkBase
{
public:
	EmptyChunk(World& world, int xPos, int yPos, int zPos);
	~EmptyChunk();
	short getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, short id);
	void setBlockRaw(int x, int y, int z, short id);
	bool isEmpty();
};

class Chunk : public ChunkBase
{
private:
public:
	short data[16][16][16];

	Chunk(World& world, int xPos, int yPos, int zPos);
	~Chunk();
	short getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, short id);
	void setBlockRaw(int x, int y, int z, short id);
	bool isEmpty();
};

class World
{
private:
	int incr(int x, int y, int z);
public:
	std::mutex chunkMapLock;
	std::map<ChunkPosition, std::shared_ptr<ChunkBase>> chunkMap;

	std::mutex additionQueueLock;
	std::queue<std::shared_ptr<ChunkBase>> additionQueue;
	std::mutex removalQueueLock;
	std::queue<std::shared_ptr<ChunkBase>> removalQueue;

	World();
	~World();

	std::shared_ptr<ChunkBase> getChunk(int x, int y, int z);
	std::shared_ptr<ChunkBase> getChunkFromBlockCoordinate(int x, int y, int z);
	bool isChunkLoaded(int x, int y, int z);
	bool isChunkAtBlockCoordinateLoaded(int x, int y, int z);
	short getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, short id);

	bool addChunk(std::shared_ptr<ChunkBase> chunk);
	void removeChunk(ChunkPosition cp);
};

bool operator<(const ChunkPosition& cp1, const ChunkPosition& cp2);
bool operator==(const ChunkPosition& cp1, const ChunkPosition& cp2);