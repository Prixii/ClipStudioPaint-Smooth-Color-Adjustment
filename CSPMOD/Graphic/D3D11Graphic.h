#ifndef D3D11Graphic_h
#define D3D11Graphic_h


#include<d3d11.h>
#include<wrl/client.h>



#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")


#define DXThrowIfFailed(X) X

class D3D11Graphic
{
private:
	static D3D11Graphic& GetIns() {
		static D3D11Graphic
			ref; return ref;
	}

public:
	static bool Init();
	static bool ShutDown();



	static ID3D11Device* GetDevice()
	{
		auto& ref = GetIns();
		if (ref.pdevice)
		{
			return ref.pdevice.Get();
		}
		Init();
		return ref.pdevice.Get();
	}
	
	static ID3D11DeviceContext* GetContext()
	{
		auto& ref = GetIns();
		if (ref.pcontext)
		{
			return ref.pcontext.Get();
		}
		Init();
		return ref.pcontext.Get();
	}
	
	static void BindDefaultBlend()
	{
		auto& ref = GetIns();
		float f[] = { 1.f,1.f,1.f,1.f };
		ref.pcontext->OMSetBlendState(ref.pblendstate.Get(), f, 0XFFFFFFFF);
	}
	 



	Microsoft::WRL::ComPtr<ID3D11Device> pdevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pcontext;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pblendstate;


};














#endif