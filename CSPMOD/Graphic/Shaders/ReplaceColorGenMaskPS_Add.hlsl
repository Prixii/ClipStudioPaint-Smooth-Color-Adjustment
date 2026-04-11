#include"Tolerance.hlsli"

cbuffer _ConstantBuffer : register(b0)
{
    float selectR;
    float selectG;
    float selectB;
    float tolerance;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


Texture2D origTexture : register(t0);
Texture2D origMask : register(t1);


SamplerState textureSampler : register(s0);








//普通的选择模式

float4 main(PS_IN psIN) : SV_TARGET
{
    float4 origColor = origTexture.Sample(textureSampler, psIN.uv);
    
    float weight = CalcPixWeight(origColor.rgb, float3(selectR, selectG, selectB), tolerance);
    weight = max(weight, origMask.Sample(textureSampler, psIN.uv).r);
    return float4(weight, weight, weight, 1.0f);
}