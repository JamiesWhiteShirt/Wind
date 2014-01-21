#define _USE_MATH_DEFINES

#include "window.h"
#include "ioutil.h"
#include "graphics.h"
#include "input.h"
#include <math.h>
#include "threads.h"

using namespace gfxu;
using namespace GameStates;

int ticks = 0;

int camX, camY, camZ;
int prevCamX = 0, prevCamY = 0, prevCamZ = 0;

bool mainLoop()
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
		GlobalThread::world.setBlock((int)cam->pos.x, (int)cam->pos.y, (int)cam->pos.z, 1); //SPAAAAAAAAAAAAAACE!
	}

	Mouse::actions.clear();
	Keyboard::actions.clear();

	camX = cam->pos.x / 16.0f;
	camY = cam->pos.y / 16.0f;
	camZ = cam->pos.z / 16.0f;
	
	if(camX != prevCamX || camY != prevCamY || camZ != prevCamZ)
	{
		int xLimit = (camX > prevCamX ? camX : prevCamX) + 9;
		int yLimit = (camY > prevCamY ? camY : prevCamY) + 9;
		int zLimit = (camZ > prevCamZ ? camZ : prevCamZ) + 9;
		for(int i = (camX < prevCamX ? camX : prevCamX) - 9; i < xLimit; i++)
		{
			int xDist = (i - camX) * (i - camX);
			int xPrevDist = (i - prevCamX) * (i - prevCamX);
			for(int j = (camY < prevCamY ? camY : prevCamY) - 9; j < yLimit; j++)
			{
				int yDist = (j - camY) * (j - camY);
				int yPrevDist = (j - prevCamY) * (j - prevCamY);
				for(int k = (camZ < prevCamZ ? camZ : prevCamZ) - 9; k < zLimit; k++)
				{
					int zDist = (k - camZ) * (k - camZ);
					int zPrevDist = (k - prevCamZ) * (k - prevCamZ);

					bool in1 = xDist + yDist + zDist <= 8 * 8;
					bool in2 = xPrevDist + yPrevDist + zPrevDist <= 8 * 8;

					std::shared_ptr<ChunkBase> c = GlobalThread::world.getChunk(i, j, k);

					if(in1 & !in2)
					{
						if(c == nullptr || !c->isLoaded())
						{
							c = std::shared_ptr<ChunkBase>(new Chunk(GlobalThread::world, i, j, k));
							GlobalThread::world.additionQueue.push(c);
							ChunkLoadThread::loadQueue.push(c);
						}
					}
					else if(!in1 & in2)
					{
						if(c != nullptr)
						{
							GlobalThread::world.removalQueue.push(c->pos);
						}
					}

					//ChunkBase* c = GlobalThread::world.getChunk(i, j, k);
					//GlobalThread::world.chunkMap[c->pos] = c;
					//ChunkLoadThread::loadQueue.push(c);
				}
			}
		}
	}

	prevCamX = camX;
	prevCamY = camY;
	prevCamZ = camZ;

	GameStates::swapProcessedPending();

	if((GameStates::processedState == GameStates::pendingState) | (GameStates::pendingState == GameStates::renderingState) | (GameStates::processedState == GameStates::renderingState))
	{
		GlobalThread::stop = true;
	}

	Sleep(5);
	ticks++;
	return true;
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int _nCmdShow)
{
	for(int i = -8; i <= 8; i++)
	{
		for(int j = -8; j <= 8; j++)
		{
			RenderThread::grid << Vertex(i, j, -8);
			RenderThread::grid << Vertex(i, j, 8);
			RenderThread::grid << Vertex(i, -8, j);
			RenderThread::grid << Vertex(i, 8, j);
			RenderThread::grid << Vertex(-8, i, j);
			RenderThread::grid << Vertex(8, i, j);
		}
	}

	RenderThread::square << Vertex(0.0f, 0.0f, 0.0f);
	RenderThread::square << Vertex(1.0f, 0.0f, 0.0f);
	RenderThread::square << Vertex(1.0f, 0.0f, 1.0f);
	RenderThread::square << Vertex(1.0f, 0.0f, 1.0f);
	RenderThread::square << Vertex(0.0f, 0.0f, 1.0f);
	RenderThread::square << Vertex(0.0f, 0.0f, 0.0f);

	/*for(int i = 0; i < 32; i++)
	{
		for(int j = -2; j < 4; j++)
		{
			for(int k = 0; k < 32; k++)
			{
				Chunk* c = new Chunk(GlobalThread::world, i, j, k);
				GlobalThread::world.chunkMap[c->pos] = c;
				ChunkLoadThread::loadQueue.push(c);
			}
		}
	}*/

	IOUtil::init();

	GLWindow::instance = new GLWindow(L"Wind", 600, 600, _hInstance, 1, 2);
	GLWindow::instance->initWindow();

	RenderThread::thread = std::thread(RenderThread::loop);
	while(GLWindow::instance == nullptr || !GLWindow::instance->isOK())
	{

	}

	ChunkLoadThread::thread = std::thread(ChunkLoadThread::loop);
	ChunkDrawThread::thread = std::thread(ChunkDrawThread::loop);

	GameStates::init();

	MSG msg;

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
			RenderThread::skipRender = false;
			if((GLWindow::instance->active && !mainLoop()))
			{
				GlobalThread::stop = true;
			}
		}
	}

	RenderThread::thread.join();
	ChunkLoadThread::thread.join();
	ChunkDrawThread::thread.join();

	delete GLWindow::instance;
	GameStates::cleanup();
	return msg.wParam;
}