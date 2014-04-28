#pragma once

#include "graphics.h"

class World;
class Block;

typedef bool (*BLOCK_RENDER_FUNC)(World*, int, int, int, Block*, gfxu::VertexStream*);

namespace BlockRenderers
{
	bool renderOrdinaryBlock(World* world, int x, int y, int z, Block* block, gfxu::VertexStream* vStream);
	bool renderInvisibleBlock(World* world, int x, int y, int z, Block* block, gfxu::VertexStream* vStream);
	bool renderFluidBlock(World* world, int x, int y, int z, Block* block, gfxu::VertexStream* vStream);
};

class BlockRelative
{
public:
	int x;
	int y;
	int z;
protected:
	BlockRelative(int x, int y, int z);
};

class BlockFace : public BlockRelative
{
public:
	BlockFace(int x, int y, int z);
};

class BlockEdge : public BlockRelative
{
public:
	BlockEdge(int x, int y, int z);
};

class BlockCorner : public BlockRelative
{
public:
	BlockCorner(int x, int y, int z);
};

namespace Relatives
{
	extern BlockFace* W;
	extern BlockFace* E;
	extern BlockFace* D;
	extern BlockFace* U;
	extern BlockFace* S;
	extern BlockFace* N;

	extern BlockEdge* WD;
	extern BlockEdge* WU;
	extern BlockEdge* WS;
	extern BlockEdge* WN;
	extern BlockEdge* ED;
	extern BlockEdge* EU;
	extern BlockEdge* ES;
	extern BlockEdge* EN;
	extern BlockEdge* DS;
	extern BlockEdge* DN;
	extern BlockEdge* US;
	extern BlockEdge* UN;

	extern BlockCorner* WDS;
	extern BlockCorner* EDS;
	extern BlockCorner* WUS;
	extern BlockCorner* EUS;
	extern BlockCorner* WDN;
	extern BlockCorner* EDN;
	extern BlockCorner* WUN;
	extern BlockCorner* EUN;
};

class Block
{
private:

protected:
	Block(unsigned short id, BLOCK_RENDER_FUNC firstPassRenderer, BLOCK_RENDER_FUNC secondPassRenderer);
public:
	const unsigned short id;
	BLOCK_RENDER_FUNC firstPassRenderer;
	BLOCK_RENDER_FUNC secondPassRenderer;

	Block(unsigned short id);
	~Block();

	virtual bool solidCube(World* world, int x, int y, int z);
	virtual bool solidFace(World* world, int x, int y, int z, BlockFace* face);
	virtual bool solidEdge(World* world, int x, int y, int z, BlockEdge* edge);
	virtual bool solidCorner(World* world, int x, int y, int z, BlockCorner* edge);
};

class BlockAir : public Block
{
public:
	BlockAir(unsigned short id);
	
	virtual bool solidCube(World* world, int x, int y, int z);
};

class BlockFluid : public Block
{
public:
	BlockFluid(unsigned short id);
	
	virtual bool solidCube(World* world, int x, int y, int z);
};

namespace Blocks
{
	extern Block* blockArray[4096];

	extern BlockAir* air;
	extern Block* stone;
	extern BlockFluid* water;

	void initialize();
	void destroy();
};