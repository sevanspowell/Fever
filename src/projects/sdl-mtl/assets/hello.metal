#include <metal_stdlib>

using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float3 color;
};

constant float2 positions[3] = {
    float2(0.0f, -0.5f),
    float2(0.5f, 0.5f),
    float2(-0.5f, 0.5f)
};

constant float3 colors[3] = {
    float3(1.0f, 0.0f, 0.0f),
    float3(1.0f, 0.0f, 0.0f),
    float3(1.0f, 0.0f, 0.0f)
};

vertex VertexOut vertFunc(unsigned int vid [[vertex_id]]) {
    VertexOut out;

    out.position = float4(positions[vid], 0.0f, 1.0f);
    out.color = colors[vid];

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]]) {
    return float4(1, 0, 0, 1.0f);
}