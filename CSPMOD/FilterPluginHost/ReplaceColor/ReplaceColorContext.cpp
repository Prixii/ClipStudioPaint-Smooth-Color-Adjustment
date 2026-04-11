#include "pch.h"
#include"Graphic/D3D11Graphic.h"
#include"ReplaceColor.h"
#include "ReplaceColorContext.h"


#include"Graphic/Shaders/FilterVS.h"
#include"Graphic/Shaders/ReplaceColorPS.h"
#include"Graphic/Shaders/ReplaceColorGenMaskPS.h"
#include"Graphic/Shaders/ReplaceColorGenMaskPS_Add.h"
#include"Graphic/Shaders/ReplaceColorGenMaskPS_Sub.h"
#include"Graphic/Shaders/ReplaceColorPreviewSelecetAreaPS.h"





//#include <corecrt_math.h>
#include <stdio.h>








//创建顶点结构
struct Vertex
{
    float posX;
    float posY;
    float u;
    float v;
};




bool ReplaceColorContext::CreateContext()
{
    auto pdevice=D3D11Graphic::GetDevice();
    auto pcontext=D3D11Graphic::GetContext();
    
    //这种不需要对其他地方进行采样的后处理用像素着色器吧
    







    //创建矩形顶点
    {
        Vertex vertexs[4] =
        {
            {-1.f,1.f,  0.f,0.f},//左上
            {1.f,1.f,   1.f,0.f},//右上
            {-1.f,-1.f, 0.f,1.f},//左下
            {1.f,-1.f,  1.f,1.f}//右下
        };

        //创建顶点缓冲区
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT; // GPU 读写优化
        bd.ByteWidth = sizeof(vertexs); // 
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertexs;

        if (FAILED(pdevice->CreateBuffer(&bd, &initData, &pVertexBuffer)))
        {
            return false;
        }
    }


    //索引缓冲
    {
        uint16_t index[] = { 0,1,2,   1,3,2 };

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = index;

        D3D11_BUFFER_DESC ibDesc = {};
        ibDesc.Usage = D3D11_USAGE_DEFAULT;
        ibDesc.ByteWidth = sizeof(index);
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibDesc.CPUAccessFlags = 0;
        if (FAILED(pdevice->CreateBuffer(&ibDesc, &initData, &pIndexBuffer)))
        {
            return false;
        }
    }





    //输入布局input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 } 
    };

    if (FAILED(pdevice->CreateInputLayout(layout, 2, FilterVS, sizeof(FilterVS), &pInputLayout)))
    {
        return false;
    }



    //着色器
    if (FAILED(pdevice->CreateVertexShader(FilterVS, sizeof(FilterVS), NULL, &pFilterVS)))
    {
        return false;
    }
    if (FAILED(pdevice->CreatePixelShader(ReplaceColorPS, sizeof(ReplaceColorPS), NULL, &pReplaceColorPS)))
    {
        return false;
    }
    if (FAILED(pdevice->CreatePixelShader(ReplaceColorGenMaskPS, sizeof(ReplaceColorGenMaskPS), NULL, &pReplaceColorGenMaskPS)))
    {
        return false;
    }
    if (FAILED(pdevice->CreatePixelShader(ReplaceColorGenMaskPS_Add, sizeof(ReplaceColorGenMaskPS_Add), NULL, &pReplaceColorGenMaskPS_Add)))
    {
        return false;
    }
    if (FAILED(pdevice->CreatePixelShader(ReplaceColorGenMaskPS_Sub, sizeof(ReplaceColorGenMaskPS_Sub), NULL, &pReplaceColorGenMaskPS_Sub)))
    {
        return false;
    }
    if (FAILED(pdevice->CreatePixelShader(ReplaceColorPreviewSelecetAreaPS, sizeof(ReplaceColorPreviewSelecetAreaPS), NULL, &pReplaceColorPreviewSelecetAreaPS)))
    {
        return false;
    }


    //采样器
    D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    if (FAILED(pdevice->CreateSamplerState(&samplerDesc, &pSampler)))
    {
        return false;
    }






    //着色器参数数
    {
        D3D11_BUFFER_DESC cbDesc;
        ZeroMemory(&cbDesc, sizeof(cbDesc));
        //对齐16字节
        auto padding = 16 - sizeof(ReplaceColorUniform) % 16; 
        if (padding == 16)padding = 0;
        cbDesc.ByteWidth = static_cast<UINT>(sizeof(ReplaceColorUniform)+ padding);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;      // 允许CPU动态更新
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU可写入
        if (FAILED(pdevice->CreateBuffer(&cbDesc, nullptr, &pUniformBuffer)))
        {
            return false;
        }
    }





	return true;
}

void ReplaceColorContext::BindAndDraw(RenderTexture* renderTarget,Texture* orgiTexture,Texture* selectAreaTexture, Texture* maskActive,
    float H, float S,float V,float strength)
{
    if (needInit)
    {
        if (CreateContext())
            needInit = false;
        else
            return;
    }

    auto pdevice = D3D11Graphic::GetDevice();
    auto pcontext = D3D11Graphic::GetContext();


    //设置参数
    ReplaceColorUniform ReplaceColorUniform = {H,S,V,strength};






    //更新constantBuffer
    D3D11_MAPPED_SUBRESOURCE msr;
    pcontext->Map(pUniformBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
    memcpy(msr.pData, &ReplaceColorUniform, sizeof(ReplaceColorUniform));
    pcontext->Unmap(pUniformBuffer.Get(), 0u);


    //绑定并绘制
    {
        auto rt = renderTarget->GetRT();
        float clearColor[4] = { 0.f,0.f,0.f,0.f };
        pcontext->OMSetRenderTargets(1, &rt,nullptr);
        pcontext->ClearRenderTargetView(rt, clearColor);


        UINT stride = sizeof(Vertex); 
        UINT offset = 0;
        pcontext->IASetVertexBuffers(0,1, pVertexBuffer.GetAddressOf(),&stride,&offset);
        pcontext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT,0);
        pcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pcontext->IASetInputLayout(pInputLayout.Get());

        pcontext->VSSetShader(pFilterVS.Get(),nullptr,0);
        pcontext->PSSetShader(pReplaceColorPS.Get(),nullptr,0);


        ID3D11ShaderResourceView* pTex[3] = { orgiTexture->GetTex(),selectAreaTexture->GetTex(),maskActive->GetTex() };
        pcontext->PSSetShaderResources(0, 3, pTex);
        pcontext->PSSetSamplers(0,1 ,pSampler.GetAddressOf());
        pcontext->PSSetConstantBuffers(0,1, pUniformBuffer.GetAddressOf());


        D3D11_VIEWPORT vp = {};
        vp.Width = static_cast<float>(renderTarget->GetSizeW());
        vp.Height = static_cast<float>(renderTarget->GetSizeH());
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        pcontext->RSSetViewports(1,&vp);

        pcontext->DrawIndexed(6,0,0);


        ID3D11ShaderResourceView* pNullTex[3] = {};
        pcontext->PSSetShaderResources(0, 3, pNullTex);
    
    }




}

void ReplaceColorContext::GenerateMaskTexture(RenderTexture* maskActive, Texture* orgiTexture, Texture* maskOrig, float selectR, float selectG, float selectB, float tolerance, ReplaceColor::PickType pickType)
{

    if (needInit)
    {
        if (CreateContext())
            needInit = false;
        else
            return;
    }

    auto pdevice = D3D11Graphic::GetDevice();
    auto pcontext = D3D11Graphic::GetContext();


    //设置参数
    GenerateMaskUniform generateMaskUniform = { selectR,selectG,selectB,tolerance };






    //更新constantBuffer
    D3D11_MAPPED_SUBRESOURCE msr;
    pcontext->Map(pUniformBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
    memcpy(msr.pData, &generateMaskUniform, sizeof(generateMaskUniform));
    pcontext->Unmap(pUniformBuffer.Get(), 0u);


    //绑定并绘制
    {
        auto rt = maskActive->GetRT();
        float clearColor[4] = { 0.f,0.f,0.f,0.f };
        pcontext->OMSetRenderTargets(1, &rt, nullptr);
        pcontext->ClearRenderTargetView(rt, clearColor);


        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        pcontext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
        pcontext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        pcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pcontext->IASetInputLayout(pInputLayout.Get());

        pcontext->VSSetShader(pFilterVS.Get(), nullptr, 0);

        switch (pickType)
        {
        case ReplaceColor::PickType_Normal:
            pcontext->PSSetShader(pReplaceColorGenMaskPS.Get(), nullptr, 0);

            break;
        case ReplaceColor::PickType_Add:
            pcontext->PSSetShader(pReplaceColorGenMaskPS_Add.Get(), nullptr, 0);

            break;
        case ReplaceColor::PickType_Sub:
            pcontext->PSSetShader(pReplaceColorGenMaskPS_Sub.Get(), nullptr, 0);

            break;
        default:
            break;
        }


        ID3D11ShaderResourceView* pTex[2] = { orgiTexture->GetTex(),maskOrig->GetTex() };
        pcontext->PSSetShaderResources(0, 2, pTex);
        pcontext->PSSetSamplers(0, 1, pSampler.GetAddressOf());
        pcontext->PSSetConstantBuffers(0, 1, pUniformBuffer.GetAddressOf());


        D3D11_VIEWPORT vp = {};
        vp.Width = static_cast<float>(maskActive->GetSizeW());
        vp.Height = static_cast<float>(maskActive->GetSizeH());
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        pcontext->RSSetViewports(1, &vp);

        pcontext->DrawIndexed(6, 0, 0);

        ID3D11ShaderResourceView* pNullTex[3] = {};
        pcontext->PSSetShaderResources(0, 2, pNullTex);

    }



}

void ReplaceColorContext::RenderSelectAreaPreviewTexture( Texture* orgiTexture, Texture* selectAreaTexture, Texture* maskActive)
{


    if (needInit)
    {
        if (CreateContext())
            needInit = false;
        else
            return;
    }

    auto pdevice = D3D11Graphic::GetDevice();
    auto pcontext = D3D11Graphic::GetContext();



    //绑定并绘制
    {

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        pcontext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
        pcontext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        pcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pcontext->IASetInputLayout(pInputLayout.Get());

        pcontext->VSSetShader(pFilterVS.Get(), nullptr, 0);
        pcontext->PSSetShader(pReplaceColorPreviewSelecetAreaPS.Get(), nullptr, 0);





        ID3D11ShaderResourceView* pTex[3] = { orgiTexture->GetTex(),selectAreaTexture->GetTex(),maskActive->GetTex()};
        pcontext->PSSetShaderResources(0, 3, pTex);
        pcontext->PSSetSamplers(0, 1, pSampler.GetAddressOf());




        pcontext->DrawIndexed(6, 0, 0);


        ID3D11ShaderResourceView* pNullTex[3] = {};
        pcontext->PSSetShaderResources(0, 3, pNullTex);

    }

}

