#include "graphics.h"
#include "window.h"

using namespace gfxu;
using namespace GameStates;
using namespace RenderActions;



RAVertexStreamDraw::RAVertexStreamDraw(VertexStream* vStream)
	: vStream(vStream)
{

}
bool RAVertexStreamDraw::invoke()
{
	vStream->draw();
	return true;
}

RAMatrixSimpleOp::RAMatrixSimpleOp(int m)
	: m(m)
{

}

RAMatrixPop::RAMatrixPop(int m)
	: RAMatrixSimpleOp(m)
{

}
bool RAMatrixPop::invoke()
{
	if(m == MODELVIEW_MATRIX)
	{
		MatrixManager::popM();
	}
	else if(m == PROJECTION_MATRIX)
	{
		MatrixManager::popP();
	}

	return true;
}

RAMatrixUniform::RAMatrixUniform(int m)
	: RAMatrixSimpleOp(m)
{

}
bool RAMatrixUniform::invoke()
{
	if(m == MODELVIEW_MATRIX)
	{
		MatrixManager::popM();
	}
	else if(m == PROJECTION_MATRIX)
	{
		MatrixManager::popP();
	}

	return true;
}

RAMatrixPerspective::RAMatrixPerspective(int m)
	: RAMatrixSimpleOp(m)
{

}
bool RAMatrixPerspective::invoke()
{
	if(m == MODELVIEW_MATRIX)
	{
		MatrixManager::multM(Matrix::perspective(90.0f, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.01f, 100.0f));
	}
	else if(m == PROJECTION_MATRIX)
	{
		MatrixManager::multP(Matrix::perspective(90.0f, (float)GLWindow::instance->width / (float)GLWindow::instance->height, 0.01f, 100.0f));
	}

	return true;
}

RAMatrixOp::RAMatrixOp(int m, Matrix mat)
	: RAMatrixSimpleOp(m), mat(mat)
{

}

RAMatrixPush::RAMatrixPush(int m, Matrix mat)
	: RAMatrixOp(m, mat)
{

}
bool RAMatrixPush::invoke()
{
	if(m == MODELVIEW_MATRIX)
	{
		MatrixManager::pushM(mat);
	}
	else if(m == PROJECTION_MATRIX)
	{
		MatrixManager::pushP(mat);
	}

	return true;
}

RAMatrixMult::RAMatrixMult(int m, Matrix mat)
	: RAMatrixOp(m, mat)
{

}
bool RAMatrixMult::invoke()
{
	if(m == MODELVIEW_MATRIX)
	{
		MatrixManager::multM(mat);
	}
	else if(m == PROJECTION_MATRIX)
	{
		MatrixManager::multP(mat);
	}

	return true;
}



Camera::Camera()
{

}



GameState::GameState()
	: heap(MemUtil::MiniHeap(0x10000000)), actionsSize(0), renderActions(new RA*[2048])
{

}

GameState::~GameState()
{
	delete[] renderActions;

	//clean();
}

bool GameState::isEmpty()
{
	return actionsSize == 0;
}

bool GameState::render()
{
	for(unsigned int i = 0; i < actionsSize; i++)
	{
		RA* ra = renderActions[i];
		if(ra == nullptr || !renderActions[i]->invoke())
		{
			return false;
		}

		if(gfxu::getError())
		{
			return false;
		}
	}

	return true;
}

void GameState::clean()
{
	for(unsigned int i = 0; i < actionsSize; i++)
	{
		renderActions[i]->~RA();
	}

	heap.clear();
	actionsSize = 0;
}

std::mutex GameStates::mut;
bool GameStates::newPendingAvailable = false;
GameState* GameStates::processedState;
GameState* GameStates::pendingState;
GameState* GameStates::renderingState;

void GameStates::swapProcessedPending()
{
	mut.lock();
	
	pendingState->cam = processedState->cam;
	GameState* temp = GameStates::processedState;
	GameStates::processedState = GameStates::pendingState;
	GameStates::pendingState = temp;

	newPendingAvailable = true;
	mut.unlock();
}

void GameStates::swapPendingRendering()
{
	mut.lock();
	if(newPendingAvailable)
	{
		renderingState->cam = pendingState->cam;
		GameState* temp = GameStates::pendingState;
		GameStates::pendingState = GameStates::renderingState;
		GameStates::renderingState = temp;

		newPendingAvailable = false;
	}
	mut.unlock();
}