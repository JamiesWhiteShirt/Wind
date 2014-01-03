#include "graphics.h"
#include "window.h"

using namespace gfxu;
using namespace GameStates;



Camera::Camera()
{

}



GameState::GameState()
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