#include "Window.h"
#include <assert.h>
#include <cstring>

Window::Window(int width, int height, HINSTANCE hInstance)
	:
	hInst(hInstance),
	WindowWidth(width),
	WindowHeight(height)

{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = StartMessage;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;

	RegisterClassExW(&wc);

	hwnd =
		CreateWindowExW(0, pClassName, L"WinFramework", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
			WindowWidth, WindowHeight, nullptr, nullptr, hInstance, this);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	ResizeDIB(WindowWidth, WindowHeight);
}

Window::~Window() //Window calss construct
{
	UnregisterClassW(pClassName, hInst);
	delete[] Colors;
	Colors = nullptr;
}

bool Window::RunMessageLoop()
{
	MSG msg;
	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return true;
}

void Window::CleanScreenSuperFast()
{
	memset(Colors, 0, WindowWidth * WindowHeight * 4);
}

void Window::ResizeDIB(int BitmapWidth, int BitmapHeight)
{
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = BitmapWidth;
	bmInfo.bmiHeader.biHeight = -BitmapHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	delete[] Colors;
	Colors = new int[BitmapWidth * BitmapHeight];

	CleanScreenSuperFast();

}

void Window::UpdateWindowBuffer(HDC hdc, int BitmapWidth, int BitmapHeight)
{
	StretchDIBits(
		hdc,
		0,
		0,
		BitmapWidth,
		BitmapHeight,
		0,
		0,
		BitmapWidth,
		BitmapHeight,
		Colors,
		&bmInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

int* Window::GetColorBuffer()
{
	return Colors;
}


LRESULT Window::StartMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = reinterpret_cast<Window*>(pCreate->lpCreateParams);

		assert(pWnd != nullptr);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::MessageSetup));
		return pWnd->MessageSetup(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::MessageSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return pWnd->HandleMessage(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KILLFOCUS:
	{
		kbd.ClearState();
	}break;
	case WM_KEYDOWN:
	{
		const bool wasDown = (lParam & 0x40000000) != 0;
		if (!wasDown || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
	}break;
	case WM_KEYUP:
	{
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
	}break;
	case WM_CHAR:
	{
		kbd.OnChar(static_cast<char>(wParam));
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0); //refuse all resources
	}break;

	case WM_QUIT:
	{
		PostQuitMessage(0);
	}break;
	case WM_SIZE:
	{
		ResizeDIB(WindowWidth, WindowHeight);
	}break;

	case WM_PAINT:
	{
		PAINTSTRUCT pPaintStruct;
		HDC hdc = BeginPaint(hwnd, &pPaintStruct);

		int WindowWidth = pPaintStruct.rcPaint.right - pPaintStruct.rcPaint.left;
		int WindowHeight = pPaintStruct.rcPaint.bottom - pPaintStruct.rcPaint.top;

		UpdateWindowBuffer(hdc, WindowWidth, WindowHeight);
		EndPaint(hwnd, &pPaintStruct);
	}break;
	case WM_MOUSEMOVE:
	{
		POINTS pt = MAKEPOINTS(lParam);
		mouse.SetPos(pt.x, pt.y);
	}break;

	}
	return DefWindowProcA(hwnd, msg, wParam, lParam);
}