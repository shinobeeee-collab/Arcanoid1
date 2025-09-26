#pragma once
#include "Winlib.h"
#include "Mouse.h"
#include "Keyboard.h"


class Window
{
public:
	Window(int width, int height, HINSTANCE hInstance);
	Window(const Window&) = delete;
	Window operator =(const Window&) = delete;
	~Window();
	bool RunMessageLoop();

private:
	static LRESULT CALLBACK StartMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MessageSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void ResizeDIB(int BitmapWidth, int BitmapHeight);
	void CleanScreenSuperFast();
public:
	void UpdateWindowBuffer(HDC hdc, int BitmapWidth, int BitmapHeight);
	int* GetColorBuffer();
	HWND GetCustomWindow() const { return hwnd; }

private:

	HINSTANCE hInst;
	HWND hwnd = nullptr;
	HDC hdc = nullptr;
	BITMAPINFO bmInfo = {};
	int* Colors = nullptr;
	int WindowWidth;
	int WindowHeight;
	static constexpr wchar_t* pClassName = L"WinFramework";
public:
	Keyboard kbd;
	Mouse mouse;
};