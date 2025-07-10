struct VSOut
{
    float3 color: Color;
    float4 pos: SV_Position;
};

VSOut VSMain(float2 pos: Position, float3 color: Color)
{
    VSOut vso;
    vso.pos = float4(pos, 0.0f, 1.0f);
    vso.color = color;

    return vso;
}


float4 PSMain(float3 color: Color): SV_Target
{
    return float4(color, 1.0f);
}