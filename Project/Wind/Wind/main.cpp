#define _USE_MATH_DEFINES

#include "window.h"
#include "ioutil.h"
#include <iostream>
#include <sstream>
#include "graphics.h"
#include <thread>
#include "input.h"
#include <math.h>
#include <queue>
#include "world.h"
#include "task.h"
#include "list.h"

using namespace gfxu;
using namespace GameStates;

std::thread graphicsThread;
std::thread chunkThread;

int ticks = 0;
bool stop = false;
bool enableRender = false;

World world;

std::queue<ChunkBase*> chunkLoadQueue;

geom::Quad q(geom::Vector(0.0f, 0.0f, 0.0f), geom::Vector(0.0f, 25.0f, 0.0f), geom::Vector(25.0f, 50.0f, 25.0f), geom::Vector(25.0f, 0.0f, 25.0f));
geom::Line l(geom::Vector(16.0f, 30.0f, -100.0f), geom::Vector(16.0f, 30.0f, 10.0f));
geom::TracedIntersection intersection(q, l);

geom::AxisAlignedCube cube(geom::Vector(), geom::Vector(10.0f, 10.0f, 10.0f));
geom::Ray r(geom::Vector(-1.0f, 1.0f, 1.0f), geom::Vector(2.0f, 0.0f, 0.0f));
geom::Intersection intersection2(cube, r);

VertexStream grid;
VertexStream square;
VertexStream quad;
VertexStream line;

MemUtil::MiniHeap taskList(4096);

void graphicsLoop()
{
	GLWindow::instance->initGL();
	
	if(!GLWindow::instance->isOK())
	{
		return;
	}

	std::wstring filePath(IOUtil::EXE_DIR);
	filePath += L"\\shaders\\normal.vert";
	VertexShader normalVShader(filePath);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\normal.frag";
	FragmentShader normalFShader(filePath);

	ShaderProgram normalShaderProgram(&normalVShader, nullptr, &normalFShader);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\noTex.vert";
	VertexShader noTexVShader(filePath);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\noTex.frag";
	FragmentShader noTexFShader(filePath);

	ShaderProgram noTexShaderProgram(&noTexVShader, nullptr, &noTexFShader);

	Uniforms::reset();

	Noise::NoiseGenerator2D noise1(8, 8, 2.0f, 0);
	Noise::NoiseGenerator2D noise2(8, 8, 2.0f, 1);
	Noise::NoiseGenerator2D noise3(8, 8, 2.0f, 2);
	Noise::NoiseGenerator2D noise4(8, 8, 2.0f, 3);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\textures\\test.png";
	Texture2D testure(filePath, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
	//Texture2D testure(256, 256, &noise1, &noise2, &noise3, &noise4);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!stop)
	{
		/*for(int i = 0; i < taskList.size; i++)
		{
			if(!taskList[i].invoke())
			{
				stop = true;
			}
		}*/

		taskList.clear();

		if(!enableRender)
		{
			continue;
		}

		swapPendingRendering();

		GameState* state = GameStates::renderingState;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(GLWindow::instance->rescaled)
		{
			glViewport(0, 0, GLWindow::instance->width, GLWindow::instance->height);
			GLWindow::instance->rescaled = false;
		}
		
		normalShaderProgram.bind();
		Uniforms::color.set(1.0f, 1.0f, 1.0f, 1.0f);
		Uniforms::reset();
		Uniforms::PMS.mult(geom::Matrix::perspective(90.0f, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.1f, 100.0f));
		Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.x, 1.0f, 0.0f, 0.0f));
		Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.y, 0.0f, 1.0f, 0.0f));
		Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.z, 0.0f, 0.0f, 1.0f));
		Uniforms::PMS.mult(geom::Matrix::translate(-state->cam.pos.x, -state->cam.pos.y, -state->cam.pos.z));
		
		testure.bind();
		for(map<ChunkPosition, ChunkBase*>::const_iterator iter = world.chunkMap.begin(); iter != world.chunkMap.end(); ++iter)
		{
			ChunkBase* chunk = iter->second;

			if(chunk != nullptr && chunk->isLoaded())
			{
				float xPos = chunk->pos.x * 16;
				float yPos = chunk->pos.y * 16;
				float zPos = chunk->pos.z * 16;
				geom::AxisAlignedCube cube(geom::Vector(-1.0f, -1.0f, -1.0f), geom::Vector(2.0f, 2.0f, 2.0f));
				if(cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos, yPos, zPos)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos + 16, yPos, zPos)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos, yPos + 16, zPos)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos + 16, yPos + 16, zPos)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos, yPos, zPos + 16)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos + 16, yPos, zPos + 16)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos, yPos + 16, zPos + 16)).wDivide())
					|| cube.inside((Uniforms::PMS.getTopmost() * geom::Vector(xPos + 16, yPos + 16, zPos + 16)).wDivide()))
				{
					Uniforms::MMS.push(geom::Matrix::translate(xPos, yPos, zPos));
					chunk->mut.lock();
					chunk->renderStream->draw();
					chunk->mut.unlock();
					Uniforms::MMS.pop();
				}
			}
		}

		noTexShaderProgram.bind();
		Uniforms::MMS.push(geom::Matrix::scale(16.0f, 16.0f, 16.0f));
		
		Uniforms::color.set(0.5f, 0.5f, 0.5f, 1.0f);
		grid.draw(GL_LINES);

		glDepthFunc(GL_GREATER);
		glEnable(GL_BLEND);
		Uniforms::color.set(0.5f, 0.5f, 0.5f, 0.125f);
		grid.draw(GL_LINES);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);

		Uniforms::MMS.pop();


		
		/*glDisable(GL_CULL_FACE);
		Uniforms::color.set(1.0f, 0.0f, 0.0f, 1.0f);
		quad.draw();
		Uniforms::color.set(0.0f, 1.0f, 0.0f, 1.0f);
		line.draw(GL_LINES);
		if(intersection.type == INTERSECTION_VECTOR)
		{
			geom::Vector vec = *(geom::Vector*)intersection.result;
			Uniforms::color.set(0.0f, 0.0f, 1.0f, 1.0f);
			Uniforms::MMS.push(Matrix::translate(vec.x, vec.y, vec.z));
			square.draw();
			Uniforms::MMS.pop();
		}
		glEnable(GL_CULL_FACE);*/

		glFlush();
		if(getError()) stop = true;
		GLWindow::instance->swapBuffers();
	}

	GLWindow::instance->destroyGL();
}

void chunkLoaderLoop()
{
	Noise::NoiseGenerator3D noise(8, 8, 2.0f, 3);
	Noise::NoiseGenerator2D noise2dx(8, 8, 2.0f, 0);
	Noise::NoiseGenerator2D noise2dz(8, 8, 2.0f, 1);
	Noise::NoiseGenerator2D noise2dlevel(8, 8, 2.0f, 2);

	while(!stop)
	{
		if(chunkLoadQueue.empty())
		{
			continue;
		}

		ChunkBase* chunk = chunkLoadQueue.front();
		chunkLoadQueue.pop();
		
		float treshold = 0.5f;

		for(unsigned int i = 0; i < 16; i++)
		{
			unsigned int x = i + chunk->pos.x * 16;
			for(unsigned int k = 0; k < 16; k++)
			{
				unsigned int z = k + chunk->pos.z * 16;
				//float xd = noise2dx.getNoise(x, z) * 2.0 - 1.0;
				//float zd = noise2dz.getNoise(x, z) * 2.0 - 1.0;
				//float len = sqrtf(xd * xd + zd * zd);
				for(unsigned int j = 0; j < 16; j++)
				{
					unsigned int y = j + chunk->pos.y * 16;
					//if(noise.getNoise(x, y, z) >= treshold)
					if(y < 64)
					{
						chunk->setBlockRaw(i, j, k, 1);
					}
					else
					{
						//unsigned int h = 64.0f + noise2dlevel.getNoise(x + xd * 8.0f / len, z + zd * 8.0f / len) * 64.0;
						//if(y < h)
						if((y - 64) / 64.0f < noise.getNoise(x, y, z))
						{
							chunk->setBlockRaw(i, j, k, 1);
						}
						else
						{
							chunk->setBlockRaw(i, j, k, 0);
						}
					}
				}
			}
		}

		chunk->setLoaded();
		chunk->setRenderUpdateNeeded(true);
	}
}

bool drawLoop()
{
	Camera* cam = &GameStates::processedState->cam;

	if(Mouse::getMB(0))
	{
		for(int i = 0; i < Mouse::actions.getSize(); i++)
		{
			MouseAction ma = Mouse::actions[i];
			cam->rot.x -= ma.ry * 0.5f;
			cam->rot.y -= ma.rx * 0.5f;
		}
	}

	float b = Keyboard::getKey(16) ? 0.4f : 0.1f;

	if(Keyboard::getKey(87)) //W
	{
		cam->pos.x -= b * cos(cam->rot.x * M_PI / 180.0f) * sin(cam->rot.y * M_PI / 180.0f);
		cam->pos.y += b * sin(cam->rot.x * M_PI / 180.0f);
		cam->pos.z += b * cos(cam->rot.x * M_PI / 180.0f) * cos(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(83)) //S
	{
		cam->pos.x += b * cos(cam->rot.x * M_PI / 180.0f) * sin(cam->rot.y * M_PI / 180.0f);
		cam->pos.y -= b * sin(cam->rot.x * M_PI / 180.0f);
		cam->pos.z -= b * cos(cam->rot.x * M_PI / 180.0f) * cos(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(65)) //A
	{
		cam->pos.x -= b * cos(cam->rot.y * M_PI / 180.0f);
		cam->pos.z -= b * sin(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(68)) //D
	{
		cam->pos.x += b * cos(cam->rot.y * M_PI / 180.0f);
		cam->pos.z += b * sin(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(32)) //SPACE
	{ //SPACE!
		world.setBlock((int)cam->pos.x, (int)cam->pos.y, (int)cam->pos.z, 1); //SPAAAAAAAAAAAAAACE!
	}

	/*for(int i = 0; i < Keyboard::actions.getSize(); i++)
	{
		KeyboardAction ka = Keyboard::actions[i];

		if((ka.button == 32) & ka.released) //SPACE
		{
			world.setBlock((int)cam->pos.x, (int)cam->pos.y, (int)cam->pos.z, 1);
		}
	}*/

	Mouse::actions.clear();
	Keyboard::actions.clear();

	world.chunkMapLock.lock();
	for(map<ChunkPosition, ChunkBase*>::const_iterator iter = world.chunkMap.begin(); iter != world.chunkMap.end(); ++iter)
	{
		ChunkBase* chunk = iter->second;

		if(chunk != nullptr && chunk->isLoaded() && chunk->isRenderUpdateNeeded())
		{
			bool flag = true;
			for(int i = -1; (i < 2) & flag; i++)
			{
				for(int j = -1; (j < 2) & flag; j++)
				{
					for(int k = -1; (k < 2) & flag; k++)
					{
						if(!world.isChunkLoaded(chunk->pos.x + i, chunk->pos.y + j, chunk->pos.z + k))
						{
							flag = false;
						}
					}
				}
			}

			if(!flag)
			{
				continue;
			}

			VertexStream& vStream = *(new VertexStream(8192));
			int cx = chunk->pos.x * 16;
			int cy = chunk->pos.y * 16;
			int cz = chunk->pos.z * 16;
				
			for(unsigned int i = 0; i < 16; i++)
			{
				for(unsigned int j = 0; j < 16; j++)
				{
					for(unsigned int k = 0; k < 16; k++)
					{
						if(world.getBlock(i | cx, j | cy, k | cz))
						{
							if(!world.getBlock(i | cx, j | cy, (k | cz) - 1))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(world.getBlock((i | cx) - 1, j | cy, (k | cz) - 1))
								{
									i1++;
									i4++;
								}

								if(world.getBlock(i | cx, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(world.getBlock((i | cx) + 1, j | cy, (k | cz) - 1))
								{
									i2++;
									i3++;
								}

								if(world.getBlock(i | cx, (j | cy) + 1, (k | cz) - 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) - 1))
								{
									i3++;
								}
								
								if(i4 < 2 && world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) - 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
							}
					
							if(!world.getBlock(i | cx, j | cy, (k | cz) + 1))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(world.getBlock((i | cx) - 1, j | cy, (k | cz) + 1))
								{
									i1++;
									i4++;
								}

								if(world.getBlock(i | cx, (j | cy) - 1, (k | cz) + 1))
								{
									i1++;
									i2++;
								}

								if(world.getBlock((i | cx) + 1, j | cy, (k | cz) + 1))
								{
									i2++;
									i3++;
								}

								if(world.getBlock(i | cx, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) + 1))
								{
									i1++;
								}

								if(i2 < 2 && world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) + 1))
								{
									i2++;
								}
								
								if(i3 < 2 && world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
							}

							if(!world.getBlock(i | cx, (j | cy) - 1, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(world.getBlock((i | cx) - 1, (j | cy) - 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(world.getBlock(i | cx, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(world.getBlock((i | cx) + 1, (j | cy) - 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(world.getBlock(i | cx, (j | cy) - 1, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 0.0f, f4, f4, f4, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
							}

							if(!world.getBlock(i | cx, (j | cy) + 1, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(world.getBlock((i | cx) - 1, (j | cy) + 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(world.getBlock(i | cx, (j | cy) + 1, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(world.getBlock((i | cx) + 1, (j | cy) + 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(world.getBlock(i | cx, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f, f1, f1, f1, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 1.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
							}

							if(!world.getBlock((i | cx) - 1, j | cy, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(world.getBlock((i | cx) - 1, (j | cy) - 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(world.getBlock((i | cx) - 1, j | cy, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(world.getBlock((i | cx) - 1, (j | cy) + 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(world.getBlock((i | cx) - 1, j | cy, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && world.getBlock((i | cx) - 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && world.getBlock((i | cx) - 1, (j | cy) - 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f, f4, f4, f4, 1.0f);
							}

							if(!world.getBlock((i | cx) + 1, j | cy, k | cz))
							{
								int i1 = 0;
								int i2 = 0;
								int i3 = 0;
								int i4 = 0;

								if(world.getBlock((i | cx) + 1, (j | cy) - 1, k | cz))
								{
									i1++;
									i4++;
								}

								if(world.getBlock((i | cx) + 1, j | cy, (k | cz) - 1))
								{
									i1++;
									i2++;
								}

								if(world.getBlock((i | cx) + 1, (j | cy) + 1, k | cz))
								{
									i2++;
									i3++;
								}

								if(world.getBlock((i | cx) + 1, j | cy, (k | cz) + 1))
								{
									i3++;
									i4++;
								}

								if(i1 < 2 && world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) - 1))
								{
									i1++;
								}

								if(i2 < 2 && world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) - 1))
								{
									i2++;
								}
								
								if(i3 < 2 && world.getBlock((i | cx) + 1, (j | cy) + 1, (k | cz) + 1))
								{
									i3++;
								}
								
								if(i4 < 2 && world.getBlock((i | cx) + 1, (j | cy) - 1, (k | cz) + 1))
								{
									i4++;
								}

								float f1 = 1.0F - i1 * 0.1f;
								float f2 = 1.0F - i2 * 0.1f;
								float f3 = 1.0F - i3 * 0.1f;
								float f4 = 1.0F - i4 * 0.1f;

								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f, f4, f4, f4, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f, f1, f1, f1, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f, f3, f3, f3, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f, f2, f2, f2, 1.0f);
								vStream << VertexUVRGBA(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f, f1, f1, f1, 1.0f);
							}
						}
					}
				}
			}
			
			chunk->mut.lock();
			chunk->setRenderUpdateNeeded(false);
			taskList.put(Tasks::TaskDelete(chunk->drawStream));
			chunk->drawStream = &vStream;
			chunk->swapStreams();
			chunk->mut.unlock();

			break;
		}
	}

	world.chunkMapLock.unlock();

	GameStates::swapProcessedPending();

	if((GameStates::processedState == GameStates::pendingState) | (GameStates::pendingState == GameStates::renderingState) | (GameStates::processedState == GameStates::renderingState))
	{
		stop = true;
	}

	Sleep(5);
	ticks++;
	return true;
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int _nCmdShow)
{
	for(int i = 0; i <= 8; i++)
	{
		for(int j = 0; j <= 8; j++)
		{
			grid << Vertex(i, j, 0);
			grid << Vertex(i, j, 8);
			grid << Vertex(i, 0, j);
			grid << Vertex(i, 8, j);
			grid << Vertex(0, i, j);
			grid << Vertex(8, i, j);
		}
	}

	square << Vertex(-1.0f, -1.0f, 0.0f);
	square << Vertex(1.0f, -1.0f, 0.0f);
	square << Vertex(-1.0f, 1.0f, 0.0f);
	square << Vertex(1.0f, -1.0f, 0.0f);
	square << Vertex(1.0f, 1.0f, 0.0f);
	square << Vertex(-1.0f, 1.0f, 0.0f);

	quad << Vertex(q.vec1.x, q.vec1.y, q.vec1.z);
	quad << Vertex(q.vec2.x, q.vec2.y, q.vec2.z);
	quad << Vertex(q.vec3.x, q.vec3.y, q.vec3.z);
	quad << Vertex(q.vec1.x, q.vec1.y, q.vec1.z);
	quad << Vertex(q.vec3.x, q.vec3.y, q.vec3.z);
	quad << Vertex(q.vec4.x, q.vec4.y, q.vec4.z);

	line << Vertex(l.vec1.x, l.vec1.y, l.vec1.z);
	line << Vertex(l.vec2.x, l.vec2.y, l.vec2.z);

	for(int i = 0; i < 32; i++)
	{
		for(int j = 3; j < 9; j++)
		{
			for(int k = 0; k < 32; k++)
			{
				Chunk* c = new Chunk(world, i, j, k);
				world.chunkMap[c->pos] = c;
				chunkLoadQueue.push(c);
			}
		}
	}

	IOUtil::init();

	GLWindow::instance = new GLWindow(L"Wind", 600, 600, _hInstance, 1, 2);
	GLWindow::instance->initWindow();

	thread graphicsThread(graphicsLoop);
	while(GLWindow::instance == nullptr || !GLWindow::instance->isOK())
	{

	}

	thread chunkThread(chunkLoaderLoop);

	GameStates::processedState = new GameState();
	GameStates::pendingState = new GameState();
	GameStates::renderingState = new GameState();

	MSG msg;

	while(!stop)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				stop = true;
			} else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else
		{
			enableRender = true;
			if((GLWindow::instance->active && !drawLoop()))
			{
				stop = true;
			}
		}
	}

	chunkThread.join();
	graphicsThread.join();

	delete GLWindow::instance;
	delete GameStates::renderingState;
	delete GameStates::processedState;
	delete GameStates::pendingState;
	return msg.wParam;
}