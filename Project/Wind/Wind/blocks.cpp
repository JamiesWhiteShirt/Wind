#include "blocks.h"

using namespace Blocks;

BlockRelative::BlockRelative(int x, int y, int z)
	: x(x), y(y), z(z)
{

}

BlockFace::BlockFace(int x, int y, int z)
	: BlockRelative(x, y, z)
{

}

BlockEdge::BlockEdge(int x, int y, int z)
	: BlockRelative(x, y, z)
{

}

BlockCorner::BlockCorner(int x, int y, int z)
	: BlockRelative(x, y, z)
{

}

BlockFace* Relatives::E;
BlockFace* Relatives::W;
BlockFace* Relatives::D;
BlockFace* Relatives::U;
BlockFace* Relatives::S;
BlockFace* Relatives::N;

BlockEdge* Relatives::WD;
BlockEdge* Relatives::WU;
BlockEdge* Relatives::WS;
BlockEdge* Relatives::WN;
BlockEdge* Relatives::ED;
BlockEdge* Relatives::EU;
BlockEdge* Relatives::ES;
BlockEdge* Relatives::EN;
BlockEdge* Relatives::DS;
BlockEdge* Relatives::DN;
BlockEdge* Relatives::US;
BlockEdge* Relatives::UN;

BlockCorner* Relatives::WDS;
BlockCorner* Relatives::EDS;
BlockCorner* Relatives::WUS;
BlockCorner* Relatives::EUS;
BlockCorner* Relatives::WDN;
BlockCorner* Relatives::EDN;
BlockCorner* Relatives::WUN;
BlockCorner* Relatives::EUN;

Block::Block(unsigned short id)
	: id(id), firstPassRenderer(&BlockRenderers::renderOrdinaryBlock), secondPassRenderer(&BlockRenderers::renderInvisibleBlock)
{
	Blocks::blockArray[id] = this;
}

Block::Block(unsigned short id, BLOCK_RENDER_FUNC firstPassRenderer, BLOCK_RENDER_FUNC secondPassRenderer)
	: id(id), firstPassRenderer(firstPassRenderer), secondPassRenderer(secondPassRenderer)
{
	Blocks::blockArray[id] = this;
}

Block::~Block()
{

}

bool Block::solidCube(World* world, int x, int y, int z)
{
	return true;
}

bool Block::solidFace(World* world, int x, int y, int z, BlockFace* face)
{
	return solidCube(world, x, y, z);
}

bool Block::solidEdge(World* world, int x, int y, int z, BlockEdge* face)
{
	return solidCube(world, x, y, z);
}

bool Block::solidCorner(World* world, int x, int y, int z, BlockCorner* face)
{
	return solidCube(world, x, y, z);
}

BlockAir::BlockAir(unsigned short id)
	: Block(id, &BlockRenderers::renderInvisibleBlock, &BlockRenderers::renderInvisibleBlock)
{

}

bool BlockAir::solidCube(World* world, int x, int y, int z)
{
	return false;
}

BlockFluid::BlockFluid(unsigned short id)
	: Block(id, &BlockRenderers::renderInvisibleBlock, &BlockRenderers::renderFluidBlock)
{

}

bool BlockFluid::solidCube(World* world, int x, int y, int z)
{
	return false;
}

Block* Blocks::blockArray[4096];

BlockAir* Blocks::air;
Block* Blocks::stone;
BlockFluid* Blocks::water;

void Blocks::initialize()
{
	Relatives::E = new BlockFace(-1, 0, 0);
	Relatives::W = new BlockFace(1, 0, 0);
	Relatives::D = new BlockFace(0, -1, 0);
	Relatives::U = new BlockFace(0, 1, 0);
	Relatives::S = new BlockFace(0, 0, -1);
	Relatives::N = new BlockFace(0, 0, 1);

	Relatives::WD = new BlockEdge(-1, -1, 0);
	Relatives::WU = new BlockEdge(-1, 1, 0);
	Relatives::WS = new BlockEdge(-1, 0, -1);
	Relatives::WN = new BlockEdge(-1, 0, 1);
	Relatives::ED = new BlockEdge(1, -1, 0);
	Relatives::EU = new BlockEdge(1, 1, 0);
	Relatives::ES = new BlockEdge(1, 0, -1);
	Relatives::EN = new BlockEdge(1, 0, 1);
	Relatives::DS = new BlockEdge(0, -1, -1);
	Relatives::DN = new BlockEdge(0, -1, 1);
	Relatives::US = new BlockEdge(0, 1, -1);
	Relatives::UN = new BlockEdge(0, 1, 1);

	Relatives::WDS = new BlockCorner(-1, -1, -1);
	Relatives::EDS = new BlockCorner(1, -1, -1);
	Relatives::WUS = new BlockCorner(-1, 1, -1);
	Relatives::EUS = new BlockCorner(1, 1, -1);
	Relatives::WDN = new BlockCorner(-1, -1, 1);
	Relatives::EDN = new BlockCorner(1, -1, 1);
	Relatives::WUN = new BlockCorner(-1, 1, 1);
	Relatives::EUN = new BlockCorner(1, 1, 1);

	for(int i = 0; i < 4096; i++)
	{
		blockArray[i] = nullptr;
	}

	air = new BlockAir(0);
	stone = new Block(1);
	water = new BlockFluid(2);
}

void Blocks::destroy()
{
	delete Relatives::E;
	delete Relatives::W;
	delete Relatives::D;
	delete Relatives::U;
	delete Relatives::S;
	delete Relatives::N;

	delete Relatives::WD;
	delete Relatives::WU;
	delete Relatives::WS;
	delete Relatives::WN;
	delete Relatives::ED;
	delete Relatives::EU;
	delete Relatives::ES;
	delete Relatives::EN;
	delete Relatives::DS;
	delete Relatives::DN;
	delete Relatives::US;
	delete Relatives::UN;

	delete Relatives::WDS;
	delete Relatives::EDS;
	delete Relatives::WUS;
	delete Relatives::EUS;
	delete Relatives::WDN;
	delete Relatives::EDN;
	delete Relatives::WUN;
	delete Relatives::EUN;

	delete air;
	delete stone;
	delete water;
}