struct VS_IN
{
    float2 pos : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT main(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    Out.Position = float4(In.pos, 0.0f, 1.0f);
    Out.uv=In.uv;
    return Out;
}