#include "threads.h"
#include "window.h"
#include "ioutil.h"
#include <sstream>

using namespace RenderThread;

bool RenderThread::skipRender = true;
std::thread RenderThread::thread;
VirtualList<Tasks::Task, 256, 4098> RenderThread::taskList;

gfxu::VertexStream RenderThread::grid;
gfxu::VertexStream RenderThread::square;

void RenderThread::loop()
{
	GLWindow::instance->initGL();
	
	if(!GLWindow::instance->isOK())
	{
		return;
	}

	std::wstring filePath(IOUtil::EXE_DIR);
	filePath += L"\\shaders\\normal.vert";
	gfxu::VertexShader normalVShader(filePath);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\normal.frag";
	gfxu::FragmentShader normalFShader(filePath);

	gfxu::ShaderProgram normalShaderProgram(&normalVShader, nullptr, &normalFShader);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\noTex.vert";
	gfxu::VertexShader noTexVShader(filePath);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\noTex.frag";
	gfxu::FragmentShader noTexFShader(filePath);

	gfxu::ShaderProgram noTexShaderProgram(&noTexVShader, nullptr, &noTexFShader);

	gfxu::Uniforms::reset();

	Noise::NoiseGenerator2D noise1(8, 8, 2.0f, 0);
	Noise::NoiseGenerator2D noise2(8, 8, 2.0f, 1);
	Noise::NoiseGenerator2D noise3(8, 8, 2.0f, 2);
	Noise::NoiseGenerator2D noise4(8, 8, 2.0f, 3);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\textures\\test.png";
	gfxu::Texture2D testure(filePath, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
	//Texture2D testure(256, 256, &noise1, &noise2, &noise3, &noise4);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!GlobalThread::stop)
	{
		for(int i = 0; i < RenderThread::taskList.getSize(); i++)
		{
			if(!RenderThread::taskList[i]->invoke())
			{
				GlobalThread::stop = true;
			}
		}

		RenderThread::taskList.clear();

		if(RenderThread::skipRender)
		{
			continue;
		}

		GameStates::swapPendingRendering();

		GameStates::GameState* state = GameStates::renderingState;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(GLWindow::instance->rescaled)
		{
			glViewport(0, 0, GLWindow::instance->width, GLWindow::instance->height);
			GLWindow::instance->rescaled = false;
		}
		
		normalShaderProgram.bind();
		gfxu::Uniforms::color.set(1.0f, 1.0f, 1.0f, 1.0f);
		gfxu::Uniforms::reset();
		gfxu::Uniforms::PMS.mult(geom::Matrix::perspective(90.0f, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.1f, 128.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.x, 1.0f, 0.0f, 0.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.y, 0.0f, 1.0f, 0.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.z, 0.0f, 0.0f, 1.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::translate(-state->cam.pos.x, -state->cam.pos.y, -state->cam.pos.z));
		
		testure.bind();

		geom::AxisAlignedCube cube(geom::Vector(-1.5f, -1.5f, -1.5f), geom::Vector(3.0f, 3.0f, 3.0f));
		int xCam = floorf(state->cam.pos.x / 16.0f);
		int yCam = floorf(state->cam.pos.y / 16.0f);
		int zCam = floorf(state->cam.pos.z / 16.0f);
		bool bGrid[17][17][17];
		for(int i = 0; i < 17; i++)
		{
			int f1 = i - 8;
			for(int j = 0; j < 17; j++)
			{
				int f2 = j - 8;
				for(int k = 0; k < 17; k++)
				{
					int f3 = k - 8;
					bGrid[i][j][k] = cube.inside((gfxu::Uniforms::PMS.getTopmost() * geom::Vector((xCam + f1) * 16.0f, (yCam + f2) * 16.0f, (zCam + f3) * 16.0f)).wDivide());
				}
			}
		}

		while(!GlobalThread::world.additionQueue.empty())
		{
			std::shared_ptr<ChunkBase> c = GlobalThread::world.additionQueue.front();
			GlobalThread::world.additionQueue.pop();
			GlobalThread::world.chunkMap[c->pos] = c;
		}

		while(!GlobalThread::world.removalQueue.empty())
		{
			ChunkPosition cp = GlobalThread::world.removalQueue.front();
			GlobalThread::world.removalQueue.pop();
			GlobalThread::world.chunkMap.erase(cp);
		}

		for(map<ChunkPosition, std::shared_ptr<ChunkBase>>::const_iterator iter = GlobalThread::world.chunkMap.begin(); iter != GlobalThread::world.chunkMap.end(); ++iter)
		{
			std::shared_ptr<ChunkBase> chunk = iter->second;

			if(chunk != nullptr && chunk->isLoaded())
			{
				if(chunk->pos.x - 8 <= xCam && chunk->pos.x + 8 >= xCam && chunk->pos.y - 8 <= yCam && chunk->pos.y + 8 >= yCam && chunk->pos.z - 8 <= zCam && chunk->pos.z + 8 > zCam)
				{
					if(bGrid[chunk->pos.x - xCam + 8][chunk->pos.y - yCam + 8][chunk->pos.z - zCam + 8] |
						bGrid[chunk->pos.x - xCam + 9][chunk->pos.y - yCam + 8][chunk->pos.z - zCam + 8] |
						bGrid[chunk->pos.x - xCam + 8][chunk->pos.y - yCam + 9][chunk->pos.z - zCam + 8] |
						bGrid[chunk->pos.x - xCam + 9][chunk->pos.y - yCam + 9][chunk->pos.z - zCam + 8] |
						bGrid[chunk->pos.x - xCam + 8][chunk->pos.y - yCam + 8][chunk->pos.z - zCam + 9] |
						bGrid[chunk->pos.x - xCam + 9][chunk->pos.y - yCam + 8][chunk->pos.z - zCam + 9] |
						bGrid[chunk->pos.x - xCam + 8][chunk->pos.y - yCam + 9][chunk->pos.z - zCam + 9] |
						bGrid[chunk->pos.x - xCam + 9][chunk->pos.y - yCam + 9][chunk->pos.z - zCam + 9])
					{
						gfxu::Uniforms::MMS.push(geom::Matrix::translate(chunk->pos.x * 16, chunk->pos.y * 16, chunk->pos.z * 16));
						chunk->mut.lock();
						chunk->renderStream->draw();
						chunk->mut.unlock();
						gfxu::Uniforms::MMS.pop();
					}
				}
			}
		}

		glEnable(GL_BLEND);
		noTexShaderProgram.bind();
		gfxu::Uniforms::MMS.push(geom::Matrix::scale(16.0f * 32.0f, 1.0f, 16.0f * 32.0f));
		gfxu::Uniforms::MMS.mult(geom::Matrix::translate(0.0f, -0.125f, 0.0f));
		gfxu::Uniforms::color.set(0.0f, 0.0f, 0.25f, 0.5f);
		square.draw();
		gfxu::Uniforms::MMS.pop();

		gfxu::Uniforms::MMS.push(geom::Matrix::scale(16.0f, 16.0f, 16.0f));
		gfxu::Uniforms::MMS.mult(geom::Matrix::translate(xCam, yCam, zCam));
		gfxu::Uniforms::color.set(1.0f, 0.0f, 0.0f, 0.125f);
		grid.draw(GL_LINES);

		glDepthFunc(GL_GREATER);
		gfxu::Uniforms::color.set(1.0f, 0.0f, 0.0f, 0.0625f);
		grid.draw(GL_LINES);
		gfxu::Uniforms::MMS.pop();
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);

		glFlush();
		if(gfxu::getError()) GlobalThread::stop = true;
		GLWindow::instance->swapBuffers();
	}

	GLWindow::instance->destroyGL();
}