#include <map>
#include "graphics.h"
#include <mutex>
#include <queue>
#include <mutex>

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
public:
	ChunkPosition pos;
	std::mutex mut;
	gfxu::VertexStream* renderStream;
	gfxu::VertexStream* drawStream;

	ChunkBase(World& world, int xPos, int yPos, int zPos);
	~ChunkBase();

	virtual short getBlock(int x, int y, int z) = 0;
	virtual void setBlock(int x, int y, int z, short id) = 0;
	virtual void setBlockRaw(int x, int y, int z, short id) = 0;

	void setLoaded();
	bool isLoaded();

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
};

class World
{
private:
public:
	std::mutex chunkMapLock;
	std::map<ChunkPosition, ChunkBase*> chunkMap;
	std::queue<ChunkBase*> additionQueue;

	World();
	~World();

	ChunkBase* getChunk(int x, int y, int z);
	ChunkBase* getChunkFromBlockCoordinate(int x, int y, int z);
	bool isChunkLoaded(int x, int y, int z);
	bool isChunkAtBlockCoordinateLoaded(int x, int y, int z);
	short getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, short id);
};

bool operator<(const ChunkPosition& cp1, const ChunkPosition& cp2);
bool operator==(const ChunkPosition& cp1, const ChunkPosition& cp2);