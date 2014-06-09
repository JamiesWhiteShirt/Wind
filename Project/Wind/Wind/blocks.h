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
	gfxu::TiledTexture::Icon* icon;
	std::wstring texturePath;

	Block(unsigned short id, std::wstring texture, BLOCK_RENDER_FUNC firstPassRenderer, BLOCK_RENDER_FUNC secondPassRenderer);
public:
	const unsigned short id;
	BLOCK_RENDER_FUNC firstPassRenderer;
	BLOCK_RENDER_FUNC secondPassRenderer;

	Block(unsigned short id, std::wstring texture);
	~Block();

	virtual void registerIcons(gfxu::TiledTexture* texture);

	virtual gfxu::TiledTexture::Icon* getIcon(World* world, int x, int y, int z, const BlockFace* face);

	virtual bool solidCube(World* world, int x, int y, int z);
	virtual bool solidFace(World* world, int x, int y, int z, const BlockFace* face);
	virtual bool solidEdge(World* world, int x, int y, int z, const BlockEdge* edge);
	virtual bool solidCorner(World* world, int x, int y, int z, const BlockCorner* edge);
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
	BlockFluid(unsigned short id, std::wstring texture);
	
	virtual bool solidCube(World* world, int x, int y, int z);
};

class BlockGrass : public Block
{
private:
	gfxu::TiledTexture::Icon* iconTop;
	gfxu::TiledTexture::Icon* iconBottom;

public:
	BlockGrass(unsigned short id, std::wstring texture);

	virtual void registerIcons(gfxu::TiledTexture* texture);

	virtual gfxu::TiledTexture::Icon* getIcon(World* world, int x, int y, int z, const BlockFace* face);
};

namespace Blocks
{
	extern Block* blockArray[4096];

	extern BlockAir* air;
	extern Block* stone;
	extern BlockFluid* water;
	extern Block* dirt;
	extern BlockGrass* grass;
	extern Block* fineGravel;
	extern Block* gravel;
	extern Block* sand;

	void staticInit();
	void destroy();
};