//#define NOMINMAX
//#include<Windows.h>
#pragma once
#include<SDL3/SDL.h>
#include"pch.h"

class CatWindow_Child
{


private:
	class WindowClass
	{
	public:
		static const WCHAR* GetName()noexcept;
		static HINSTANCE GetInstance()noexcept;
	private:
		WindowClass()noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const WCHAR* wndClassName =L"CSPMOD DX Child Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};



public:
	CatWindow_Child();
	bool Create(const SDL_Rect& rect, HWND parent,bool show=true)noexcept;
	HWND GetHWND();

	typedef unsigned int CatWindow_ChildStyle;
	enum _CatWindow_ChildStyle
	{
		Default=0,
		Normal=1,
		Transparent =1<<1,
		Lock =1<<2,
	};
	
	
	void SetWindowTitle(const std::wstring& name);


	void SetStyle(CatWindow_ChildStyle _style);
	inline long CatSetWindowLong(HWND hwnd, int nIndex, long dwNewLong);
	CatWindow_ChildStyle GetStyle();
	void SetIsTopWindow(bool flag);
	bool GetIsTopWindow();

	//统一采用客户区左上角坐标
	void GetWindowPosition(int* x, int* y);
	void SetWindowPosition(int x, int y);
	void SetWindowSize(int x, int y);
	void SetWindowPosAndSize(int x, int y,int w,int h);
	void SetAspectRatio(int x, int y);
	void GetWindowSize(int* x, int* y);
	void GetWindowAspectRatio(int* x, int* y);
	void ScaleWindow(int pixel);


	void SetTaskbarIconVisibility(bool b_show);
	void SetVisiability(bool b_show);


public:
	//在收到关闭窗口时调用这个函数,如果返回真，则关闭窗口
	virtual bool OnCloseWindow() { return true; };

	virtual void OnResizeWindow(int w,int h) {};
	virtual void OnLoseFocus();

	virtual ~CatWindow_Child();
	CatWindow_Child(const CatWindow_Child&) = delete;
	CatWindow_Child& operator=(const CatWindow_Child&) = delete;


protected:
	virtual LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept;

private:
	//这两个消息函数被用来传递CatWindow_Child类自身指针
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lpParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lpParam);
	void OnRightClick(int px, int py);//右键打开菜单
	bool OnClickMenuItem(ui::EventArgs* args);


private:
	//用于固定窗口长宽比
	int aspectRatio_width;
	int aspectRatio_height;

	//用于存储最小化前的长宽
	int lastsizeW;
	int lastsizeH;
	int lastPosX;
	int lastPosY;
	HWND hWnd;
	CatWindow_ChildStyle style;
	bool isTopWindow;
	bool isTaskbarIconVisible;
};















//error exception helper macro
//#define CHWND_EXCEPT(hr) CatWindow_Child::Exception(__LINE__,__FILE__,hr);
//#define CHWND_LAST_EXCEPT(hr) CatWindow_Child::Exception(__LINE__,__FILE__,GetLastError());