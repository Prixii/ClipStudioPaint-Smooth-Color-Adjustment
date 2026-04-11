cbuffer _ConstantBuffer : register(b0)
{
    float h_rad;
    float pixperuv;
    float padding2;
    float padding3;
};

//通过对透明度模糊来抗锯齿
struct PS_IN
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D baseTex : register(t0);
SamplerState TableSampler : register(s0);
float4 main(PS_IN In) : SV_TARGET
{
    return baseTex.Sample(TableSampler, In.uv);
    int colorCount;
    
    //LT
    float x = In.uv.x * pixperuv;
    float y = In.uv.y * pixperuv;
    float4 curColorLT = baseTex.Load(int3(x - 1, y-1, 0)) * 0.05;
    float4 curColorT = baseTex.Load(int3(x, y-1, 0)) * 0.1;
    float4 curColorRT = baseTex.Load(int3(x+1, y-1, 0)) * 0.05;
    
    float4 curColorL = baseTex.Load(int3(x-1, y, 0)) * 0.1;
    float4 origColor = baseTex.Load(int3(x, y, 0)) ;
    float4 curColor = origColor * 0.4;
    float4 curColorR = baseTex.Load(int3(x+1 , y, 0)) * 0.1;
    
    float4 curColorLB = baseTex.Load(int3(x-1,y+1, 0)) * 0.05;
    float4 curColorB = baseTex.Load(int3(x, y+1, 0)) * 0.1;
    float4 curColorRB = baseTex.Load(int3(x + 1, y+1, 0)) * 0.05;
        
    
    float4 sumColor = curColorLT + curColorT + curColorRT
    + curColorL + curColor + curColorR
    + curColorLB + curColorB + curColorRB;
    
    //return sumColor;
    //return float4(1, 0, 0, 1);
    if (any(origColor))
    {
        
        return float4(origColor.rgb, sumColor.a);
    }
    else
    {
        return float4(sumColor.rgb / sumColor.a, sumColor.a);

    }
}