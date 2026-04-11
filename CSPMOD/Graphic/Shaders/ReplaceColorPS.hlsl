#include"HSVConvert.hlsli"


cbuffer _ConstantBuffer : register(b0)
{
    float H;
    float S;
    float V;
    float Strength;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


Texture2D origTexture : register(t0);
Texture2D selectAreaTexture : register(t1);
Texture2D activeMaskTexture : register(t2);


SamplerState textureSampler : register(s0);










float4 main(PS_IN psIN) : SV_TARGET
{

    
    float4 origColor = origTexture.Sample(textureSampler, psIN.uv);
    //float origAlpha = alphaTexture.Sample(textureSampler, psIN.uv).a;
    
   
    
    
    //测试alpha
    if (origColor.a == 0.f)
    {
        return origColor;
    }
    
    float selectAlpha = selectAreaTexture.Sample(textureSampler, psIN.uv).z*
    activeMaskTexture.Sample(textureSampler, psIN.uv).z*
    Strength;
    

    
    if (selectAlpha == 0)
    {
        return origColor;
    }
    
    float3 orgHSV = RGBtoHSV(origColor.xyz);
    
    float3 hsvParam;
    hsvParam.x = H;
    hsvParam.y = S;
    hsvParam.z = V;
    
    hsvParam = HSVFilter(orgHSV, hsvParam);
    
    
    float3 destColor = HSVtoRGB(hsvParam);
    
    //混色？
    float3 resultColor = (destColor.xyz - origColor.xyz) * selectAlpha + origColor.xyz;
    
    
    
    return float4(resultColor, origColor.a);
}