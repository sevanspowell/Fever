#include <metal_stdlib>

using namespace metal;

struct VertexIn {
    float2 position [[attribute(0)]];
    float3 color [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 color;
    float2 texCoord;
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

    out.color = vert.color;
    out.position = ubo.proj * ubo.view * ubo.model * float4(vert.position, 0.0f, 1.0f);
    out.texCoord = vert.texCoord;

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]], texture2d<float>
diffuseTexture [[texture(0)]], sampler samplr [[sampler(0)]]) {
    return diffuseTexture.sample(samplr, inFrag.texCoord);
}
