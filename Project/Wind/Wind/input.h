#include <mutex>
#include "list.h"

class KeyboardAction
{
public:
	int button;
	bool released;
	KeyboardAction(int button = -1, bool released = false);
};

class Keyboard
{
private:
	static bool keys[256];
public:
	static List<KeyboardAction, 16> actions;
	static std::mutex mut;
	static void setKey(int index, bool value);
	static bool getKey(int index);
};

class MouseAction
{
public:
	int rx, ry, sc;
	int button;
	bool released;
	MouseAction(int rx = 0, int ry = 0, int sc = 0, int button = -1, bool release = false);
};

class Mouse
{
private:
	static bool mbs[3];
public:
	static List<MouseAction, 16> actions;
	static std::mutex mut;
	static void setMB(int index, bool value);
	static bool getMB(int index);
};