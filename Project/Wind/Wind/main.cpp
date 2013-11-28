#include "window.h"
#include "ioutil.h"
#include <iostream>
#include <sstream>
#include "graphics.h"
//#include "noise.h"

using namespace gfxu;

VertexStream vStream(36);

int time = 0;

bool drawLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, GLWindow::instance->width, GLWindow::instance->height);

	MatrixManager::uniformP();
	MatrixManager::multP(Matrix::ortho(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f));
	MatrixManager::multP(Matrix::perspective(90.0f, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.01f, 100.0f));

	MatrixManager::uniformM();
	MatrixManager::multM(Matrix::rotate(time / 100.0f, 0.0f, 0.0f, 1.0f));
	MatrixManager::multM(Matrix::rotate(time / 200.0f, 0.0f, 1.0f, 0.0f));
	MatrixManager::multM(Matrix::rotate(time / 300.0f, 1.0f, 0.0f, 0.0f));

	MatrixManager::pushM(Matrix::translate(-1.5f, -1.5f, -1.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(0.5f, -1.5f, -1.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(-1.5f, 0.5f, -1.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(0.5f, 0.5f, -1.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(-1.5f, -1.5f, 0.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(0.5f, -1.5f, 0.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(-1.5f, 0.5f, 0.5f));
	vStream.draw();
	MatrixManager::popM();

	MatrixManager::pushM(Matrix::translate(0.5f, 0.5f, 0.5f));
	vStream.draw();
	MatrixManager::popM();

	time++;
	if(getError()) return false;

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
	
	GLWindow::instance = new GLWindow(L"Wind", 600, 600, _hInstance, 1, 2);
	GLWindow::instance->initWindow();

	if(!GLWindow::instance->isOK())
	{
		delete GLWindow::instance;
		return 0;
	}

	IOUtil::init();
	
	std::wstring filePath = IOUtil::EXE_DIR + L"\\shaders\\normal.vert";
	VertexShader normalVShader(filePath);

	filePath = IOUtil::EXE_DIR + L"\\shaders\\normal.frag";
	FragmentShader normalFShader(filePath);
	
	/*filePath = IOUtil::EXE_DIR + L"\\shaders\\normal.geom";
	GeometryShader normalGShader(filePath);*/

	ShaderProgram normalShaderProgram(&normalVShader, nullptr, &normalFShader);
	normalShaderProgram.create();

	normalShaderProgram.bind();

	MatrixManager::reset();
	/*Matrix mat;
	mat.data[0] = 1.0f / 3.0f;
	mat.data[1] = 1.0f / 3.0f;
	mat.data[2] = 1.0f / 3.0f;
	mat.data[4] = 1.0f / 3.0f;
	mat.data[5] = 1.0f / 3.0f;
	mat.data[6] = 1.0f / 3.0f;
	mat.data[8] = 1.0f / 3.0f;
	mat.data[9] = 1.0f / 3.0f;
	mat.data[10] = 1.0f / 3.0f;
	mat.data[15] = 1.0f;
	MatrixManager::setColorManipulation(mat);*/

	Noise::NoiseGenerator2D noise1(8, 8, 2.0f, 0);

	//Texture2D testure(IOUtil::EXE_DIR + L"\\textures\\test.png", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
	Texture2D testure(256, 256, &noise1, &noise1, &noise1, nullptr);
	testure.bind();

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	MSG msg;
	bool done = false;

	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				done = true;
			} else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else
		{
			if((GLWindow::instance->active && !drawLoop()))
			{
				done = true;
			} else
			{
				GLWindow::instance->swapBuffers();
			}
		}
	}

	delete GLWindow::instance;
	return msg.wParam;
}