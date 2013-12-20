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

using namespace gfxu;
using namespace GameStates;
using namespace RenderActions;

std::thread graphicsThread;
std::thread chunkThread;

int ticks = 0;
bool stop = false;
bool enableRender = false;

World world;

std::queue<Chunk*> chunkLoadQueue;

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
	normalShaderProgram.create();
	normalShaderProgram.bind();

	MatrixManager::reset();

	Noise::NoiseGenerator2D noise1(8, 8, 2.0f, 0);
	Noise::NoiseGenerator2D noise2(8, 8, 2.0f, 1);
	Noise::NoiseGenerator2D noise3(8, 8, 2.0f, 2);
	Noise::NoiseGenerator2D noise4(8, 8, 2.0f, 3);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\textures\\test.png";
	Texture2D testure(filePath, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
	//Texture2D testure(256, 256, &noise1, &noise2, &noise3, &noise4);
	testure.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while(!stop)
	{
		if(!enableRender)
		{
			continue;
		}

		swapPendingRendering();

		if(GameStates::renderingState->isEmpty())
		{
			continue;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(GLWindow::instance->rescaled)
		{
			glViewport(0, 0, GLWindow::instance->width, GLWindow::instance->height);
			GLWindow::instance->rescaled = false;
		}
		
		MatrixManager::reset();
		GameStates::renderingState->render();

		if(getError()) stop = true;

		glFlush();

		GLWindow::instance->swapBuffers();
	}

	GLWindow::instance->destroyGL();
}

void chunkLoaderLoop()
{
	Noise::NoiseGenerator3D noise(8, 8, 2.0f, 3);

	while(!stop)
	{
		if(chunkLoadQueue.empty())
		{
			continue;
		}

		Chunk* chunk = chunkLoadQueue.front();
		chunkLoadQueue.pop();
		
		float treshold = 0.5f;

		for(unsigned int i = 0; i < 16; i++)
		{
			for(unsigned int j = 0; j < 16; j++)
			{
				for(unsigned int k = 0; k < 16; k++)
				{
					if(noise.getNoise(i + chunk->pos.x * 16, j + chunk->pos.y * 16, k + chunk->pos.z * 16) >= treshold)
					{
						chunk->data[i][j][k] = 1;
					}
					else
					{
						chunk->data[i][j][k] = 0;
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

	if(Keyboard::getKey(87)) //W
	{
		cam->pos.x += 0.1f * cos(cam->rot.x * M_PI / 180.0f) * sin(cam->rot.y * M_PI / 180.0f);
		cam->pos.y -= 0.1f * sin(cam->rot.x * M_PI / 180.0f);
		cam->pos.z -= 0.1f * cos(cam->rot.x * M_PI / 180.0f) * cos(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(83)) //S
	{
		cam->pos.x -= 0.1f * cos(cam->rot.x * M_PI / 180.0f) * sin(cam->rot.y * M_PI / 180.0f);
		cam->pos.y += 0.1f * sin(cam->rot.x * M_PI / 180.0f);
		cam->pos.z += 0.1f * cos(cam->rot.x * M_PI / 180.0f) * cos(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(65)) //A
	{
		cam->pos.x += 0.1f * cos(cam->rot.y * M_PI / 180.0f);
		cam->pos.z += 0.1f * sin(cam->rot.y * M_PI / 180.0f);
	}

	if(Keyboard::getKey(68)) //D
	{
		cam->pos.x -= 0.1f * cos(cam->rot.y * M_PI / 180.0f);
		cam->pos.z -= 0.1f * sin(cam->rot.y * M_PI / 180.0f);
	}

	Mouse::actions.clear();
	Keyboard::actions.clear();

	GameStates::processedState->clean();

	GameStates::processedState->put(RAMatrixPerspective(PROJECTION_MATRIX));
	
	GameStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::rotate(cam->rot.x, 1.0f, 0.0f, 0.0f)));
	GameStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::rotate(cam->rot.y, 0.0f, 1.0f, 0.0f)));
	GameStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::rotate(cam->rot.z, 0.0f, 0.0f, 1.0f)));
	GameStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::translate(cam->pos.x, cam->pos.y, cam->pos.z)));

	world.chunkMapLock.lock();
	for(map<ChunkPosition, Chunk*>::const_iterator iter = world.chunkMap.begin(); iter != world.chunkMap.end(); ++iter)
	{
		Chunk* chunk = iter->second;

		if(chunk != nullptr && chunk->isLoaded() && chunk->isRenderUpdateNeeded())
		{
			/*if(!world.isChunkLoaded(chunk->pos.x - 1, chunk->pos.y, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x + 1, chunk->pos.y, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y - 1, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y + 1, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y, chunk->pos.z - 1) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y, chunk->pos.z + 1))
			{
				continue;
			}*/

			VertexStream& vStream = chunk->vStream;
			int cx = chunk->pos.x << 4;
			int cy = chunk->pos.y << 4;
			int cz = chunk->pos.z << 4;

			vStream.lock();
			vStream.clear();
				
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
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
							}
					
							if(!world.getBlock(i | cx, j | cy, (k | cz) + 1))
							{
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
							}

							if(!world.getBlock(i | cx, (j | cy) - 1, k | cz))
							{
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
							}

							if(!world.getBlock(i | cx, (j | cy) + 1, k | cz))
							{
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
							}

							if(!world.getBlock((i | cx) - 1, j | cy, k | cz))
							{
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
							}

							if(!world.getBlock((i | cx) + 1, j | cy, k | cz))
							{
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
								vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
							}
						}
					}
				}
			}
			
			chunk->setRenderUpdateNeeded(false);
			vStream.unlock();

			break;
		}
	}

	for(map<ChunkPosition, Chunk*>::const_iterator iter = world.chunkMap.begin(); iter != world.chunkMap.end(); ++iter)
	{
		Chunk* chunk = iter->second;

		if(chunk != nullptr && chunk->isLoaded())
		{
			/*if(!world.isChunkLoaded(chunk->pos.x - 1, chunk->pos.y, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x + 1, chunk->pos.y, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y - 1, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y + 1, chunk->pos.z) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y, chunk->pos.z - 1) ||
				!world.isChunkLoaded(chunk->pos.x, chunk->pos.y, chunk->pos.z + 1))
			{
				continue;
			}*/

			GameStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(chunk->pos.x << 4, chunk->pos.y << 4, chunk->pos.z << 4)));
			GameStates::processedState->put(RAVertexStreamDraw(&chunk->vStream));
			GameStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));
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
	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			for(int k = 0; k < 10; k++)
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

	//chunkThread.join();
	graphicsThread.join();

	delete GLWindow::instance;
	delete GameStates::renderingState;
	delete GameStates::processedState;
	delete GameStates::pendingState;
	return msg.wParam;
}