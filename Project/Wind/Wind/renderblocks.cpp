#include "blocks.h"
#include "world.h"

using namespace BlockRenderers;
using namespace gfxu;

bool BlockRenderers::renderOrdinaryBlock(World* world, int x, int y, int z, Block* block, VertexStream* vStream)
{
	if(!world->getBlock(x, y, z - 1)->solidFace(world, x, y, z - 1, Relatives::N))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

		if(world->getBlock(x - 1, y, z - 1)->solidEdge(world, x - 1, y, z - 1, Relatives::EN))
		{
			i1++;
			i4++;
		}

		if(world->getBlock(x, y - 1, z - 1)->solidEdge(world, x, y - 1, z - 1, Relatives::UN))
		{
			i1++;
			i2++;
		}

		if(world->getBlock(x + 1, y, z - 1)->solidEdge(world, x + 1, y, z - 1, Relatives::WN))
		{
			i2++;
			i3++;
		}

		if(world->getBlock(x, y + 1, z - 1)->solidEdge(world, x, y + 1, z - 1, Relatives::DN))
		{
			i3++;
			i4++;
		}

		if(i1 < 2 && world->getBlock(x - 1, y - 1, z - 1)->solidCorner(world, x - 1, y - 1, z - 1, Relatives::EUN))
		{
			i1++;
		}

		if(i2 < 2 && world->getBlock(x + 1, y - 1, z - 1)->solidCorner(world, x + 1, y - 1, z - 1, Relatives::WUN))
		{
			i2++;
		}
								
		if(i3 < 2 && world->getBlock(x + 1, y + 1, z - 1)->solidCorner(world, x + 1, y + 1, z - 1, Relatives::WDN))
		{
			i3++;
		}
								
		if(i4 < 2 && world->getBlock(x - 1, y + 1, z - 1)->solidCorner(world, x - 1, y + 1, z - 1, Relatives::EDN))
		{
			i4++;
		}

		float f1 = 1.0F - i1 * 0.1f;
		float f2 = 1.0F - i2 * 0.1f;
		float f3 = 1.0F - i3 * 0.1f;
		float f4 = 1.0F - i4 * 0.1f;

		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f));
	}
					
	if(!world->getBlock(x, y, z + 1)->solidFace(world, x, y, z + 1, Relatives::S))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

		if(world->getBlock(x - 1, y, z + 1)->solidEdge(world, x - 1, y, z + 1, Relatives::ES))
		{
			i1++;
			i4++;
		}

		if(world->getBlock(x, y - 1, z + 1)->solidEdge(world, x, y - 1, z + 1, Relatives::US))
		{
			i1++;
			i2++;
		}

		if(world->getBlock(x + 1, y, z + 1)->solidEdge(world, x + 1, y, z + 1, Relatives::WS))
		{
			i2++;
			i3++;
		}

		if(world->getBlock(x, y + 1, z + 1)->solidEdge(world, x, y + 1, z + 1, Relatives::DS))
		{
			i3++;
			i4++;
		}

		if(i1 < 2 && world->getBlock(x - 1, y - 1, z + 1)->solidCorner(world, x - 1, y - 1, z + 1, Relatives::EUS))
		{
			i1++;
		}

		if(i2 < 2 && world->getBlock(x + 1, y - 1, z + 1)->solidCorner(world, x + 1, y - 1, z + 1, Relatives::WUS))
		{
			i2++;
		}
								
		if(i3 < 2 && world->getBlock(x + 1, y + 1, z + 1)->solidCorner(world, x + 1, y + 1, z + 1, Relatives::WDS))
		{
			i3++;
		}
								
		if(i4 < 2 && world->getBlock(x - 1, y + 1, z + 1)->solidCorner(world, x - 1, y + 1, z + 1, Relatives::EDS))
		{
			i4++;
		}

		float f1 = 1.0F - i1 * 0.1f;
		float f2 = 1.0F - i2 * 0.1f;
		float f3 = 1.0F - i3 * 0.1f;
		float f4 = 1.0F - i4 * 0.1f;

		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 1.0f, 1.0f, 1.0f, f4, f4, f4, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, f1, f1, f1, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 1.0f, 0.0f, 0.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, f1, f1, f1, 1.0f));
	}

	if(!world->getBlock(x, y - 1, z)->solidFace(world, x, y - 1, z, Relatives::U))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

		if(world->getBlock(x - 1, y - 1, z)->solidEdge(world, x - 1, y - 1, z, Relatives::EU))
		{
			i1++;
			i4++;
		}

		if(world->getBlock(x, y - 1, z - 1)->solidEdge(world, x, y - 1, z - 1, Relatives::UN))
		{
			i1++;
			i2++;
		}

		if(world->getBlock(x + 1, y - 1, z)->solidEdge(world, x + 1, y - 1, z, Relatives::WU))
		{
			i2++;
			i3++;
		}

		if(world->getBlock(x, y - 1, z + 1)->solidEdge(world, x, y - 1, z + 1, Relatives::US))
		{
			i3++;
			i4++;
		}

		if(i1 < 2 && world->getBlock(x - 1, y - 1, z - 1)->solidCorner(world, x - 1, y - 1, z - 1, Relatives::EUN))
		{
			i1++;
		}

		if(i2 < 2 && world->getBlock(x + 1, y - 1, z - 1)->solidCorner(world, x + 1, y - 1, z - 1, Relatives::WUN))
		{
			i2++;
		}
								
		if(i3 < 2 && world->getBlock(x + 1, y - 1, z + 1)->solidCorner(world, x + 1, y - 1, z + 1, Relatives::WUS))
		{
			i3++;
		}
								
		if(i4 < 2 && world->getBlock(x - 1, y - 1, z + 1)->solidCorner(world, x - 1, y - 1, z + 1, Relatives::EUS))
		{
			i4++;
		}

		float f1 = 1.0F - i1 * 0.1f;
		float f2 = 1.0F - i2 * 0.1f;
		float f3 = 1.0F - i3 * 0.1f;
		float f4 = 1.0F - i4 * 0.1f;

		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f));
	}

	if(!world->getBlock(x, y + 1, z)->solidFace(world, x, y, z, Relatives::D))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

		if(world->getBlock(x - 1, y + 1, z)->solidEdge(world, x - 1, y + 1, z, Relatives::ED))
		{
			i1++;
			i4++;
		}

		if(world->getBlock(x, y + 1, z - 1)->solidEdge(world, x, y + 1, z - 1, Relatives::DN))
		{
			i1++;
			i2++;
		}

		if(world->getBlock(x + 1, y + 1, z)->solidEdge(world, x + 1, y + 1, z, Relatives::WD))
		{
			i2++;
			i3++;
		}

		if(world->getBlock(x, y + 1, z + 1)->solidEdge(world, x, y + 1, z + 1, Relatives::DS))
		{
			i3++;
			i4++;
		}

		if(i1 < 2 && world->getBlock(x - 1, y + 1, z - 1)->solidCorner(world, x - 1, y + 1, z - 1, Relatives::EDN))
		{
			i1++;
		}

		if(i2 < 2 && world->getBlock(x + 1, y + 1, z - 1)->solidCorner(world, x + 1, y + 1, z - 1, Relatives::WDN))
		{
			i2++;
		}
								
		if(i3 < 2 && world->getBlock(x + 1, y + 1, z + 1)->solidCorner(world, x + 1, y + 1, z + 1, Relatives::WDS))
		{
			i3++;
		}
								
		if(i4 < 2 && world->getBlock(x - 1, y + 1, z + 1)->solidCorner(world, x - 1, y + 1, z + 1, Relatives::EDS))
		{
			i4++;
		}

		float f1 = 1.0F - i1 * 0.1f;
		float f2 = 1.0F - i2 * 0.1f;
		float f3 = 1.0F - i3 * 0.1f;
		float f4 = 1.0F - i4 * 0.1f;

		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f));
	}

	if(!world->getBlock(x - 1, y, z)->solidFace(world, x - 1, y, z, Relatives::E))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

		if(world->getBlock(x - 1, y - 1, z)->solidEdge(world, x - 1, y - 1, z, Relatives::EU))
		{
			i1++;
			i4++;
		}

		if(world->getBlock(x - 1, y, z - 1)->solidEdge(world, x - 1, y, z - 1, Relatives::EN))
		{
			i1++;
			i2++;
		}

		if(world->getBlock(x - 1, y + 1, z)->solidEdge(world, x - 1, y + 1, z, Relatives::ED))
		{
			i2++;
			i3++;
		}

		if(world->getBlock(x - 1, y, z + 1)->solidEdge(world, x - 1, y, z + 1, Relatives::ES))
		{
			i3++;
			i4++;
		}

		if(i1 < 2 && world->getBlock(x - 1, y - 1, z - 1)->solidCorner(world, x - 1, y - 1, z - 1, Relatives::EUN))
		{
			i1++;
		}

		if(i2 < 2 && world->getBlock(x - 1, y + 1, z - 1)->solidCorner(world, x - 1, y + 1, z - 1, Relatives::EDN))
		{
			i2++;
		}
								
		if(i3 < 2 && world->getBlock(x - 1, y + 1, z + 1)->solidCorner(world, x - 1, y + 1, z + 1, Relatives::EDS))
		{
			i3++;
		}
								
		if(i4 < 2 && world->getBlock(x - 1, y - 1, z + 1)->solidCorner(world, x - 1, y - 1, z + 1, Relatives::EUS))
		{
			i4++;
		}

		float f1 = 1.0F - i1 * 0.1f;
		float f2 = 1.0F - i2 * 0.1f;
		float f3 = 1.0F - i3 * 0.1f;
		float f4 = 1.0F - i4 * 0.1f;

		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, f1, f1, f1, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f));
	}

	if(!world->getBlock(x + 1, y, z)->solidFace(world, x, y, z, Relatives::W))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

		if(world->getBlock(x + 1, y - 1, z)->solidEdge(world, x + 1, y - 1, z, Relatives::WU))
		{
			i1++;
			i4++;
		}

		if(world->getBlock(x + 1, y, z - 1)->solidEdge(world, x + 1, y, z - 1, Relatives::WN))
		{
			i1++;
			i2++;
		}

		if(world->getBlock(x + 1, y + 1, z)->solidEdge(world, x + 1, y + 1, z, Relatives::WD))
		{
			i2++;
			i3++;
		}

		if(world->getBlock(x + 1, y, z + 1)->solidEdge(world, x + 1, y, z + 1, Relatives::WS))
		{
			i3++;
			i4++;
		}

		if(i1 < 2 && world->getBlock(x + 1, y - 1, z - 1)->solidCorner(world, x + 1, y - 1, z - 1, Relatives::WUN))
		{
			i1++;
		}

		if(i2 < 2 && world->getBlock(x + 1, y + 1, z - 1)->solidCorner(world, x + 1, y + 1, z - 1, Relatives::WDN))
		{
			i2++;
		}
								
		if(i3 < 2 && world->getBlock(x + 1, y + 1, z + 1)->solidCorner(world, x + 1, y + 1, z + 1, Relatives::WDS))
		{
			i3++;
		}
								
		if(i4 < 2 && world->getBlock(x + 1, y - 1, z + 1)->solidCorner(world, x + 1, y - 1, z + 1, Relatives::WUS))
		{
			i4++;
		}

		float f1 = 1.0F - i1 * 0.1f;
		float f2 = 1.0F - i2 * 0.1f;
		float f3 = 1.0F - i3 * 0.1f;
		float f4 = 1.0F - i4 * 0.1f;

		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, f4, f4, f4, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 1.0f, 0.0f, 0.0f, 1.0f, f2, f2, f2, 1.0f));
		vStream->put(gfxu::VertexUVRGBA(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f));
	}

	return true;
}

bool BlockRenderers::renderInvisibleBlock(World* world, int x, int y, int z, Block* block, VertexStream* vStream)
{
	return true;
}

bool BlockRenderers::renderFluidBlock(World* world, int x, int y, int z, Block* block, VertexStream* vStream)
{
	vStream->setColor(16, 64, 224, 127);

	Block* b = world->getBlock(x, y, z - 1);
	if(b != block && !b->solidFace(world, x, y + 1, z, Relatives::N))
	{
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 0.0f, 0.0f, 1.0f));

		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 0.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 0.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f));
	}

	b = world->getBlock(x, y, z + 1);
	if(b != block && !b->solidFace(world, x, y + 1, z, Relatives::S))
	{
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 1.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 1.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 1.0f, 0.0f, 1.0f));

		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 1.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 1.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 1.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f));
	}

	b = world->getBlock(x, y - 1, z);
	if(b != block && !b->solidFace(world, x, y + 1, z, Relatives::U))
	{
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 1.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 1.0f, 0.0f, 1.0f));

		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 1.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	b = world->getBlock(x, y + 1, z);
	if(b != block && !b->solidFace(world, x, y + 1, z, Relatives::D))
	{
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 0.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 1.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 1.0f, 0.0f, 1.0f));

		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 1.0f, 0.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 0.0f));
		vStream->put(gfxu::VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f));
		vStream->put(gfxu::VertexUV(0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	}

	return true;
}