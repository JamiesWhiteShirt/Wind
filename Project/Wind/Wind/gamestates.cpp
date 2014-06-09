#include "graphics.h"
#include "window.h"

using namespace gfxu;
using namespace GameStates;



Camera::Camera()
{

}



GameState::GameState()
	: FOV(90)
{

}

GameState::~GameState()
{
	//clean();
}

std::mutex GameStates::mut;
bool GameStates::newPendingAvailable = false;
GameState* GameStates::processedState;
GameState* GameStates::pendingState;
GameState* GameStates::renderingState;

void GameStates::staticInit()
{
	GameStates::processedState = new GameState();
	GameStates::pendingState = new GameState();
	GameStates::renderingState = new GameState();
}

void GameStates::cleanup()
{
	delete GameStates::renderingState;
	delete GameStates::processedState;
	delete GameStates::pendingState;
}

void swap(GameState* &gs1, GameState* &gs2)
{
	gs1->cam = gs2->cam;
	gs1->FOV = gs2->FOV;
	gs1->devEnabled = gs2->devEnabled;
	GameState* temp = gs2;
	gs2 = gs1;
	gs1 = temp;
}

void GameStates::swapProcessedPending()
{
	mut.lock();
	swap(pendingState, processedState);
	newPendingAvailable = true;
	mut.unlock();
}

void GameStates::swapPendingRendering()
{
	mut.lock();
	if(newPendingAvailable)
	{
		swap(renderingState, pendingState);
		newPendingAvailable = false;
	}
	mut.unlock();
}