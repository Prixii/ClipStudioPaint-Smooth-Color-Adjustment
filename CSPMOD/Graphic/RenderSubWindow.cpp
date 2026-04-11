#include "RenderSubWindow.h"

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
	static constexpr const WCHAR* wndClassName = L"CSPMOD DX Child Window";
	static WindowClass wndClass;
	HINSTANCE hInst;
};

WindowClass WindowClass::wndClass;

WindowClass::WindowClass()noexcept
	:hInst(GetModuleHandle(NULL))
{

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = RenderSubWindow::HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetName();
	wc.hIconSm = NULL;
	auto result = RegisterClassEx(&wc);
	return;
}

WindowClass::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const WCHAR* WindowClass::WindowClass::GetName()noexcept
{
	return wndClassName;
}

HINSTANCE WindowClass::WindowClass::GetInstance()noexcept
{
	return wndClass.hInst;
}




bool RenderSubWindow::Create(HWND parent, int x, int y, int w, int h)
{
    if (hwnd) return true;
	if (w == 0 || h == 0)return false;


    hwnd = CreateWindowEx(
        0,
		WindowClass::GetName(), // 使用内置的 Static 类作为宿主，或者注册自定义类
        L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        x, y, w, h, // 初始位置和大小
        parent,
        0,
		WindowClass::GetInstance(),
        this
    );

    if (!hwnd) return false;



    sizeW = w;
    sizeH = h;

	::ShowWindow(hwnd, SW_SHOWDEFAULT);

    //创建交换链





    return true;
}




bool RenderSubWindow::InitDX()
{



	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	auto pDevice=D3D11Graphic::GetDevice();
	DXThrowIfFailed(pDevice->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice));

	

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	DXThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));



	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
	DXThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory));




	DXGI_SWAP_CHAIN_DESC1 scdesc;
	scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scdesc.BufferCount = 2;
	scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scdesc.Flags = NULL;
	scdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scdesc.Width = sizeW;
	scdesc.Height = sizeH;
	scdesc.SampleDesc.Count = 1;
	scdesc.SampleDesc.Quality = 0;
	//scdesc.SampleDesc.Count = 4;
	//UINT x4MsaaQuality;
	//GetDevice()->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM,4, &x4MsaaQuality);
	//scdesc.SampleDesc.Quality = x4MsaaQuality - 1;
	scdesc.Scaling = DXGI_SCALING_STRETCH;
	scdesc.Stereo = FALSE;
	scdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;





	//DXThrowIfFailed(
	//	GetDxgiFactory()->CreateSwapChainForHwnd(GetDevice(), hwnd, &scdesc, NULL, NULL, &pSwapChain));
	DXThrowIfFailed(
		dxgiFactory->CreateSwapChainForHwnd(pDevice, hwnd, &scdesc, NULL, NULL, &pSwapChain));
	
	DXThrowIfFailed(
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

	DXThrowIfFailed(
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));




	return true;
}

void RenderSubWindow::Begin()
{
	if (resizeFlag)
	{
		pBackBuffer.Reset();
		pRenderTarget.Reset();
		DXThrowIfFailed(
			pSwapChain->ResizeBuffers(0, resizeTargetW, resizeTargetH, DXGI_FORMAT_UNKNOWN, 0));

		DXThrowIfFailed(
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

		DXThrowIfFailed(
			D3D11Graphic::GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));

		sizeW = resizeTargetW;
		sizeH = resizeTargetH;
		resizeFlag = false;
	}
}


void RenderSubWindow::Clear(float r, float g, float b, float a)
{
	float clearColor[4] = { r,g,b,a };
	D3D11Graphic::GetContext()->ClearRenderTargetView(pRenderTarget.Get(), clearColor);
}

void RenderSubWindow::Clear()
{
	Clear(0.f,0.f,0.f,0.f);
}


void RenderSubWindow::BindBackBuffer()
{
	D3D11Graphic::GetContext()->OMSetRenderTargets(1, pRenderTarget.GetAddressOf(),nullptr);

	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(sizeW);
	vp.Height = static_cast<float>(sizeH);
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	D3D11Graphic::GetContext()->RSSetViewports(1, &vp);
}

void RenderSubWindow::End()
{
	HRESULT hr = pSwapChain->Present(0u, 0u);
	if (FAILED(hr))
	{
		DXThrowIfFailed(D3D11Graphic::GetDevice()->GetDeviceRemovedReason());
	}
}








void RenderSubWindow::OnResizeWindow(int w, int h)
{
	if (sizeW == w && sizeH == h)
	{
		resizeFlag = false;
	}
	resizeTargetW = w;
	resizeTargetH = h;
	resizeFlag = true;

}







LRESULT WINAPI RenderSubWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		RenderSubWindow* const pWnd = static_cast<RenderSubWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&RenderSubWindow::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	//在WM_NCCREATE之前获得的消息交给默认函数处理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT WINAPI RenderSubWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RenderSubWindow* const pWnd = reinterpret_cast<RenderSubWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}




LRESULT RenderSubWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		return  DefWindowProc(hWnd, msg, wParam, lParam);

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
		break;
	}


	case WM_SIZE:
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);

		if (wParam != SIZE_MINIMIZED) {
			// 在这里重建或 ResizeBuffers

			OnResizeWindow(width,height);
		}
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

