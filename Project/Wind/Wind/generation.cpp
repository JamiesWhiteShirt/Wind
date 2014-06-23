#include "threads.h"
#include "generation.h"
#include "noise.h"

CaveNode::CaveNode()
{

}

CaveNode::CaveNode(geom::Vector pos)
	: pos(pos)
{

}

Cave::Cave()
	: chunkPos(0, 0, 0), length(0), nodes(nullptr)
{

}

Cave::Cave(ChunkPosition chunkPos, Random& random)
	: chunkPos(chunkPos), length(random.getUint(16, 48)), nodes(new CaveNode[length + 1])
	//:chunkPos(chunkPos), length(1), nodes(new CaveNode[length + 1])
{
	Noise::NoiseGenerator1D xNoise(5, 8, 2.0f, random);
	Noise::NoiseGenerator1D yNoise(5, 8, 2.0f, random);
	Noise::NoiseGenerator1D zNoise(5, 8, 2.0f, random);
	Noise::NoiseGenerator1D wNoise(5, 8, 2.0f, random);

	const CaveNode rootNode(geom::Vector(random.getFloat(0.0f, 16.0f) + chunkPos.x * 16.0f, random.getFloat(0.0f, 16.0f) + chunkPos.y * 16.0f, random.getFloat(0.0f, 16.0f) + chunkPos.z * 16.0f, 0.0f));
	nodes[0] = rootNode;

	geom::Vector min(rootNode.pos);
	geom::Vector max(rootNode.pos);

	CaveNode* prevNode = &nodes[0];
	float n = random.getFloat(256.0f, 512.0f);
	for(int i = 1; i <= length; i++)
	{
		n += 24.0f;
		geom::Vector translation = geom::Vector(xNoise.getNoise(n) - 0.5f, yNoise.getNoise(n) - 0.5f, zNoise.getNoise(n) - 0.5f) - (prevNode->pos - rootNode.pos) / 64.0f;
		//geom::Vector translation = geom::Vector(xNoise.getNoise(n) - 0.5f, yNoise.getNoise(n) - 0.5f, zNoise.getNoise(n) - 0.5f);
		CaveNode newNode(prevNode->pos + translation.normalize() * 4.0);
		newNode.pos.w = (1.0f + wNoise.getNoise(n / 2.0f)) * 10.0f * i * (length - i) / (length * length) + 0.5f;
		//newNode.pos.w = 6.0;
		nodes[i] = newNode;
		prevNode = &nodes[i];

		if(newNode.pos.x < min.x) min.x = newNode.pos.x;
		else if(newNode.pos.x > max.x) max.x = newNode.pos.x;

		if(newNode.pos.y < min.y) min.y = newNode.pos.y;
		else if(newNode.pos.y > max.y) max.y = newNode.pos.y;
		
		if(newNode.pos.z < min.z) min.z = newNode.pos.z;
		else if(newNode.pos.z > max.z) max.z = newNode.pos.z;
	}
	
	geom::Vector translate = (min - max) / 2.0f;
	
	for(int i = 0; i <= length; i++)
	{
		nodes[i].pos += translate;
	}

	boundingBox = geom::AxisAlignedCube(rootNode.pos + translate, max - rootNode.pos - translate);
}

Cave::~Cave()
{
	if(nodes != nullptr)
	{
		delete[] nodes;
	}
}

std::unordered_map<ChunkPosition, std::shared_ptr<Cave>> CaveCache::generatedCaves;
std::unordered_map<ChunkPosition, std::vector<std::shared_ptr<Cave>>> CaveCache::caves;
std::shared_ptr<Cave> CaveCache::nullCave(new Cave());

std::vector<std::shared_ptr<Cave>> CaveCache::getCavesInRegion(ChunkPosition chunkPos)
{
	const int bb = 4;
	for(int i = -bb; i <= bb; i++)
	{
		for(int j = -bb; j <= bb; j++)
		{
			for(int k = -bb; k <= bb; k++)
			{
				makeCave(ChunkPosition(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k));
			}
		}
	}

	return caves[chunkPos];
}

void CaveCache::makeCave(ChunkPosition chunkPos)
{
	if(chunkPos.y >= 8)
	{
		return;
	}

	auto iter = generatedCaves.find(chunkPos);
	if(iter == generatedCaves.end())
	{
		unsigned int h = 14365216;
		h *= GlobalThread::world.getSeed();
		h ^= (chunkPos.x) * 374769045;
		h ^= (chunkPos.y) * 387619473;
		h ^= (chunkPos.z) * 72658975;
		h += (chunkPos.x) >> 24 ^ (chunkPos.x) << 8;
		h += (chunkPos.y) >> 16 ^ (chunkPos.y) << 16;
		h += (chunkPos.z) >> 8 ^ (chunkPos.z) << 24;
		h ^= 305771093;
		h -= GlobalThread::world.getSeed();
		h ^= h << 1;
		const int caveChecks = 2;
		const int caveChance = 3;
		for(int check = 0; check < caveChecks; check++)
		{
			const unsigned int c = (h >> (check * 4)) & 0xf;
			if(c < caveChance)
			{
				Random random(h);
				std::shared_ptr<Cave> cave(new Cave(ChunkPosition(chunkPos.x, chunkPos.y, chunkPos.z), random));
				generatedCaves[chunkPos] = cave;

				geom::Vector bottom = cave->boundingBox.pos - geom::Vector(8.0f, 8.0f, 8.0f);
				geom::Vector top = cave->boundingBox.pos + cave->boundingBox.size + geom::Vector(8.0f, 8.0f, 8.0f);
				for(int i = floorf(bottom.x / 16.0f); i <= floorf(top.x / 16.0f); i++)
				{
					for(int j = floorf(bottom.y / 16.0f); j <= floorf(top.y / 16.0f); j++)
					{
						for(int k = floorf(bottom.z / 16.0f); k <= floorf(top.z / 16.0f); k++)
						{
							caves[ChunkPosition(i, j, k)].push_back(cave);
						}
					}
				}

				return;
			}
		}
	}
}