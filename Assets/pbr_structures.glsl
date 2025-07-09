struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 tangent;
    vec4 bitangent;
    vec4 color;
};

struct Light {
    vec4 color;
    vec4 direction;
    vec3 position;
    int type;
};

layout(set = 0, binding = 0) uniform  SceneData{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec4 ambientColor;
    vec4 sunlightDirection; //w for sun power
    vec4 sunlightColor;
} sceneData;

layout(set = 0, binding = 1) readonly buffer LightData {
    Light lights[32];
    int lightCount;
} lightData;

layout(set = 1, binding = 0) uniform GLTFMaterialData{
    vec4 TintColor;
    vec4 Emissive;
    float MetalFactor;
    float RoughFactor;
} materialData;

layout(set = 1, binding = 1) uniform sampler2D colorTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;
layout(set = 1, binding = 3) uniform sampler2D emissiveTexture;
layout(set = 1, binding = 4) uniform sampler2D metalTexture;
layout(set = 1, binding = 5) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 6) uniform sampler2D ambientOcclusionTexture;