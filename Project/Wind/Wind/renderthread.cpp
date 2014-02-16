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

const int renderDistance = 8;
bool bGrid[renderDistance * 2 + 1][renderDistance * 2 + 1][renderDistance * 2 + 1];

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

	glClearColor(0.5f, 0.875f, 1.0f, 1.0f);
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
		gfxu::Uniforms::camPos.set(state->cam.pos);
		gfxu::Uniforms::setColor(1.0f, 1.0f, 1.0f, 1.0f);
		gfxu::Uniforms::setFogColor(0.5f, 0.875f, 1.0f, 1.0f);
		gfxu::Uniforms::fogDist.set(16.0f * renderDistance);
		gfxu::Uniforms::reset();
		gfxu::Uniforms::PMS.mult(geom::Matrix::perspective(state->FOV, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.1f, 16.0f * renderDistance));
		gfxu::Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.x, 1.0f, 0.0f, 0.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.y, 0.0f, 1.0f, 0.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::rotate(state->cam.rot.z, 0.0f, 0.0f, 1.0f));
		gfxu::Uniforms::PMS.mult(geom::Matrix::translate(-state->cam.pos.x, -state->cam.pos.y, -state->cam.pos.z));
		
		testure.bind();

		float cScale = 1.0 + 90.0f / state->FOV;
		geom::AxisAlignedCube cube(geom::Vector(-cScale, -cScale, -cScale), geom::Vector(2.0f, 2.0f, 2.0f) * cScale);
		int xCam = floorf(state->cam.pos.x / 16.0f);
		int yCam = floorf(state->cam.pos.y / 16.0f);
		int zCam = floorf(state->cam.pos.z / 16.0f);
		for(int i = 0; i <= renderDistance * 2; i++)
		{
			int f1 = i - renderDistance;
			for(int j = 0; j <= renderDistance * 2; j++)
			{
				int f2 = j - renderDistance;
				for(int k = 0; k <= renderDistance * 2; k++)
				{
					int f3 = k - renderDistance;
					bGrid[i][j][k] = cube.inside((gfxu::Uniforms::PMS.getTopmost() * geom::Vector((xCam + f1) * 16.0f, (yCam + f2) * 16.0f, (zCam + f3) * 16.0f)).wDivide());
				}
			}
		}

		GlobalThread::world.chunkMapLock.lock();
		GlobalThread::world.additionQueueLock.lock();
		while(!GlobalThread::world.additionQueue.empty())
		{
			std::shared_ptr<ChunkBase> c = GlobalThread::world.additionQueue.front();
			GlobalThread::world.additionQueue.pop();
			GlobalThread::world.addChunk(c);
		}
		GlobalThread::world.additionQueueLock.unlock();
		
		GlobalThread::world.removalQueueLock.lock();
		while(!GlobalThread::world.removalQueue.empty())
		{
			std::shared_ptr<ChunkBase> c = GlobalThread::world.removalQueue.front();
			c->setUnloaded();
			GlobalThread::world.removalQueue.pop();
			GlobalThread::world.removeChunk(c->pos);
		}
		GlobalThread::world.removalQueueLock.unlock();

		std::queue<shared_ptr<ChunkBase>> chunksToRender;
		for(map<ChunkPosition, std::shared_ptr<ChunkBase>>::const_iterator iter = GlobalThread::world.chunkMap.begin(); iter != GlobalThread::world.chunkMap.end(); ++iter)
		{
			chunksToRender.push(iter->second);
		}
		GlobalThread::world.chunkMapLock.unlock();

		while(!chunksToRender.empty())
		{
			std::shared_ptr<ChunkBase> chunk = chunksToRender.front();
			chunksToRender.pop();

			if(!chunk->isEmpty() && chunk->isLoaded())
			{
				if(chunk->pos.x - renderDistance <= xCam && chunk->pos.x + renderDistance >= xCam && chunk->pos.y - renderDistance <= yCam && chunk->pos.y + renderDistance >= yCam && chunk->pos.z - renderDistance <= zCam && chunk->pos.z + renderDistance > zCam)
				{
					if(chunk->renderStream != nullptr && (
						bGrid[chunk->pos.x - xCam + renderDistance][chunk->pos.y - yCam + renderDistance][chunk->pos.z - zCam + renderDistance] |
						bGrid[chunk->pos.x - xCam + renderDistance + 1][chunk->pos.y - yCam + renderDistance][chunk->pos.z - zCam + renderDistance] |
						bGrid[chunk->pos.x - xCam + renderDistance][chunk->pos.y - yCam + renderDistance + 1][chunk->pos.z - zCam + renderDistance] |
						bGrid[chunk->pos.x - xCam + renderDistance + 1][chunk->pos.y - yCam + renderDistance + 1][chunk->pos.z - zCam + renderDistance] |
						bGrid[chunk->pos.x - xCam + renderDistance][chunk->pos.y - yCam + renderDistance][chunk->pos.z - zCam + renderDistance + 1] |
						bGrid[chunk->pos.x - xCam + renderDistance + 1][chunk->pos.y - yCam + renderDistance][chunk->pos.z - zCam + renderDistance + 1] |
						bGrid[chunk->pos.x - xCam + renderDistance][chunk->pos.y - yCam + renderDistance + 1][chunk->pos.z - zCam + renderDistance + 1] |
						bGrid[chunk->pos.x - xCam + renderDistance + 1][chunk->pos.y - yCam + renderDistance + 1][chunk->pos.z - zCam + renderDistance + 1]))
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
		/*gfxu::Uniforms::MMS.push(geom::Matrix::scale(16.0f * 32.0f, 1.0f, 16.0f * 32.0f));
		gfxu::Uniforms::MMS.mult(geom::Matrix::translate(0.0f, -0.125f, 0.0f));
		gfxu::Uniforms::color.set(0.0f, 0.0f, 0.25f, 0.5f);
		square.draw();
		gfxu::Uniforms::MMS.pop();*/
		
		gfxu::Uniforms::setFogColor(1.0f, 0.0f, 0.0f, 0.0f);
		gfxu::Uniforms::fogDist.set(128.0f);
		gfxu::Uniforms::MMS.push(geom::Matrix::scale(16.0f, 16.0f, 16.0f));
		gfxu::Uniforms::MMS.mult(geom::Matrix::translate(xCam, yCam, zCam));
		gfxu::Uniforms::setColor(1.0f, 0.0f, 0.0f, 0.25f);
		grid.draw(GL_LINES);

		glDepthFunc(GL_GREATER);
		gfxu::Uniforms::setColor(1.0f, 0.0f, 0.0f, 0.125f);
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