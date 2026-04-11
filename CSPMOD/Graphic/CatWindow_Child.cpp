//#include"Dui/DuiCommon.h"

#include "CatWindow_Child.h"
#include"resource.h"
#include<iostream>


#define CAT_WS_NONE WS_POPUP| WS_VISIBLE
#define CAT_WS_NORMALWINDOW WS_CAPTION|WS_THICKFRAME| WS_MINIMIZEBOX |WS_SYSMENU| WS_VISIBLE
//#define CAT_WS_EX_NONE WS_EX_NOACTIVATE
#define CAT_WS_EX_NONE WS_EX_NOACTIVATE
//#define CAT_WS_EX_NORMALWINDOW 0
#define CAT_WS_EX_NORMALWINDOW WS_EX_NOREDIRECTIONBITMAP

#pragma region CatWindow_Child WindowClass



CatWindow_Child::WindowClass CatWindow_Child::WindowClass::wndClass;

CatWindow_Child::WindowClass::WindowClass()noexcept
	:hInst(GetModuleHandle(NULL))
{

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon =NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName=GetName();
	wc.hIconSm = NULL;
	auto result=RegisterClassEx(&wc);
	return;
}

CatWindow_Child::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const WCHAR* CatWindow_Child::WindowClass::GetName()noexcept
{
	return wndClassName;
}

HINSTANCE CatWindow_Child::WindowClass::GetInstance()noexcept
{
	return wndClass.hInst;
}
#pragma endregion
CatWindow_Child::CatWindow_Child()
	:aspectRatio_width(0), aspectRatio_height(0), hWnd(NULL),style(_CatWindow_ChildStyle::Default), isTaskbarIconVisible(true)
{

}

bool CatWindow_Child::Create(const SDL_Rect& rect, HWND parent, bool show)noexcept
{
	//DWORD dwstyle = CAT_WS_NONE;
	//DWORD dwexstyle = CAT_WS_EX_NONE;
	//DWORD dwstyle =  WS_VISIBLE|WS_CHILDWINDOW &(~WS_POPUP);
	//DWORD dwexstyle = CAT_WS_EX_NONE;
	DWORD dwstyle = WS_CHILD | WS_CLIPSIBLINGS;
	//DWORD dwexstyle = CAT_WS_EX_NONE;
	DWORD dwexstyle = 0;

	//parent = NULL;
	if (hWnd)
	{
		if(show)
			ShowWindow(hWnd, SW_SHOWDEFAULT);
		return true;
		//已经有窗口的情况下不进行任何操作
	}


	RECT wr;
	wr.left = rect.x;//todo::在屏幕中心创建窗口
	wr.right = rect.w + wr.left;
	wr.top = rect.y;
	wr.bottom = rect .h + wr.top;
	AdjustWindowRectEx( &wr,dwstyle,false,dwexstyle);
	hWnd = CreateWindowEx(
		dwexstyle, WindowClass::GetName(), L"Cat Child Window",
		show?dwstyle:(dwstyle&~WS_VISIBLE)
		, wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top,
		parent,NULL,WindowClass::GetInstance(),this
	);

	if (hWnd)
	{
		this->aspectRatio_width = rect.w;
		this->aspectRatio_height = rect.h;
		this->lastsizeW = rect.w;
		this->lastsizeH = rect.h;
		if (show)
			ShowWindow(hWnd, SW_SHOWDEFAULT);
		//else
		//	ShowWindow(hWnd, SW_HIDE);
		return true;
	}
	else
	{	
		HRESULT hr = GetLastError();
		
		return false;
	}


}

HWND CatWindow_Child::GetHWND()
{
	return hWnd;
}

inline long CatWindow_Child::CatSetWindowLong(HWND hwnd,int nIndex,long dwNewLong)
{
	long result,style,exstyle;
	int px, py,w,h;
	GetWindowPosition(&px, &py);
	GetWindowSize(&w,&h);


	result=SetWindowLong(hwnd, nIndex, dwNewLong);


	exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	style= GetWindowLong(hwnd, GWL_STYLE);
	


	//RECT rect,rect2;
	//rect.left = 100;
	//rect.right = 100;
	//rect.top = 100;
	//rect.bottom = 100;
	//rect2 = rect;
	//AdjustWindowRectEx(&rect2, style,false,exstyle);
	//POINT deltaLT, deltaRB;
	//deltaLT.x = rect2.left - rect.left;
	//deltaLT.y = rect2.top - rect.top;
	//deltaRB.x = rect2.right - rect.right;
	//deltaRB.y = rect2.bottom - rect.bottom;

	RECT rect;
	rect.left = px;
	rect.right = px+w;
	rect.top = py;
	rect.bottom = py+h;
	AdjustWindowRectEx(&rect, style, false, exstyle);
	MoveWindow(hwnd,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,true);
	return result;
}




void CatWindow_Child::SetWindowTitle(const std::wstring& name)
{
	::SetWindowText(hWnd, name.c_str());
}

void CatWindow_Child::SetStyle(CatWindow_ChildStyle _style)
{
	if (style == _style)
		return;
	style = _style;
	if (_style == _CatWindow_ChildStyle::Default)
	{



		CatSetWindowLong(hWnd, GWL_STYLE, CAT_WS_NORMALWINDOW);
		CatSetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
		//DWM_BLURBEHIND bb = { 0 };
		////HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
		//bb.dwFlags = DWM_BB_ENABLE; //| DWM_BB_BLURREGION;
		////bb.hRgnBlur = hRgn;
		//bb.fEnable = FALSE;
		//DwmEnableBlurBehindWindow(hWnd, &bb);
		return;
	}

	if (style & Normal && style & Transparent)
	{//Normal和Transparent同时出现时设置为Transaprent
		style = style & (~Normal);
	}
	if (style & _CatWindow_ChildStyle::Normal)
	{

		CatSetWindowLong(hWnd,GWL_STYLE, CAT_WS_NORMALWINDOW);
	}
	if (style & _CatWindow_ChildStyle::Transparent)
	{

		bool ischild = (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD);



		CatSetWindowLong(hWnd, GWL_STYLE, CAT_WS_NONE );



		//DWM_BLURBEHIND bb = { 0 };
		//HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
		//bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
		//bb.hRgnBlur = hRgn;
		//bb.fEnable = TRUE;
		//DwmEnableBlurBehindWindow(hWnd, &bb);
	}
	else
	{






		CatSetWindowLong(hWnd, GWL_STYLE, CAT_WS_NORMALWINDOW);


		//DWM_BLURBEHIND bb = { 0 };
		////HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
		//bb.dwFlags = DWM_BB_ENABLE; //| DWM_BB_BLURREGION;
		////bb.hRgnBlur = hRgn;
		//bb.fEnable = FALSE;
		//DwmEnableBlurBehindWindow(hWnd, &bb);
	}



	if (style & _CatWindow_ChildStyle::Lock)
	{
		CatSetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) |WS_EX_LAYERED| WS_EX_TRANSPARENT);
	}
	else
	{
		CatSetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
	}


}



CatWindow_Child::CatWindow_ChildStyle CatWindow_Child::GetStyle()
{
	return style;
}

void CatWindow_Child::SetIsTopWindow(bool flag)
{
	isTopWindow = flag;
	if (flag)
	{
		SetWindowPos(hWnd, HWND_TOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(hWnd, HWND_NOTOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE);
	}
}

bool CatWindow_Child::GetIsTopWindow()
{
	return isTopWindow;
}

void CatWindow_Child::GetWindowPosition(int* x, int* y)
{
	POINT lefttop;
	lefttop.x = 0;
	lefttop.y =0;
	if (ClientToScreen(hWnd, &lefttop))
	{
		if (lefttop.x < -20000 || lefttop.y < -20000)
		{
			*x = lastPosX;
			*y = lastPosY;
		}
		else
		{
			*x = lefttop.x;
			*y = lefttop.y;
		}



		printf("getwindowposition:%d,%d\n", lefttop.x, lefttop.y);
	}
	else
	{//TODO::设置一个无效值
		*x = 0;
		*y = 0;
	}


}

void CatWindow_Child::SetWindowPosition(int x, int y)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	POINT delta;
	delta.x = rect.left;
	delta.y = rect.top;
	if (ScreenToClient(hWnd,&delta))
	{
		POINT lefttop;
		lefttop.x = x + delta.x;
		lefttop.y = y + delta.y;

		MoveWindow(hWnd, lefttop.x,lefttop.y,rect.right-rect.left,rect.bottom-rect.top, true);



		printf("setwindowposition:%d,%d\n", rect.right - rect.left, rect.bottom - rect.top);

	}
	else
	{
		return;
	}
}

void CatWindow_Child::SetWindowSize(int x, int y)
{
	RECT rect,rectclient;
	







	if (GetWindowRect(hWnd, &rect))
	{


		POINT deltaLT, deltaRB,client_RB;
		deltaLT.x = rect.left;
		deltaLT.y = rect.top;



		ScreenToClient(hWnd, &deltaLT);
		GetClientRect(hWnd, &rectclient);
		client_RB.x = rectclient.right;
		client_RB.y = rectclient.bottom;
		ClientToScreen(hWnd, &client_RB);
		deltaRB.x = rect.right - client_RB.x;
		deltaRB.y = rect.bottom - client_RB.y;






			int temwidth, temheight;
			temwidth = x - deltaLT.x + deltaRB.x;//deltaLT.x<0,deltaRB.x>0
			temheight = y - deltaLT.y + deltaRB.y;

			SetWindowPos(hWnd, HWND_TOP,0,0, temwidth, temheight, SWP_NOMOVE|SWP_NOZORDER| SWP_NOACTIVATE);

			//MoveWindow(hWnd, rect.left, rect.top, temwidth, temheight, true);


			printf("setwindowsize:%d,%d\n", temwidth, temheight);
			OnResizeWindow(x, y);
	}

	else
	{
		return;
	}


	
}

void CatWindow_Child::SetWindowPosAndSize(int x, int y, int w, int h)
{
	RECT rect, rectclient;
	GetWindowRect(hWnd, &rect);
	POINT delta;
	delta.x = rect.left;
	delta.y = rect.top;
	POINT lefttop;
	if (ScreenToClient(hWnd, &delta))
	{
		
		lefttop.x = x + delta.x;
		lefttop.y = y + delta.y;

	}
	else
	{
		return;
	}


	{


		POINT deltaLT, deltaRB, client_RB;
		deltaLT.x = rect.left;
		deltaLT.y = rect.top;



		ScreenToClient(hWnd, &deltaLT);
		GetClientRect(hWnd, &rectclient);
		client_RB.x = rectclient.right;
		client_RB.y = rectclient.bottom;
		ClientToScreen(hWnd, &client_RB);
		deltaRB.x = rect.right - client_RB.x;
		deltaRB.y = rect.bottom - client_RB.y;

		int temwidth, temheight;
		temwidth = w - deltaLT.x + deltaRB.x;//deltaLT.x<0,deltaRB.x>0
		temheight = h - deltaLT.y + deltaRB.y;

		

		//MoveWindow(hWnd, rect.left, rect.top, temwidth, temheight, true);






		SetWindowPos(hWnd, HWND_TOP, lefttop.x, lefttop.y, temwidth, temheight, SWP_NOZORDER | SWP_NOACTIVATE);
		//MoveWindow(hWnd, lefttop.x, lefttop.y, rect.right - rect.left, rect.bottom - rect.top, true);
		OnResizeWindow(w, h);

		
	}






}

void CatWindow_Child::SetAspectRatio(int x, int y)
{
	aspectRatio_width = x;
	aspectRatio_height = y;

	int w, h;
	GetWindowSize(&w,&h);
	int s = w * h;
	int newwidth = sqrtf(s* (float(aspectRatio_width)/float(aspectRatio_height)));
	int newheight = newwidth* aspectRatio_height/ aspectRatio_width;
	if (newwidth == 0)
	{
		newwidth = 100;
	}
	if (newheight == 0)
	{
		newheight = 100;
	}
	SetWindowSize(newwidth, newheight);

}

void CatWindow_Child::GetWindowSize(int* x, int* y)
{
	RECT rectclient;
	GetClientRect(hWnd, &rectclient);
	*x = rectclient.right - rectclient.left;
	*y = rectclient.bottom - rectclient.top;
	if (*x == 0)
	{
		*x = lastsizeW;
		*y = lastsizeH;
	}

	printf("getwindowsize:%d,%d\n",*x,*y);
}

void CatWindow_Child::GetWindowAspectRatio(int* x, int* y)
{
	*x = aspectRatio_width;
	*y = aspectRatio_height;
}

void CatWindow_Child::ScaleWindow(int pixel)
{
	if (aspectRatio_width > aspectRatio_height)
	{
		int w, h;
		GetWindowSize(&w, &h);
		if (pixel < 0 && (w + 2 * pixel < 100 || h + 2 * pixel < 100))return;
		int newW, newH;
		newW = w + 2 * pixel;
		newH = newW * aspectRatio_height / aspectRatio_width;
		int deltaH = (newH - h) * 0.5f;
		int px, py;
		GetWindowPosition(&px, &py);
		px -= pixel;
		py -= deltaH;
		SetWindowPosAndSize(px,py,newW,newH);
	}
	else
	{
		int w, h;
		GetWindowSize(&w, &h);
		if (pixel < 0 && (w + 2 * pixel < 100 || h + 2 * pixel < 100))return;
		int newW, newH;
		newH = h + 2 * pixel;
		newW = newH * aspectRatio_width / aspectRatio_height;
		int deltaW = (newW - w) * 0.5f;
		int px, py;
		GetWindowPosition(&px, &py);
		px -= deltaW;
		py -= pixel;
		SetWindowPosAndSize(px, py, newW, newH);
	}
}

void CatWindow_Child::SetVisiability(bool b_show)
{
	if (b_show)
	{
		ShowWindow(hWnd, SW_SHOWNA);
	}
	else
	{
		ShowWindow(hWnd, SW_HIDE);
	}
}



void CatWindow_Child::OnLoseFocus()
{
	printf("Lose Focus\n");
}

CatWindow_Child::~CatWindow_Child()
{
	DestroyWindow(hWnd);
}

LRESULT WINAPI CatWindow_Child::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		CatWindow_Child* const pWnd = static_cast<CatWindow_Child*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CatWindow_Child::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd,msg,wParam, lParam);
	}
	//在WM_NCCREATE之前获得的消息交给默认函数处理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT WINAPI CatWindow_Child::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CatWindow_Child* const pWnd = reinterpret_cast<CatWindow_Child*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CatWindow_Child::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		if (OnCloseWindow())
		{
			return  DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;

	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;

	case WM_MOUSEHOVER:
	{
		//SetWindowLong(hWnd,GWL_STYLE,CAT_WS_NONE|WS_BORDER);
	}
	break;
	case WM_MOUSELEAVE:
	{
		//SetWindowLong(hWnd, GWL_STYLE, CAT_WS_NONE);
		//跟踪
		//TRACKMOUSEEVENT track;
		//track.cbSize = sizeof(TRACKMOUSEEVENT);
		//track.dwFlags = TME_HOVER | TME_LEAVE;
		//track.dwHoverTime = 100;
		//track.hwndTrack = hWnd;
		//TrackMouseEvent(&track);
	}
	break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:

		break;


		//禁用最大化消息
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_MAXIMIZE:
			return(TRUE);
		case SC_MINIMIZE:
			//return(TRUE);
		default:
			break;
		}
		break;


	case WM_WINDOWPOSCHANGING:
	{
		WINDOWPOS* pwindowpos = (WINDOWPOS*)lParam;
		if (pwindowpos->x < -20000 || pwindowpos->y < -20000)
		{
			this->GetWindowPosition(&lastPosX, &lastPosY);
			this->GetWindowSize(&lastsizeW, &lastsizeH);
		}

		break;
	}

	case WM_SIZE:
	{


		break;
	}

	//固定比例缩放
	case WM_SIZING:
	{
		break;
	}


	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}



