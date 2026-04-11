struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


Texture2D origTexture : register(t0);
Texture2D selectAreaTexture : register(t1);
Texture2D maskActiveTexture : register(t2);


SamplerState textureSampler : register(s0);








//普通的选择模式

float4 main(PS_IN psIN) : SV_TARGET
{
    float4 origColor = origTexture.Sample(textureSampler, psIN.uv);
    
    
    float currentAlpha = origTexture.Sample(textureSampler, psIN.uv).a *
    selectAreaTexture.Sample(textureSampler, psIN.uv).z *
    maskActiveTexture.Sample(textureSampler, psIN.uv).z;
    
    return float4(currentAlpha, currentAlpha, currentAlpha, 1.0f);
}