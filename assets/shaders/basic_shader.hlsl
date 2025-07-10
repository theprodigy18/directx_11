struct VSOut
{
    float3 color: Color;
    float4 pos: SV_Position;
};

cbuffer Cbuffer
{
    matrix transform;
};

VSOut VSMain(float2 pos: Position, float3 color: Color)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 0.0f, 1.0f), transform);
    vso.color = color;

    return vso;
}


float4 PSMain(float3 color: Color): SV_Target
{
    return float4(color, 1.0f);
}