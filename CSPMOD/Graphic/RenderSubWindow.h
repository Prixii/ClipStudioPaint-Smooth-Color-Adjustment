
#ifndef RENDERSUBWINDOW_h
#define RENDERSUBWINDOW_h



//提供D3D11渲染功能的子窗口
#include"pch.h"
#include"dxgi1_2.h"
#include<SDL3/SDL.h>
#include"D3D11Graphic.h"
class RenderSubWindow
{


public:

	bool Create(HWND parent,int x,int y ,int w,int h);



	bool InitDX();


	HWND GetNativeWindow() { return hwnd; }


	void Begin();
	void Clear(float r,float g,float b,float a);
	void Clear();
	void DrawTexture(class Texture* pTexture);

	void BindBackBuffer();

	void End();





	//这两个消息函数被用来传递CatWindow_Child类自身指针
	//static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lpParam);
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lpParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lpParam);
protected:
	virtual LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept;

	virtual void OnResizeWindow(int w, int h);

private:


	bool resizeFlag = false;
	int resizeTargetW = 0;
	int resizeTargetH = 0;

	//buffer size
	int sizeW=0;
	int sizeH=0;




	HWND hwnd=0;

	WNDPROC origProc;


	Microsoft::WRL::ComPtr <IDXGISwapChain1> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> pRenderTarget;
};











#endif