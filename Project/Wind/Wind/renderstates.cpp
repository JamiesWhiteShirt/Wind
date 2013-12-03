#include "graphics.h"
#include "window.h"

using namespace gfxu;
using namespace RenderStates;
using namespace RenderActions;



RAVertexStreamDraw::RAVertexStreamDraw(VertexStream &vStream)
	: vStream(&vStream)
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



RenderState::RenderState()
	: heap(MemUtil::MiniHeap(0x10000)), actionsSize(0), renderActions(new RA*[2048])
{

}

RenderState::~RenderState()
{
	delete[] renderActions;

	//clean();
}

bool RenderState::isEmpty()
{
	return actionsSize == 0;
}

bool RenderState::render()
{
	for(unsigned int i = 0; i < actionsSize; i++)
	{
		RA* ra = renderActions[i];
		if(!renderActions[i]->invoke())
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

void RenderState::clean()
{
	for(unsigned int i = 0; i < actionsSize; i++)
	{
		renderActions[i]->~RA();
	}

	heap.clear();
	actionsSize = 0;
}

std::mutex RenderStates::mut;
bool RenderStates::newPendingAvailable = false;
RenderState* RenderStates::processedState;
RenderState* RenderStates::pendingState;
RenderState* RenderStates::renderingState;

void RenderStates::swapProcessedPending()
{
	mut.lock();
	
	pendingState->cam = processedState->cam;
	RenderState* temp = RenderStates::processedState;
	RenderStates::processedState = RenderStates::pendingState;
	RenderStates::pendingState = temp;

	newPendingAvailable = true;
	mut.unlock();
}

void RenderStates::swapPendingRendering()
{
	mut.lock();
	if(newPendingAvailable)
	{
		renderingState->cam = pendingState->cam;
		RenderState* temp = RenderStates::pendingState;
		RenderStates::pendingState = RenderStates::renderingState;
		RenderStates::renderingState = temp;

		newPendingAvailable = false;
	}
	mut.unlock();
}