#include <metal_stdlib>

using namespace metal;

struct VertexIn {
    float2 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 color;
};

vertex VertexOut vertFunc(VertexIn vert [[stage_in]],
                          unsigned int vid [[vertex_id]]) {
    VertexOut out;

    out.color = vert.color;
    out.position = float4(vert.position, 0.0f, 1.0f);

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]]) {
    return float4(inFrag.color, 1.0f);
}