#include <metal_stdlib>

using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float3 color;
};

constant float2 positions[3] = {
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};

constant float3 colors[3] = {
    float3(1.0f, 0.0f, 0.0f),
    float3(1.0f, 0.0f, 0.0f),
    float3(1.0f, 0.0f, 0.0f)
};

vertex VertexOut vertFunc(unsigned int vid [[vertex_id]]) {
    VertexOut out;

    unsigned int vertId = vid;

    out.color = colors[vertId];
    out.position = float4(positions[vertId], 0.0f, 1.0f);

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]]) {
    return float4(inFrag.color, 1.0f);
}