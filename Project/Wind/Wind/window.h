#include <Windows.h>
#include <iostream>

using namespace std;

class GLWindow
{
private:
	bool OK;
	HINSTANCE hInstance;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;

	friend LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void destroyWindow();
public:
	wstring title;
	int width, height;
	const int major_gl_version, minor_gl_version;
	bool active;

	GLWindow(wstring title, int width, int height, HINSTANCE hInstance, const int major_gl_version, const int minor_gl_version);
	~GLWindow();

	void initWindow();
	void swapBuffers();

	bool isOK();

	void messageBox(LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

	static GLWindow* instance;
};

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);