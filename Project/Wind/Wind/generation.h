#pragma once

#include "world.h"
#include "geometry.h"
#include "globalutil.h"

class CaveNode
{
public:
	geom::Vector pos;

	CaveNode();
	CaveNode(geom::Vector pos);
};

class Cave
{
private:

public:
	const ChunkPosition chunkPos;
	const int length;
	geom::AxisAlignedCube boundingBox;
	CaveNode* nodes;
	Cave();
	Cave(ChunkPosition chunkPos, Random& random);
	~Cave();
};

class CaveCache
{
private:
	static std::unordered_map<ChunkPosition, std::shared_ptr<Cave>> generatedCaves;
	static std::unordered_map<ChunkPosition, std::vector<std::shared_ptr<Cave>>> caves;
	static std::shared_ptr<Cave> nullCave;

	static void makeCave(ChunkPosition chunkPos);

public:
	static std::vector<std::shared_ptr<Cave>> getCavesInRegion(ChunkPosition chunkPos);
};