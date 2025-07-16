#version 450 core

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "pbr_structures.glsl"


layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 texcoord;
layout (location = 3) out vec4 vColor;
layout (location = 4) out mat3 tangentBasis;

layout(buffer_reference, std430) readonly buffer VertexBuffer{
    Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
    mat4 worldMatrix;
    mat4 normalMatrix;
    VertexBuffer vertexBuffer;
} PushConstants;

void main()
{
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    vec4 position4 = vec4(v.position, 1.0f);
    position4 = PushConstants.worldMatrix * position4;
    position = vec3(position4);
    texcoord = vec2(v.uv_x, v.uv_y);

    gl_Position =  sceneData.viewproj * position4;

    vec4 normal4 = PushConstants.normalMatrix * vec4(v.normal.xyz, 0);
//    normal4 /= normal4.w;
    normal = normalize(normal4.xyz);
    tangentBasis = mat3(PushConstants.worldMatrix) * mat3(normalize(v.tangent), normalize(v.bitangent), normalize(v.normal));
    vColor = v.color;
}