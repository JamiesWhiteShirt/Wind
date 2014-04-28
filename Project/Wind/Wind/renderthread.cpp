#include "threads.h"
#include "window.h"
#include "ioutil.h"
#include <sstream>
#include "opencl.h"

using namespace gfxu;

bool RenderThread::skipRender = true;

cl::CommandQueue commandQueue;
cl::Program program;
cl::Buffer projectionMatrixBuffer;
cl::Buffer boolBuffer;
cl::Buffer gridBuffer;

gfxu::VertexStream* grid;
gfxu::VertexStream* square;
gfxu::VertexShader* normalVShader;
gfxu::FragmentShader* normalFShader;
gfxu::ShaderProgram* normalShaderProgram;
gfxu::VertexShader* noTexVShader;
gfxu::FragmentShader* noTexFShader;
gfxu::ShaderProgram* noTexShaderProgram;
gfxu::Texture2D* testure;

const int renderDistance = 8;
bool bGrid[renderDistance * 2 + 1][renderDistance * 2 + 1][renderDistance * 2 + 1];

bool RenderThread::tick()
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
		return true;
	}

	GameStates::swapPendingRendering();

	GameStates::GameState* state = GameStates::renderingState;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(GLWindow::instance->rescaled)
	{
		glViewport(0, 0, GLWindow::instance->width, GLWindow::instance->height);
		GLWindow::instance->rescaled = false;
	}
		
	normalShaderProgram->bind();
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
		
	testure->bind();

	float cScale = 1.0f + 90.0f / state->FOV;
	//float cScale = 1.0f;
	//geom::AxisAlignedCube cube(geom::Vector(-cScale, -cScale, -cScale), geom::Vector(2.0f, 2.0f, 2.0f) * cScale);
	int xCam = floorf(state->cam.pos.x / 16.0f);
	int yCam = floorf(state->cam.pos.y / 16.0f);
	int zCam = floorf(state->cam.pos.z / 16.0f);

	geom::Matrix projectionMatrix = gfxu::Uniforms::PMS.getTopmost();
	//geom::Matrix projectionMatrix = geom::Matrix::perspective(state->FOV, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.1f, 16.0f * renderDistance);// * geom::Matrix::rotate(state->cam.rot.x, 1.0f, 0.0f, 0.0f) * geom::Matrix::rotate(state->cam.rot.y, 0.0f, 1.0f, 0.0f) * geom::Matrix::rotate(state->cam.rot.z, 0.0f, 0.0f, 1.0f);
	projectionMatrixBuffer.write(commandQueue, projectionMatrix.data);

	const size_t global_ws_1[] = {renderDistance * 2 + 2, renderDistance * 2 + 2, renderDistance * 2 + 2};
	const size_t local_ws_1[] = {1, 1, 1};

	if(!program.prepare("gridTransform")) GlobalThread::stop = true;
	if(!program.setArgument(0, sizeof(const unsigned int), &renderDistance)) GlobalThread::stop = true;
	if(!program.setArgument(1, sizeof(cl_mem), &projectionMatrixBuffer)) GlobalThread::stop = true;
	if(!program.setArgument(2, sizeof(const int), &xCam)) GlobalThread::stop = true;
	if(!program.setArgument(3, sizeof(const int), &yCam)) GlobalThread::stop = true;
	if(!program.setArgument(4, sizeof(const int), &zCam)) GlobalThread::stop = true;
	if(!program.setArgument(5, sizeof(cl_mem), &boolBuffer)) GlobalThread::stop = true;
	if(!program.invoke(commandQueue, 3, global_ws_1, local_ws_1)) GlobalThread::stop = true;

	const size_t global_ws_2[] = {renderDistance * 2 + 1, renderDistance * 2 + 1, renderDistance * 2 + 1};
	const size_t local_ws_2[] = {1, 1, 1};

	if(!program.prepare("arrayInsideCheck")) GlobalThread::stop = true;
	if(!program.setArgument(0, sizeof(const unsigned int), &renderDistance)) GlobalThread::stop = true;
	if(!program.setArgument(1, sizeof(cl_mem), &gridBuffer)) GlobalThread::stop = true;
	if(!program.setArgument(2, sizeof(cl_mem), &boolBuffer)) GlobalThread::stop = true;
	if(!program.invoke(commandQueue, 3, global_ws_2, local_ws_2)) GlobalThread::stop = true;

	if(!gridBuffer.read(commandQueue, bGrid)) GlobalThread::stop = true;

	getError();

	/*for(int i = 0; i <= renderDistance * 2; i++)
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
	}*/

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

	std::vector<shared_ptr<ChunkBase>> chunksToRender;
	for(map<ChunkPosition, std::shared_ptr<ChunkBase>>::const_iterator iter = GlobalThread::world.chunkMap.begin(); iter != GlobalThread::world.chunkMap.end(); ++iter)
	{
		shared_ptr<ChunkBase> chunk = iter->second;
		if(!chunk->isEmpty() && chunk->isLoaded())
		{
			if(chunk->pos.x - renderDistance <= xCam && chunk->pos.x + renderDistance >= xCam && chunk->pos.y - renderDistance <= yCam && chunk->pos.y + renderDistance >= yCam && chunk->pos.z - renderDistance <= zCam && chunk->pos.z + renderDistance > zCam)
			{
				if(bGrid[chunk->pos.x - xCam + renderDistance][chunk->pos.y - yCam + renderDistance][chunk->pos.z - zCam + renderDistance])
				{
					chunksToRender.push_back(chunk);
				}
			}
		}
	}
	GlobalThread::world.chunkMapLock.unlock();

	for(int i = 0; i < chunksToRender.size(); i++)
	{
		std::shared_ptr<ChunkBase> chunk = chunksToRender[i];

		if(chunk->firstPass != nullptr)
		{
			gfxu::Uniforms::MMS.push(geom::Matrix::translate(chunk->pos.x * 16, chunk->pos.y * 16, chunk->pos.z * 16));
			chunk->firstPass->draw();
			gfxu::Uniforms::MMS.pop();
		}
	}

	glEnable(GL_BLEND);
	for(int i = 0; i < chunksToRender.size(); i++)
	{
		std::shared_ptr<ChunkBase> chunk = chunksToRender[i];

		if(chunk->secondPass != nullptr)
		{
			gfxu::Uniforms::MMS.push(geom::Matrix::translate(chunk->pos.x * 16, chunk->pos.y * 16, chunk->pos.z * 16));
			chunk->secondPass->draw();
			gfxu::Uniforms::MMS.pop();
		}
	}

	noTexShaderProgram->bind();
	/*gfxu::Uniforms::MMS.push(geom::Matrix::translate(state->cam.pos.x - renderDistance * 16.0f, -0.125f, state->cam.pos.z - renderDistance * 16.0f));
	gfxu::Uniforms::MMS.mult(geom::Matrix::scale(32.0f * renderDistance, 1.0f, 32.0f * renderDistance));
	gfxu::Uniforms::setColor(0.1f, 0.2f, 0.5f, 0.75f);
	square->draw();
	gfxu::Uniforms::MMS.pop();*/
	
	if(state->devEnabled)
	{
		gfxu::Uniforms::setFogColor(1.0f, 0.0f, 0.0f, 0.0f);
		gfxu::Uniforms::fogDist.set(128.0f);
		gfxu::Uniforms::MMS.push(geom::Matrix::scale(16.0f, 16.0f, 16.0f));
		gfxu::Uniforms::MMS.mult(geom::Matrix::translate(xCam, yCam, zCam));
		gfxu::Uniforms::setColor(1.0f, 0.0f, 0.0f, 0.25f);
		grid->draw(GL_LINES);

		glDepthFunc(GL_GREATER);
		gfxu::Uniforms::setColor(1.0f, 0.0f, 0.0f, 0.125f);
		grid->draw(GL_LINES);
		gfxu::Uniforms::MMS.pop();
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
	}

	glFlush();
	if(gfxu::getError("Graphics thread loop error")) GlobalThread::stop = true;
	GLWindow::instance->swapBuffers();

	return true;
}

void RenderThread::preStart()
{
	const size_t gridSize = sizeof(bool) * (renderDistance * 2 + 1) * (renderDistance * 2 + 1) * (renderDistance * 2 + 1);

	commandQueue.create();
	projectionMatrixBuffer.create(sizeof(float) * 16, CL_MEM_READ_ONLY);
	gridBuffer.create(gridSize, CL_MEM_WRITE_ONLY);
	boolBuffer.create(sizeof(unsigned char) * (renderDistance * 2 + 2) * (renderDistance * 2 + 2) * (renderDistance * 2 + 2), CL_MEM_READ_WRITE);

	std::wstring filePath(IOUtil::EXE_DIR);
	filePath += L"\\programs\\frustumclip.cl";
	program.create(filePath);

	grid = new VertexStream();
	for(int i = -8; i <= 8; i++)
	{
		for(int j = -8; j <= 8; j++)
		{
			grid->put(Vertex(i, j, -8));
			grid->put(Vertex(i, j, 8));
			grid->put(Vertex(i, -8, j));
			grid->put(Vertex(i, 8, j));
			grid->put(Vertex(-8, i, j));
			grid->put(Vertex(8, i, j));
		}
	}

	square = new VertexStream();
	square->put(Vertex(0.0f, 0.0f, 0.0f));
	square->put(Vertex(1.0f, 0.0f, 0.0f));
	square->put(Vertex(1.0f, 0.0f, 1.0f));
	square->put(Vertex(1.0f, 0.0f, 1.0f));
	square->put(Vertex(0.0f, 0.0f, 1.0f));
	square->put(Vertex(0.0f, 0.0f, 0.0f));

	GLWindow::instance->initGL();
	
	if(!GLWindow::instance->isOK())
	{
		return;
	}

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\normal.vert";
	normalVShader = new gfxu::VertexShader(filePath);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\normal.frag";
	normalFShader = new gfxu::FragmentShader(filePath);

	normalShaderProgram = new gfxu::ShaderProgram(normalVShader, nullptr, normalFShader);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\noTex.vert";
	noTexVShader = new gfxu::VertexShader(filePath);

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\shaders\\noTex.frag";
	noTexFShader = new gfxu::FragmentShader(filePath);

	noTexShaderProgram = new gfxu::ShaderProgram(noTexVShader, nullptr, noTexFShader);

	gfxu::Uniforms::reset();

	filePath = IOUtil::EXE_DIR;
	filePath += L"\\textures\\test.png";
	testure = new gfxu::Texture2D(filePath, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false);

	glClearColor(0.5f, 0.875f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderThread::postStop()
{
	delete grid;
	delete square;
	delete normalVShader;
	delete normalFShader;
	delete normalShaderProgram;
	delete noTexVShader;
	delete noTexFShader;
	delete noTexShaderProgram;
	delete testure;

	GLWindow::instance->destroyGL();
}