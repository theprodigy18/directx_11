cbuffer Cbuffer
{
    matrix transform;
};

float4 VSMain(float3 pos: Position): SV_Position
{
    return mul(float4(pos, 1.0f), transform);
}


cbuffer Cbuffer2
{
    float4 faceColor[6];
}

float4 PSMain(uint tID: SV_PrimitiveID): SV_Target
{
    return faceColor[tID / 2];
}