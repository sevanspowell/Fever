#include <metal_stdlib>

using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
    float3 normal;
    float3 viewVec;
    float3 lightVec;
};

struct UniformBufferObject {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4x4 invTransposeModel;
    float4 lightPos;
};

vertex VertexOut vertFunc(VertexIn vert [[stage_in]],
                          unsigned int vid [[vertex_id]],
                          const device UniformBufferObject &ubo [[buffer(1)]]) { // Change to buffer(1)?
    VertexOut out;

    out.texCoord = vert.texCoord;

    float3 worldPos = float3(ubo.model * float4(vert.position, 1.0f));

    out.position = ubo.proj * ubo.view * float4(worldPos, 1.0);

    float4 pos = float4(worldPos, 1.0); 
    out.normal = float3x3(ubo.invTransposeModel[0].xyz, ubo.invTransposeModel[1].xyz, ubo.invTransposeModel[2].xyz) * vert.normal;

    float3 lightPos = float3(ubo.lightPos);
    float3 lPos = float3x3(ubo.model[0].xyz, ubo.model[1].xyz, ubo.model[2].xyz) * lightPos;
    out.lightVec = lPos - pos.xyz;
    out.viewVec = float3(ubo.view[3]);

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]], texture2d<float>
diffuseTexture [[texture(0)]], sampler samplr [[sampler(0)]]) {
    float4 color = diffuseTexture.sample(samplr, inFrag.texCoord);

    float3 N = normalize(inFrag.normal);
    float3 L = normalize(inFrag.lightVec);
    float3 V = normalize(inFrag.viewVec);
    float3 R = reflect(-L, N);
    float3 diffuse = max(dot(N, L), 0.0) * float3(1.0);
    float specular = pow(max(dot(R, V), 0.0), 16.0) * color.a;

    return float4(diffuse * color.rgb + specular, 1.0);
    // return float4(diffuse * color.rgb, 1.0);
}
