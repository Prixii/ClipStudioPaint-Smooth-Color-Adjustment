#ifndef ReplaceColorContext_h
#define ReplaceColorContext_h


#include <d3d11.h>
#include <cstdint>
#include"Graphic/Texture.h"

#define MAX_ReplaceColor_RADIUS 256

class ReplaceColorContext
{
public:
	static ReplaceColorContext& GetIns() { static ReplaceColorContext ref; return ref; }
	static void ShutDown() {

		GetIns().pVertexBuffer.Reset();
		GetIns().pIndexBuffer.Reset();
		GetIns().pInputLayout.Reset();
		GetIns().pFilterVS.Reset();
		GetIns().pReplaceColorPS.Reset();

		GetIns().pReplaceColorGenMaskPS.Reset();
		GetIns().pReplaceColorGenMaskPS_Add.Reset();
		GetIns().pReplaceColorGenMaskPS_Sub.Reset();
		GetIns().pReplaceColorPreviewSelecetAreaPS.Reset();


		GetIns().pSampler.Reset();
		GetIns().pUniformBuffer.Reset();
		GetIns().needInit = true;
	};
	


	bool CreateContext();

	void BindAndDraw(
		RenderTexture* renderTarget,
		Texture* orgiTexture,
		Texture* selectAreaTexture,
		Texture* maskActive,//初始为纯白吧
		float H,
		float S,
		float V,
		float strength
		);
	void GenerateMaskTexture(
		RenderTexture* maskActive, 
		Texture* orgiTexture,
		//Texture* selectAreaTexture, //不使用选区，选区用于最后输出时做乘法，//渲染选区时也先渲染选区，然后乘法混合maskActive
		Texture* maskOrig,
		float selectR,
		float selectG,
		float selectB,
		float tolerance,
		ReplaceColor::PickType pickType
		);


	void RenderSelectAreaPreviewTexture(
		Texture* orgiTexture,
		Texture* selectAreaTexture, 
		Texture* maskActive
		);



private:
	//bool _Cleanup();//使用了智能指针，不需要clean


	//d3d11需要16字节对齐，我不想在着色器中将寻址复杂化所以构建了一个16字节的结构表示浮点数
	struct BUFFER_POINT
	{
		int offset;
		float value[3]; 
		//BUFFER_FOLOAT& operator=(const float& other)
		//{
		//	this->value = other;
		//	return *this;
		//}
		//operator float() const { // 必须是const成员函数
		//	return static_cast<float>(this->value); // 返回float类型值
		//}
	};


	struct ReplaceColorUniform
	{
		float H;
		float S;
		float V;
		float strength;
	};

	struct GenerateMaskUniform
	{
		float selectR;
		float selectG;
		float selectB;
		float tolerance;
	};

	bool needInit = true;



	Microsoft::WRL::ComPtr <ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr <ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> pInputLayout;
	//Microsoft::WRL::ComPtr <id3d11topo> pInputLayout;

	Microsoft::WRL::ComPtr <ID3D11VertexShader> pFilterVS;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> pReplaceColorPS;

	Microsoft::WRL::ComPtr <ID3D11PixelShader> pReplaceColorGenMaskPS;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> pReplaceColorGenMaskPS_Add;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> pReplaceColorGenMaskPS_Sub;

	Microsoft::WRL::ComPtr <ID3D11PixelShader> pReplaceColorPreviewSelecetAreaPS;

	Microsoft::WRL::ComPtr <ID3D11SamplerState> pSampler;

	Microsoft::WRL::ComPtr <ID3D11Buffer> pUniformBuffer;//像素着色器参数



};


#endif // !1
