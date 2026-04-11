
#include "pch.h"
#include"Bindables.h"
#include "Window2DComponent_NOTDCOMP.h"




void c2d::Window2DComponent_NDCOM::Init(HWND hwnd, unsigned int bufferW, unsigned int bufferH, Cat2D* _pC2D)
{
	pC2D = _pC2D;
	if (pC2D == NULL)
	{
		pC2D = &Cat2D::GetDefaultInstance();
	}


	//std::string result;
	//HRESULT hr=c2d.Init(&result);
	//DXThrowIfFailedWithSingleInfo(hr, result);
	//hr = c2dcommon.Init(&result);
	//DXThrowIfFailedWithSingleInfo(hr, result);

	auto& c2dcommon = *pC2D;


	HRESULT hr = c2dcommon.Init();



	DXThrowIfFailed(hr);
	//hr = c2dcommon.Init(&result);
	//DXThrowIfFailedWithSingleInfo(hr, result);




	//DXGI_SWAP_CHAIN_DESC1 scdesc;
	//scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//scdesc.BufferCount = 2;
	//scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//scdesc.Flags = NULL;
	//scdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//scdesc.Width = bufferW;
	//scdesc.Height = bufferH;
	//scdesc.SampleDesc.Count = 1;
	//scdesc.SampleDesc.Quality = 0;
	//scdesc.Scaling = DXGI_SCALING_STRETCH;
	//scdesc.Stereo = FALSE;
	//scdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


	DXGI_SWAP_CHAIN_DESC1 scdesc;
	scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scdesc.BufferCount = 2;
	scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scdesc.Flags = NULL;
	scdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scdesc.Width = bufferW;
	scdesc.Height = bufferH;
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
	{
		C2DLOG_INFO("Execute CreateDCSwapChain...");
		DXThrowIfFailed(
			GetDxgiFactory(c2dcommon)->CreateSwapChainForHwnd(GetDxgiDevice(c2dcommon), hwnd, &scdesc, NULL, NULL, &pSwapChain));

	}

	C2DLOG_INFO("Execute GetSwapChainBuffer...");
	DXThrowIfFailed(
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

	C2DLOG_INFO("Execute CreateRenderTargetView...");
	DXThrowIfFailed(
		GetDevice(c2dcommon)->CreateRenderTargetView(pBackBuffer.Get(),NULL, &pRenderTarget));
	C2DLOG_INFO("Window Graphic Init Finished.");



	//C2DLOG_INFO(std::string("viewport Set (")+ std::to_string(bufferW)+","+ std::to_string(bufferH)+")");
	//viewport.reset(new bind::Viewport(bufferW,bufferH));



	C2DLOG_INFO("offscreenTexture reset");
	offscreenTexture.reset(new bind::RenderTexture(c2dcommon,cat::Vector2i(bufferW, bufferH),true));




}

void c2d::Window2DComponent_NDCOM::SetViewPort(unsigned int viewW, unsigned int viewH)
{
	assert(false);
	//viewport->SetViewport(viewW, viewH);
}



void c2d::Window2DComponent_NDCOM::SetOpacity(float opacity)
{
	//sprite->SetOpacity(opacity);
	pDCompEffectGroup->SetOpacity(opacity);
}

void c2d::Window2DComponent_NDCOM::SetFlip(bool hor, bool ver)
{
	//sprite->SetFlip(hor,ver);

}


void c2d::Window2DComponent_NDCOM::Present()
{
	auto& c2dcommon= *pC2D;
#ifdef Codeignore
	offscreenTexture->ReleaseKey(1);
	offscreenTexture_share->LockKey(1);
	GetContext(c2d)->ResolveSubresource(pBackBuffer.Get(),0, offscreenTexture_share->GetResource(),0, DXGI_FORMAT_B8G8R8A8_UNORM);
	//Cat2D::SetRenderTarget(pRenderTarget.Get());
	//viewport->Bind(Cat2D::GetInstance());
	//Cat2D::Draw(*sprite);
	HRESULT hr = pSwapChain->Present(0u, 0u);

	{
		GetCompDevice(c2d)->Commit();//可能得放在外面每帧运行一次，而不是每帧中每窗口运行一次

	}
	//HRESULT hr = pSwapChain->Present1(0u, 0u, &dpp);

	offscreenTexture_share->ReleaseKey(1);
	offscreenTexture->LockKey(1);


	if (FAILED(hr))
	{
		hr = GetDevice(c2d)->GetDeviceRemovedReason();
	}




	ID3D11ShaderResourceView* const pNullTex = nullptr;
	GetContext(c2dcommon)->PSSetShaderResources(0, 1, &pNullTex); // fullscreen input texture
	//GetContext()->ClearState();



	if (isResized && targetSizeW > 0 && targetSizeH > 0)
	{
		float currentSizeW = targetSizeW;
		float currentSizeH = targetSizeH;
		isResized = false;
		pRenderTarget.Reset();
		pBackBuffer.Reset();
		DXThrowIfFailed(
			pSwapChain->ResizeBuffers(0, currentSizeW, currentSizeH, DXGI_FORMAT_UNKNOWN, 0));
		DXThrowIfFailed(
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

		DXThrowIfFailed(
			GetDevice(c2d)->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));
		viewport->SetViewport(currentSizeW, currentSizeH);
		pDCompVisual->SetContent(pSwapChain.Get());
		pComposTarget->SetRoot(pDCompVisual.Get());
		offscreenTexture->Resize(currentSizeW, currentSizeH);
		offscreenTexture_share.reset(new Cat2DTexture(c2d, offscreenTexture->GetShareHandle()));
		ResizedFlag = true;
	}

#endif


	offscreenTexture->CopyToBuffer(pBackBuffer.Get());



	HRESULT hr = pSwapChain->Present(0u, 0u);
	if (FAILED(hr))
	{


		hr = GetDevice(c2dcommon)->GetDeviceRemovedReason();

		if (FAILED(hr))\
		{\
			throw DxException(__LINE__, __FILE__, (hr)); \
		}\

	}
	{
		GetCompDevice(c2dcommon)->Commit();//todo可能得放在外面每帧运行一次，而不是每帧中每窗口运行一次

	}
	//HRESULT hr = pSwapChain->Present1(0u, 0u, &dpp);

	//offscreenTexture_share->ReleaseKey(1);
	//offscreenTexture->LockKey(1);







	ID3D11ShaderResourceView* const pNullTex = nullptr;
	GetContext(c2dcommon)->PSSetShaderResources(0, 1, &pNullTex); // fullscreen input texture
	//GetContext()->ClearState();

	//vcam.PushFrame();

	//只有在vcam未启用的时候可以改变缓冲大小

	if (isResized && targetSizeW > 0 && targetSizeH > 0/*&& vcam.GetSlot()==-1*/)
	{
		float currentSizeW = targetSizeW;
		float currentSizeH = targetSizeH;
		isResized = false;
		pRenderTarget.Reset();
		pBackBuffer.Reset();
		DXThrowIfFailed(
			pSwapChain->ResizeBuffers(0, currentSizeW, currentSizeH, DXGI_FORMAT_UNKNOWN, 0));
		DXThrowIfFailed(
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

		DXThrowIfFailed(
			GetDevice(c2dcommon)->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));
		//viewport->SetViewport(currentSizeW, currentSizeH);
		offscreenTexture->Resize(cat::Vector2i(currentSizeW, currentSizeH));
		//offscreenTexture_share.reset(new Cat2DTexture(c2d, offscreenTexture->GetShareHandle()));
		ResizedFlag = true;
	}
}

HANDLE c2d::Window2DComponent_NDCOM::GetTextureHandle()
{
	assert(false);
	return NULL;
}

void c2d::Window2DComponent_NDCOM::LockTexture(uint64_t key)
{
	assert(false);
	//offscreenTexture->LockKey(key);

}

void c2d::Window2DComponent_NDCOM::ReleaseTexture(uint64_t key)
{
	assert(false);
	//offscreenTexture->ReleaseKey(key);
}

void c2d::Window2DComponent_NDCOM::ResizeBuffer(int newW, int newH)
{
	isResized = true;
	targetSizeW = newW;
	targetSizeH = newH;
	


}

void c2d::Window2DComponent_NDCOM::GetViewSize(int* w, int* h)
{
	assert(false);
	//*w=viewport->GetWidth();
	//*h=viewport->GetHeight();
}


bool c2d::Window2DComponent_NDCOM::GetResizeFlag()
{
	if (ResizedFlag)
	{
		ResizedFlag = false;
		return true;
	}
	else
	{
		return false;
	}

}

bool c2d::Window2DComponent_NDCOM::CopyBuffer(uint8_t* buff, size_t buflen, size_t* outlen,ID3D11Resource* pResource)
{
	assert(false);
	auto& c2dcommon = *pC2D;
	GetContext(c2dcommon)->CopyResource(pResource, pBackBuffer.Get());

	D3D11_MAPPED_SUBRESOURCE resultResources;
	GetContext(c2dcommon)->Map(pResource, 0, D3D11_MAP_READ, 0, &resultResources);




	
	auto screenSize=offscreenTexture->GetSize();
	int height = screenSize.x;
	int width = screenSize.y;
	*outlen = width * height * 4;


	for (int i = 0; i < height; i++)
	{
		memcpy(buff + (height - i - 1) * width * 4, (BYTE*)resultResources.pData + i * resultResources.RowPitch, width * 4);

	}




	//memcpy(buff, (BYTE*)resultResources.pData+ resultResources.RowPitch, *outlen);
	GetContext(c2dcommon)->Unmap(pResource, 0);

	return true;

	//return offscreenTexture->CopyToMem(buff, buflen, outlen);













}


//存储代码  sharetex
#ifdef codeignore
void c2d::Window2DComponent_NDCOM::Init(HWND hwnd, unsigned int bufferW, unsigned int bufferH, std::function<void(const std::string&)>outputfunc)
{

	//std::string result;
	//HRESULT hr=c2d.Init(&result);
	//DXThrowIfFailedWithSingleInfo(hr, result);
	//hr = c2dcommon.Init(&result);
	//DXThrowIfFailedWithSingleInfo(hr, result);


	std::string result;
	HRESULT hr = c2d.Init(outputfunc);
	DXThrowIfFailed(hr);
	hr = c2dcommon.Init(&result);
	DXThrowIfFailedWithSingleInfo(hr, result);




	//DXGI_SWAP_CHAIN_DESC1 scdesc;
	//scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	//scdesc.BufferCount = 2;
	//scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//scdesc.Flags = NULL;
	//scdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//scdesc.Width = bufferW;
	//scdesc.Height = bufferH;
	//scdesc.SampleDesc.Count = 1;
	//scdesc.SampleDesc.Quality = 0;
	//scdesc.Scaling = DXGI_SCALING_STRETCH;
	//scdesc.Stereo = FALSE;
	//scdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


	DXGI_SWAP_CHAIN_DESC1 scdesc;
	scdesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	//scdesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scdesc.BufferCount = 2;
	scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scdesc.Flags = NULL;
	scdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scdesc.Width = bufferW;
	scdesc.Height = bufferH;
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
	{
		outputfunc("Execute CreateDCSwapChain...");
		DXThrowIfFailed(
			GetDxgiFactory(c2d)->CreateSwapChainForComposition(GetDxgiDevice(c2d), &scdesc, NULL, &pSwapChain));

		outputfunc("Execute CreateDCTarget...");
		DXThrowIfFailed(
			GetCompDevice(c2d)->CreateTargetForHwnd(
				hwnd,
				// 因为我的窗口不是TOPMOST的, 因此是false
				// 如果加了TOPMOST样式要改成true哈
				false,
				pComposTarget.GetAddressOf()
			));
		outputfunc("Execute CreateDCVisual...");
		DXThrowIfFailed(
			GetCompDevice(c2d)->CreateVisual(&pDCompVisual));
		outputfunc("Execute CreateDCEffectGroup...");
		DXThrowIfFailed(
			GetCompDevice(c2d)->CreateEffectGroup(&pDCompEffectGroup));
		outputfunc("Execute SetDCEffect...");
		DXThrowIfFailed(pDCompVisual->SetEffect(pDCompEffectGroup.Get()));
		outputfunc("Execute SetDCContent...");
		DXThrowIfFailed(pDCompVisual->SetContent(pSwapChain.Get()));
		outputfunc("Execute SetDCRoot...");
		DXThrowIfFailed(pComposTarget->SetRoot(pDCompVisual.Get()));
	}

	outputfunc("Execute GetSwapChainBuffer...");
	DXThrowIfFailed(
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

	outputfunc("Execute CreateRenderTargetView...");
	DXThrowIfFailed(
		GetDevice(c2d)->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));
	outputfunc("Window Graphic Init Finished.");



	outputfunc(std::string("viewport Set (") + std::to_string(bufferW) + "," + std::to_string(bufferH) + ")");
	viewport.reset(new bind::Viewport(bufferW, bufferH));
	outputfunc("sprite reset");
	sprite.reset(new Cat2DSprite(c2d));
	outputfunc("offscreenTexture reset");
	offscreenTexture.reset(new Cat2DTexture(c2dcommon, bufferW, bufferH, true));


	//outputfunc("offscreenTexture_share reset"+ std::to_string(unsigned int(offscreenTexture->GetShareHandle())));
	//offscreenTexture_share.reset(new Cat2DTexture(c2d, offscreenTexture->GetShareHandle()));


	outputfunc("sprite addbind " + std::to_string(unsigned int(offscreenTexture.get())));
	sprite->AddBind(offscreenTexture);



	//hr = pDXGIKeyedMutex->AcquireSync(0, INFINITE);
//hr = pDXGIKeyedMutex->ReleaseSync(1);


}

void c2d::Window2DComponent_NDCOM::SetViewPort(unsigned int viewW, unsigned int viewH)
{
	viewport->SetViewport(viewW, viewH);
}

void c2d::Window2DComponent_NDCOM::Clear(float r, float b, float g, float a)
{

	c2dcommon.SetRenderTarget(offscreenTexture->GetRenderTargetView());
	c2dcommon.Clear(0, 0, 0, 0);

	c2d.SetRenderTarget(pRenderTarget.Get());
	c2d.Clear(r, g, b, a);
}

void c2d::Window2DComponent_NDCOM::SetOffScreenRenderTarget()
{
	c2dcommon.SetRenderTarget(offscreenTexture->GetRenderTargetView());
	//Cat2D::SetRenderTarget(pRenderTarget.Get());
}

void c2d::Window2DComponent_NDCOM::SetOpacity(float opacity)
{
	//sprite->SetOpacity(opacity);
	pDCompEffectGroup->SetOpacity(opacity);
}

void c2d::Window2DComponent_NDCOM::SetFlip(bool hor, bool ver)
{
	sprite->SetFlip(hor, ver);

}


void c2d::Window2DComponent_NDCOM::Present()
{

#ifdef Codeignore
	offscreenTexture->ReleaseKey(1);
	offscreenTexture_share->LockKey(1);
	GetContext(c2d)->ResolveSubresource(pBackBuffer.Get(), 0, offscreenTexture_share->GetResource(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
	//Cat2D::SetRenderTarget(pRenderTarget.Get());
	//viewport->Bind(Cat2D::GetInstance());
	//Cat2D::Draw(*sprite);
	HRESULT hr = pSwapChain->Present(0u, 0u);

	{
		GetCompDevice(c2d)->Commit();//可能得放在外面每帧运行一次，而不是每帧中每窗口运行一次

	}
	//HRESULT hr = pSwapChain->Present1(0u, 0u, &dpp);

	offscreenTexture_share->ReleaseKey(1);
	offscreenTexture->LockKey(1);


	if (FAILED(hr))
	{
		hr = GetDevice(c2d)->GetDeviceRemovedReason();
	}




	ID3D11ShaderResourceView* const pNullTex = nullptr;
	GetContext(c2dcommon)->PSSetShaderResources(0, 1, &pNullTex); // fullscreen input texture
	//GetContext()->ClearState();



	if (isResized && targetSizeW > 0 && targetSizeH > 0)
	{
		float currentSizeW = targetSizeW;
		float currentSizeH = targetSizeH;
		isResized = false;
		pRenderTarget.Reset();
		pBackBuffer.Reset();
		DXThrowIfFailed(
			pSwapChain->ResizeBuffers(0, currentSizeW, currentSizeH, DXGI_FORMAT_UNKNOWN, 0));
		DXThrowIfFailed(
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

		DXThrowIfFailed(
			GetDevice(c2d)->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));
		viewport->SetViewport(currentSizeW, currentSizeH);
		pDCompVisual->SetContent(pSwapChain.Get());
		pComposTarget->SetRoot(pDCompVisual.Get());
		offscreenTexture->Resize(currentSizeW, currentSizeH);
		offscreenTexture_share.reset(new Cat2DTexture(c2d, offscreenTexture->GetShareHandle()));
		ResizedFlag = true;
	}

#endif


	//offscreenTexture->ReleaseKey(1);
	//offscreenTexture_share->LockKey(1);
	GetContext(c2d)->ResolveSubresource(pBackBuffer.Get(), 0, offscreenTexture->GetResource(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
	//Cat2D::SetRenderTarget(pRenderTarget.Get());
	//viewport->Bind(Cat2D::GetInstance());
	//Cat2D::Draw(*sprite);
	HRESULT hr = pSwapChain->Present(0u, 0u);

	{
		GetCompDevice(c2d)->Commit();//可能得放在外面每帧运行一次，而不是每帧中每窗口运行一次

	}
	//HRESULT hr = pSwapChain->Present1(0u, 0u, &dpp);

	//offscreenTexture_share->ReleaseKey(1);
	//offscreenTexture->LockKey(1);


	if (FAILED(hr))
	{
		hr = GetDevice(c2d)->GetDeviceRemovedReason();
	}




	ID3D11ShaderResourceView* const pNullTex = nullptr;
	GetContext(c2dcommon)->PSSetShaderResources(0, 1, &pNullTex); // fullscreen input texture
	//GetContext()->ClearState();



	if (isResized && targetSizeW > 0 && targetSizeH > 0)
	{
		float currentSizeW = targetSizeW;
		float currentSizeH = targetSizeH;
		isResized = false;
		pRenderTarget.Reset();
		pBackBuffer.Reset();
		DXThrowIfFailed(
			pSwapChain->ResizeBuffers(0, currentSizeW, currentSizeH, DXGI_FORMAT_UNKNOWN, 0));
		DXThrowIfFailed(
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));

		DXThrowIfFailed(
			GetDevice(c2d)->CreateRenderTargetView(pBackBuffer.Get(), NULL, &pRenderTarget));
		viewport->SetViewport(currentSizeW, currentSizeH);
		pDCompVisual->SetContent(pSwapChain.Get());
		pComposTarget->SetRoot(pDCompVisual.Get());
		offscreenTexture->Resize(currentSizeW, currentSizeH);
		//offscreenTexture_share.reset(new Cat2DTexture(c2d, offscreenTexture->GetShareHandle()));
		ResizedFlag = true;
	}
}

HANDLE c2d::Window2DComponent_NDCOM::GetTextureHandle()
{
	return offscreenTexture->GetShareHandle();
}

void c2d::Window2DComponent_NDCOM::LockTexture(uint64_t key)
{
	offscreenTexture->LockKey(key);

}

void c2d::Window2DComponent_NDCOM::ReleaseTexture(uint64_t key)
{
	offscreenTexture->ReleaseKey(key);
}

void c2d::Window2DComponent_NDCOM::ResizeBuffer(int newW, int newH)
{
	isResized = true;
	targetSizeW = newW;
	targetSizeH = newH;



}

void c2d::Window2DComponent_NDCOM::GetViewSize(int* w, int* h)
{
	*w = viewport->GetWidth();
	*h = viewport->GetHeight();
}

bool c2d::Window2DComponent_NDCOM::GetResizeFlag()
{
	if (ResizedFlag)
	{
		ResizedFlag = false;
		return true;
	}
	else
	{
		return false;
	}

}


#endif





