#include <Windows.h>
#include <iostream>
#include "graphics.h"
#include "window.h"

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

typedef HGLRC (APIENTRY * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef HGLRC (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int interval);

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

GLWindow::GLWindow(wstring title, int width, int height, HINSTANCE hInstance, const int major_gl_version, const int minor_gl_version)
	: title(title), width(width), height(height), hInstance(hInstance), major_gl_version(major_gl_version), minor_gl_version(minor_gl_version)
{

}

GLWindow::~GLWindow()
{
	destroyWindow();
}

void GLWindow::initWindow()
{
	HGLRC hRC_temp;
	GLuint PixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;

	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

	OK = true;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)wndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	//wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;			
	wc.lpszMenuName	= NULL;		
	wc.lpszClassName = L"GLWindow";

	if(!RegisterClass(&wc))
	{
		OK = false;
		MessageBox(NULL, L"Could not register window class.", L"Registration error", MB_OK | MB_ICONERROR);
		return;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);

	hWnd = CreateWindowEx(dwExStyle, L"GLWindow", this->title.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, NULL);
	//hWnd = CreateWindow(L"OpenGL", title, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, NULL);

	if(!hWnd)
	{
		OK = false;
		destroyWindow();
		return;
	}

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16, 
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if(!(hDC = GetDC(hWnd)))
	{
		OK = false;
		destroyWindow();
		return;
	}

	if(!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		OK = false;
		destroyWindow();
		return;
	}

	if(!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		OK = false;
		destroyWindow();
		return;
	}


	if(!(hRC_temp = wglCreateContext(hDC)))
	{
		OK = false;
		destroyWindow();
		return;
	}

	if(!wglMakeCurrent(hDC, hRC_temp))
	{
		OK = false;
		destroyWindow();
		return;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	if(glewInit() != GLEW_OK)
	{
		OK = false;
		destroyWindow();
		return;
	}

	if((major_gl_version > 1) | (minor_gl_version > 2))
	{
		return;
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major_gl_version,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor_gl_version,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress((LPCSTR)"wglCreateContextAttribsARB");
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress((LPCSTR)"wglSwapIntervalEXT");

	wglSwapIntervalEXT(0);

	if(!(hRC = wglCreateContextAttribsARB(hDC, 0, attribs)))
	{
		OK = false;
		destroyWindow();
		return;
	}

	if(!wglMakeCurrent(NULL, NULL) || !wglDeleteContext(hRC_temp) || !wglMakeCurrent(hDC, hRC))
	{
		OK = false;
		destroyWindow();
		return;
	}
}

void GLWindow::swapBuffers()
{
	SwapBuffers(hDC);
}

bool GLWindow::isOK()
{
	return OK;
}

void GLWindow::messageBox(LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	MessageBox(hWnd, lpText, lpText, uType);
}

void GLWindow::destroyWindow()
{
	bool error = false;

	glBindVertexArray(0);

	if(hRC)
	{
		if(!wglMakeCurrent(NULL, NULL))
		{
			error = true;
		}

		if(!wglDeleteContext(hRC))
		{
			error = true;
		}
	}
	if(hDC && !ReleaseDC(hWnd, hDC))
	{
		hDC = NULL;
		error = true;
	}
	if(hWnd && !DestroyWindow(hWnd))
	{
		hWnd = NULL;
		error = true;
	}
	if(!UnregisterClass(L"GLWindow", hInstance))
	{
		hInstance = NULL;
		error = true;
	}
	if(error)
	{
		MessageBox(NULL, L"There was an error while destroying the window", L"ERROR", MB_OK | MB_ICONINFORMATION);
	}
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_ACTIVATE:
		{
			if(!HIWORD(wParam))
			{
				GLWindow::instance->active = true;
			} else
			{
				GLWindow::instance->active = false;
			}

			break;
		}
	case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
				case SC_SCREENSAVE:
				{

				}
				
				case SC_MONITORPOWER:
				{
					return 0;
				}
			}

			break;
		}
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_KEYDOWN:
		{
			//keys[wParam] = true;
			break;
		}
	case WM_KEYUP:
		{
			//keys[wParam] = false;
			break;
		}
	case WM_SIZE:
		{
			GLWindow::instance->width = LOWORD(lParam);
			GLWindow::instance->height = HIWORD(lParam);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			//leftMouseDown = true;
			break;
		}
	case WM_LBUTTONUP:
		{
			//leftMouseDown = false;
			break;
		}
	case WM_RBUTTONDOWN:
		{
			//rightMouseDown = true;
			break;
		}
	case WM_RBUTTONUP:
		{
			//rightMouseDown = false;
			break;
		}
	case WM_INPUT:
		{
			/*UINT dwSize = 40;
			static byte lpb[40];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if(raw->header.dwType == RIM_TYPEMOUSE)
			{
				int xPosRelative = raw->data.mouse.lLastX;
				int yPosRelative = raw->data.mouse.lLastY;

				if(rightMouseDown)
				{
					//camRotZ += xPosRelative * 0.25f;
					camFOV += yPosRelative * 0.25f;

					if(camFOV < 1.0)
					{
						camFOV = 1.0;
					} else
					if(camFOV > 135.0)
					{
						camFOV = 135.0;
					}

					//Reperspective();
				} else
				if(leftMouseDown)
				{
					float val1 = yPosRelative * 0.5f;
					float val2 = xPosRelative * 0.5f;
					float val3 = camRotZ * M_PI / 180;
					float val4 = camFOV / 45.0f;

					camRotX += (val1 * cos(val3) + val2 * sin(val3) * 0.25f) * val4;
					camRotY += (val1 * -sin(val3) + val2 * cos(val3) * (int(abs(camRotX) + 90.0) % 360 < 180 ? 1.0 : -1.0) * 0.5f) * val4;

					//camRotX += yPosRelative * cos(camRotZ * M_PI / 180) * 0.5f;
					//camRotY += xPosRelative * cos(camRotZ * M_PI / 180) * 0.5f * (int(abs(camRotX) + 90.0) % 360 < 180 ? 1.0 : -1.0);
				}
			}
			
			break;*/
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

GLWindow* GLWindow::instance = NULL;