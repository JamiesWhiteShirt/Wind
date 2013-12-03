#define _USE_MATH_DEFINES

#include "window.h"
#include "ioutil.h"
#include <iostream>
#include <sstream>
#include "graphics.h"
#include <thread>
#include "input.h"
#include <math.h>

using namespace gfxu;
using namespace RenderStates;
using namespace RenderActions;

VertexStream vStream(36);

std::thread graphicsThread;

int ticks = 0;
bool stop = false;
bool enableRender = false;

unsigned int chunk_size = 64;
float treshold = 0.5f;

void graphicsLoop()
{
GLWindow::instance->initGL();

	if(!GLWindow::instance->isOK())
	{
		delete GLWindow::instance;
		return;
	}

	std::wstring filePath = IOUtil::EXE_DIR + L"\\shaders\\normal.vert";
	VertexShader normalVShader(filePath);

	filePath = IOUtil::EXE_DIR + L"\\shaders\\normal.frag";
	FragmentShader normalFShader(filePath);

	ShaderProgram normalShaderProgram(&normalVShader, nullptr, &normalFShader);
	normalShaderProgram.create();
	normalShaderProgram.bind();

	MatrixManager::reset();

	Noise::NoiseGenerator2D noise1(8, 8, 2.0f, 0);
	Noise::NoiseGenerator2D noise2(8, 8, 2.0f, 1);
	Noise::NoiseGenerator2D noise3(8, 8, 2.0f, 2);
	Noise::NoiseGenerator2D noise4(8, 8, 2.0f, 3);

	Texture2D testure(IOUtil::EXE_DIR + L"\\textures\\test.png", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
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

		if(RenderStates::renderingState->isEmpty())
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
		RenderStates::renderingState->render();

		if(getError()) stop = true;

		glFlush();

		GLWindow::instance->swapBuffers();
	}

	GLWindow::instance->destroyGL();
}

bool drawLoop()
{
	Camera* cam = &RenderStates::processedState->cam;

	if(Mouse::getMB(0))
	{
		for(int i = 0; i < Mouse::actions.size(); i++)
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

	RenderStates::processedState->clean();

	RenderStates::processedState->put(RAMatrixPerspective(PROJECTION_MATRIX));
	
	RenderStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::rotate(cam->rot.x, 1.0f, 0.0f, 0.0f)));
	RenderStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::rotate(cam->rot.y, 0.0f, 1.0f, 0.0f)));
	RenderStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::rotate(cam->rot.z, 0.0f, 0.0f, 1.0f)));
	RenderStates::processedState->put(RAMatrixMult(PROJECTION_MATRIX, Matrix::translate(cam->pos.x, cam->pos.y, cam->pos.z)));

	/*RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, -1.5f, -1.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, -1.5f, -1.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, 0.5f, -1.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, 0.5f, -1.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, -1.5f, 0.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, -1.5f, 0.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, 0.5f, 0.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, 0.5f, 0.5f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));*/

	RenderStates::processedState->put(RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(chunk_size / -2.0f, chunk_size / -2.0f, chunk_size / -2.0f)));
	RenderStates::processedState->put(RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(RAMatrixPop(MODELVIEW_MATRIX));

	swapProcessedPending();

	if((processedState == pendingState) | (pendingState == renderingState) | (processedState == renderingState))
	{
		stop = true;
	}

	Sleep(5);
	ticks++;
	return true;
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int _nCmdShow)
{
	/*vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);*/

	time_t timev;

	Noise::NoiseGenerator3D noise(8, 8, 2.0f, time(&timev));

	for(unsigned int i = 0; i < chunk_size; i++)
	{
		for(unsigned int j = 0; j < chunk_size; j++)
		{
			for(unsigned int k = 0; k < chunk_size; k++)
			{
				if(noise.getNoise(i, j, k) >= treshold)
				{
					if((noise.getNoise(i, j, k - 1) < treshold) | (k == 0))
					{
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
					}
					
					if((noise.getNoise(i, j, k + 1) < treshold) | (k == chunk_size - 1))
					{
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
					}

					if((noise.getNoise(i, j - 1, k) < treshold) | (j == 0))
					{
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
					}

					if((noise.getNoise(i, j + 1, k) < treshold) | (j == chunk_size - 1))
					{
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
					}

					if((noise.getNoise(i - 1, j, k) < treshold) | (i == 0))
					{
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 0.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 1.0f);
						vStream << VertexUV(i + 0.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 1.0f);
					}

					if((noise.getNoise(i + 1, j, k) < treshold) | (i == chunk_size - 1))
					{
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 1.0f, 1.0f, 0.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 1.0f, 1.0f, 1.0f, 0.0f);
						vStream << VertexUV(i + 1.0f, j + 1.0f, k + 0.0f, 1.0f, 1.0f, 1.0f);
						vStream << VertexUV(i + 1.0f, j + 0.0f, k + 0.0f, 1.0f, 0.0f, 1.0f);
					}
				}
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

	processedState = new RenderState();
	pendingState = new RenderState();
	renderingState = new RenderState();

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

	graphicsThread.join();

	delete GLWindow::instance;
	delete renderingState;
	delete processedState;
	delete pendingState;
	return msg.wParam;
}