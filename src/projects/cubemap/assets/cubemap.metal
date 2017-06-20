#include <metal_stdlib>

using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VertexOut {
    float4 mtlPosition [[position]];
};

struct UniformBufferObject {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

vertex VertexOut vertFunc(VertexIn vert [[stage_in]],
                          unsigned int vid [[vertex_id]],
                          const device UniformBufferObject &ubo [[buffer(1)]]) { // Change to buffer(1)?
    VertexOut out;

    out.mtlPosition = ubo.proj * ubo.view * ubo.model * float4(vert.position, 1.0);

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]], texturecube<float>
cubemapTexture [[texture(0)]], sampler cubemapSampler [[sampler(0)]]) {
    return float4(1, 0, 0, 1);
}
