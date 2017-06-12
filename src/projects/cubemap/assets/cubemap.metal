#include <metal_stdlib>

using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VertexOut {
    float4 mtlPosition [[position]];
    float3 pos;
    float3 normal;
    float3 viewVec;
    float3 lightVec;
    float3 texCoords;
};

struct UniformBufferObject {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4x4 invModelView;
    float4x4 invTransposeModel;
    float3 worldCameraPosition;
};

vertex VertexOut vertFunc(VertexIn vert [[stage_in]],
                          unsigned int vid [[vertex_id]],
                          const device UniformBufferObject &ubo [[buffer(1)]]) { // Change to buffer(1)?
    VertexOut out;

    out.mtlPosition = ubo.proj * ubo.view * ubo.model * float4(vert.position, 1.0);

    float4 worldCameraPosition = float4(ubo.worldCameraPosition, 0.0f);
    float4 worldPosition = ubo.model * float4(vert.position, 1.0);
    float4 worldNormal = float4(vert.normal, 0.0f);//ubo.invTransposeModel * float4(vert.normal, 0.0);
    float4 worldEyeDirection = normalize(worldPosition - worldCameraPosition);
    out.texCoords = reflect(worldEyeDirection, worldNormal).xyz;

    out.pos = (ubo.view * ubo.model * float4(vert.position, 1.0)).xyz;
    out.normal = vert.normal.xyz;

    float3 lightPos = float3(0.0f, -5.0f, -5.0f);
    out.lightVec = lightPos.xyz - out.pos.xyz;
    out.viewVec = -out.pos.xyz;

    return out;
}

fragment float4 fragFunc(VertexOut inFrag [[stage_in]], texturecube<float>
cubemapTexture [[texture(0)]], sampler cubemapSampler [[sampler(0)]],
const device UniformBufferObject &ubo [[buffer(2)]]) {

    // float3 cI = normalize(inFrag.pos);
    // float3 cR = reflect(cI, normalize(inFrag.normal));
    // cR.y *= -1.0;

    // cR = (ubo.invModelView * float4(cR, 0.0f)).xyz;
    // cR.y *= -1.0;
    // cR.x *= -1.0;

    inFrag.normal.y *= -1.0;
    float4 color = pow(cubemapTexture.sample(cubemapSampler, inFrag.normal), 2.2);

    // float3 N = normalize(inFrag.normal);
    // float3 L = normalize(inFrag.lightVec);
    // float3 V = normalize(inFrag.viewVec);
    // float3 R = reflect(-L, N);
    // float3 ambient = float3(0.5) * color.rgb;
    // float3 diffuse = max(dot(N, L), 0.0) * float3(1.0);
    // float3 specular = pow(max(dot(R, V), 0.0), 16.0) * float3(0.5);

    // return pow(float4(ambient + diffuse * color.rgb + specular, 1.0), 1.0/2.2);
    return pow(color, 1.0/2.2);
}
