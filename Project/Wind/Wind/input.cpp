#include "input.h"

KeyboardAction::KeyboardAction(int button, bool released)
	: button(button), released(released)
{

}

bool Keyboard::keys[256];
List<KeyboardAction, 16> Keyboard::actions;
std::mutex Keyboard::mut;

void Keyboard::setKey(int index, bool value)
{
	mut.lock();
	keys[index] = value;
	mut.unlock();
}

bool Keyboard::getKey(int index)
{
	mut.lock();
	bool value = keys[index];
	mut.unlock();
	return value;
}

MouseAction::MouseAction(int rx, int ry, int button, bool released)
	: rx(rx), ry(ry), button(button), released(released)
{

}

bool Mouse::mbs[3];
List<MouseAction, 16> Mouse::actions;
std::mutex Mouse::mut;

void Mouse::setMB(int index, bool value)
{
	mut.lock();
	mbs[index] = value;
	mut.unlock();
}

bool Mouse::getMB(int index)
{
	mut.lock();
	bool value = mbs[index];
	mut.unlock();
	return value;
}