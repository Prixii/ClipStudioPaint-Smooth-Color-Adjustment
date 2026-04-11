#include "pch.h"
#include "D3D11Graphic.h"


bool D3D11Graphic::Init()
{
	auto& ref = GetIns();


	//创建设备和context
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;	// Direct2D需要支持BGRA格式
#if defined(DEBUG) || defined(_DEBUG)  
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		//C2DLOG_INFO("Execute D3D11CreateDevice...");
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, NULL, 0, D3D11_SDK_VERSION, &ref.pdevice, NULL, &ref.pcontext);
		if (FAILED(hr))
		{
			
			//C2DLOG_ERROR("D3D11CreateDevice failed in Cat2D::Init.");
		}
	}



	if (SUCCEEDED(hr))
	{
		ref.pdevice->GetImmediateContext(&ref.pcontext);


		if (FAILED(hr))
		{
			//C2DLOG_ERROR("Device.As(DXGIDevice) failed in Cat2D::Init.");
		}

	}

	if (SUCCEEDED(hr))
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE; // 启用混合
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 源混合因子：使用源像素的 Alpha
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 目标混合因子：使用 (1 - 源Alpha)
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 混合操作：相加

		// Alpha 通道通常不需要混合，直接覆盖即可
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		// 允许写入所有颜色通道 (RGBA)
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// 2. 创建混合状态对象
		//ID3D11BlendState* pBlendState = nullptr;
		HRESULT hr = ref.pdevice->CreateBlendState(&blendDesc, &ref.pblendstate);
		if (FAILED(hr)) {
			// 处理创建失败
		}


	}

	if (SUCCEEDED(hr))
	{
		return true;
	} 
	return false;
}

bool D3D11Graphic::ShutDown()
{
	GetIns().pdevice.Reset();
	GetIns().pcontext.Reset();


	return false;
}
