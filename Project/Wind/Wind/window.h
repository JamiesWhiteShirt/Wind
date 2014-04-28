#include <Windows.h>
#include <iostream>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

using namespace std;

class GLWindow
{
private:
	bool OK;
	HINSTANCE hInstance;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	HGLRC hRC_temp;

	friend LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	wstring title;
	bool rescaled;
	int width, height;
	const int major_gl_version, minor_gl_version;
	bool active;

	GLWindow(wstring title, int width, int height, HINSTANCE hInstance, const int major_gl_version, const int minor_gl_version);
	~GLWindow();

	void initWindow();
	void initGL();
	void swapBuffers();
	void makeCurrent();

	void destroyGL();
	void destroyWindow();

	bool isOK();
	HWND getHWnd();

	void messageBox(LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
	void postError(const char* text, const char* caption);

	static GLWindow* instance;
};

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);