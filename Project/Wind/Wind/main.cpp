#define _USE_MATH_DEFINES

#include "window.h"
#include "ioutil.h"
#include "graphics.h"
#include "input.h"
#include <math.h>
#include "threads.h"
#include "opencl.h"

using namespace gfxu;
using namespace GameStates;

int ticks = 0;

int camX, camY, camZ;
int prevCamX = 0, prevCamY = 0, prevCamZ = 0;

const int renderDistance = 8;

bool mainLoop()
{
	Camera* cam = &GameStates::processedState->cam;

	for(int i = 0; i < Mouse::actions.getSize(); i++)
	{
		MouseAction ma = Mouse::actions[i];
		GameStates::processedState->FOV -= ma.sc * 5;
		if(Mouse::getMB(0))
		{
			cam->rot.x -= ma.ry * 0.5f;
			cam->rot.y -= ma.rx * 0.5f;
		}
	}

	for(int i = 0; i < Keyboard::actions.getSize(); i++)
	{
		KeyboardAction kba = Keyboard::actions[i];
		
		if(kba.released && kba.button == 69)
		{
			GameStates::processedState->devEnabled = !GameStates::processedState->devEnabled;
		}
	}

	if(GameStates::processedState->FOV < 5)
	{
		GameStates::processedState->FOV = 5;
	}
	else if(GameStates::processedState->FOV > 175)
	{
		GameStates::processedState->FOV = 175;
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
		GlobalThread::world.setBlock(floorf(cam->pos.x), floorf(cam->pos.y), floorf(cam->pos.z), Blocks::stone); //SPAAAAAAAAAAAAAACE!
	}

	if(Keyboard::getKey(17)) //CTRL
	{
		for(int i = -1; i < 2; i++)
		{
			for(int j = -1; j < 2; j++)
			{
				for(int k = -1; k < 2; k++)
				{
					GlobalThread::world.setBlock(floorf(cam->pos.x) + i, floorf(cam->pos.y) + j, floorf(cam->pos.z) + k, Blocks::air);
				}
			}
		}
	}

	GlobalThread::world.tick();

	Mouse::actions.clear();
	Keyboard::actions.clear();

	camX = cam->pos.x / 16.0f;
	camY = cam->pos.y / 16.0f;
	camZ = cam->pos.z / 16.0f;
	
	if(camX != prevCamX || camY != prevCamY || camZ != prevCamZ)
	{
		int xLimit = (camX > prevCamX ? camX : prevCamX) + renderDistance;
		int yLimit = (camY > prevCamY ? camY : prevCamY) + renderDistance;
		int zLimit = (camZ > prevCamZ ? camZ : prevCamZ) + renderDistance;
		GlobalThread::world.additionQueueLock.lock();
		GlobalThread::world.removalQueueLock.lock();
		for(int i = (camX < prevCamX ? camX : prevCamX) - renderDistance; i <= xLimit; i++)
		{
			int xDist = (i - camX) * (i - camX);
			int xPrevDist = (i - prevCamX) * (i - prevCamX);
			for(int j = (camY < prevCamY ? camY : prevCamY) - renderDistance; j <= yLimit; j++)
			{
				int yDist = (j - camY) * (j - camY);
				int yPrevDist = (j - prevCamY) * (j - prevCamY);
				for(int k = (camZ < prevCamZ ? camZ : prevCamZ) - renderDistance; k <= zLimit; k++)
				{
					int zDist = (k - camZ) * (k - camZ);
					int zPrevDist = (k - prevCamZ) * (k - prevCamZ);

					bool in1 = xDist + yDist + zDist <= renderDistance * renderDistance;
					bool in2 = xPrevDist + yPrevDist + zPrevDist <= renderDistance * renderDistance;

					if(j > 12) continue;

					std::shared_ptr<ChunkBase> c = GlobalThread::world.getChunk(i, j, k);

					if(in1 & !in2)
					{
						if(c->isEmpty())
						{
							c = std::shared_ptr<ChunkBase>(new Chunk(GlobalThread::world, i, j, k));
							GlobalThread::world.additionQueue.push(c);
							requestChunkLoad(c);
						}
					}
					else if(!in1 & in2)
					{
						if(!c->isEmpty())
						{
							GlobalThread::world.removalQueue.push(c);
						}
					}
				}
			}
		}
		GlobalThread::world.additionQueueLock.unlock();
		GlobalThread::world.removalQueueLock.unlock();
	}

	prevCamX = camX;
	prevCamY = camY;
	prevCamZ = camZ;

	GameStates::swapProcessedPending();

	if((GameStates::processedState == GameStates::pendingState) | (GameStates::pendingState == GameStates::renderingState) | (GameStates::processedState == GameStates::renderingState))
	{
		GlobalThread::stop = true;
	}

	ticks++;
	return true;
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int _nCmdShow)
{
	long long tickTime = 20000000;
	long long lastTick = 0;

	for(int r = 0; r <= renderDistance; r++)
	{
		int r1 = r * r;
		int r2 = r == 0 ? -1 : (r - 1) * (r - 1);
		for(int i = -r; i <= r; i++)
		{
			int xDist = i * i;
			for(int j = -r; j <= r; j++)
			{
				int yDist = xDist + j * j;
				for(int k = -r; k <= r; k++)
				{
					int zDist = yDist + k * k;
					bool b = GlobalThread::world.getChunk(i, j, k)->isEmpty();

					if(!b)
					{
						int f = 0;
					}

					if((zDist <= r1) && (zDist > r2))
					{
						std::shared_ptr<ChunkBase> c(new Chunk(GlobalThread::world, i, j, k));
						GlobalThread::world.chunkMap[c->pos] = c;
						requestChunkLoad(c);
					}
				}
			}
		}
	}

	IOUtil::staticInit();
	cl::staticInit();
	Noise::staticInit();
	Blocks::staticInit();
	ChunkDrawThread::staticInit();
	ChunkLoadThread::staticInit();

	GLWindow::instance = new GLWindow(L"Wind", 600, 600, _hInstance, 3, 4);
	GLWindow::instance->initWindow();

	renderThread.start();
	while(GLWindow::instance == nullptr || !GLWindow::instance->isOK())
	{

	}

	for(int i = 0; i < LOAD_THREAD_AMOUNT; i++)
	{
		chunkLoadThreads[i].start();
	}

	for(int i = 0; i < DRAW_THREAD_AMOUNT; i++)
	{
		chunkDrawThreads[i].start();
	}

	GameStates::staticInit();

	MSG msg;

	std::chrono::system_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	while(!GlobalThread::stop)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				GlobalThread::stop = true;
			} else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else
		{
			std::chrono::system_clock::time_point now = std::chrono::high_resolution_clock::now();
			long long difference = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime).count();

			long long tick = difference / tickTime;
			long long ticksToProcess = tick - lastTick;

			if(ticksToProcess > 0)
			{
				lastTick++;
				RenderThread::skipRender = false;
				if(GLWindow::instance->active && !mainLoop())
				{
					GlobalThread::stop = true;
				}
			}
		}
	}

	for(int i = 0; i < LOAD_THREAD_AMOUNT; i++)
	{
		chunkLoadThreads[i].stop();
	}
	for(int i = 0; i < DRAW_THREAD_AMOUNT; i++)
	{
		chunkDrawThreads[i].stop();
	}
	
	renderThread.stop();

	delete GLWindow::instance;
	GameStates::cleanup();
	cl::unload();
	Blocks::destroy();
	return msg.wParam;
}