struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 tangent;
    vec4 bitangent;
    vec4 color;
};

#define LIGHT_POINT 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2

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
    vec4 camPos;
    vec4 ambientColor;
    vec4 sunlightDirection; //w for sun power
    vec4 sunlightColor;
} sceneData;

#define MaxLight 32

layout(set = 0, binding = 1) readonly buffer LightData {
    Light lights[MaxLight];
    int lightCount;
} lightData;

layout(set = 1, binding = 0) uniform GLTFMaterialData{
    vec4 TintColor;
    vec4 Emissive;
    vec3 MetalFactor;
    vec3 RoughFactor;
} materialData;

layout(set = 1, binding = 1) uniform sampler2D albedoTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;
layout(set = 1, binding = 3) uniform sampler2D emissiveTexture;
layout(set = 1, binding = 4) uniform sampler2D metalnessTexture;
layout(set = 1, binding = 5) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 6) uniform sampler2D ambientOcclusionTexture;

const float PI = 3.141592;
const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);