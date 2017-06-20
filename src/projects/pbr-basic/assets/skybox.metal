#include <metal_stdlib>

using namespace metal;

struct SkyboxVertexIn {
    float3 position [[attribute(0)]];
};

struct SkyboxVertexOut {
    float4 position [[position]];
    float3 uvw; // For cubemap texture sampling
};

struct SkyboxUniformBufferObject {
    float4x4 proj;
    float4x4 view;
    float4x4 model;
};

vertex SkyboxVertexOut vertFunc(SkyboxVertexIn vert [[stage_in]],
                          unsigned int vid [[vertex_id]],
                          const device SkyboxUniformBufferObject &ubo [[buffer(1)]]) {
    SkyboxVertexOut out;

    out.uvw = vert.position;
    out.uvw.y *= -1.0f;

    out.position = ubo.proj * ubo.view * ubo.model * float4(vert.position, 1.0f);

    return out;
}

fragment float4 fragFunc(SkyboxVertexOut inFrag [[stage_in]]) {
    return float4(0, 0, 0, 1);
}
