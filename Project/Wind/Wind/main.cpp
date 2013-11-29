#include "window.h"
#include "ioutil.h"
#include <iostream>
#include <sstream>
#include "graphics.h"
#include <thread>

using namespace gfxu;
using namespace RenderStates;
using namespace RenderActions;

VertexStream vStream(36);

std::thread graphicsThread;

int ticks = 0;
bool stop = false;
bool enableRender = false;

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

	//Texture2D testure(IOUtil::EXE_DIR + L"\\textures\\test.png", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
	Texture2D testure(256, 256, &noise1, &noise2, &noise3, &noise4);
	testure.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	while(!stop)
	{
		if(!enableRender)
		{
			continue;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, GLWindow::instance->width, GLWindow::instance->height);
		MatrixManager::reset();

		swapPendingRendering();
		RenderStates::pendingState->render();

		if(getError()) stop = true;

		GLWindow::instance->swapBuffers();
	}

	GLWindow::instance->destroyGL();
}

bool drawLoop()
{
	RenderStates::processedState->clean();

	RenderStates::processedState->put(new RAMatrixMult(PROJECTION_MATRIX, Matrix::ortho(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f)));
	RenderStates::processedState->put(new RAMatrixPerspective(PROJECTION_MATRIX));

	RenderStates::processedState->put(new RAMatrixMult(MODELVIEW_MATRIX, Matrix::rotate(ticks / 10.0f, 0.0f, 0.0f, 1.0f)));
	RenderStates::processedState->put(new RAMatrixMult(MODELVIEW_MATRIX, Matrix::rotate(ticks / 20.0f, 0.0f, 1.0f, 0.0f)));
	RenderStates::processedState->put(new RAMatrixMult(MODELVIEW_MATRIX, Matrix::rotate(ticks / 30.0f, 1.0f, 0.0f, 0.0f)));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, -1.5f, -1.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, -1.5f, -1.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, 0.5f, -1.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, 0.5f, -1.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, -1.5f, 0.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, -1.5f, 0.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(-1.5f, 0.5f, 0.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	RenderStates::processedState->put(new RAMatrixPush(MODELVIEW_MATRIX, Matrix::translate(0.5f, 0.5f, 0.5f)));
	RenderStates::processedState->put(new RAVertexStreamDraw(vStream));
	RenderStates::processedState->put(new RAMatrixPop(MODELVIEW_MATRIX));

	swapProcessedPending();

	Sleep(1);
	ticks++;
	return true;
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int _nCmdShow)
{
	/*vStream << VertexUVRGBA(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	
	vStream << VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vStream << VertexUVRGBA(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vStream << VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vStream << VertexUVRGBA(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vStream << VertexUVRGBA(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUVRGBA(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);*/

	vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	vStream << VertexUV(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	vStream << VertexUV(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vStream << VertexUV(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);

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
			}/* else
			{
				GLWindow::instance->swapBuffers();
			}*/
		}
	}

	graphicsThread.join();

	delete processedState;
	delete pendingState;
	delete renderingState;
	delete GLWindow::instance;
	return msg.wParam;
}