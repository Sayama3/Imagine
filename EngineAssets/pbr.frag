#version 450 core

#extension GL_GOOGLE_include_directive: require
#include "pbr_structures.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 vColor;
layout (location = 3) in mat3 tangentBasis;

layout (location = 0) out vec4 FragColor;

float alpha;
vec3 albedo;
vec3 emissive;
vec3 normal;
float metallic;
float roughness;
float ao;

vec3 CalculateLighting(vec3 lightColor, vec3 lightDir, float lightIntensity)
{
    // ambient
    float ambientStrength = 0.05 * lightIntensity;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightIntensity;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(sceneData.camPos.xyz - position);
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor * lightIntensity;

    return max(((ambient * ao) + diffuse + specular) * albedo, vec3(0));
}

vec3 AddDirectionalLight(Light light)
{
    vec3 lightColor = light.color.rgb;
    vec3 lightDir = normalize(light.direction.xyz);
    float lightIntensity = light.color.a;

    return CalculateLighting(lightColor, lightDir, lightIntensity);
}

vec3 AddPointLight(Light light)
{
    vec3 lightColor = light.color.rgb;
    vec3 lightPos = light.position.xyz;
    vec3 lightDir = normalize(lightPos - position);
    float distance = distance(position, lightPos);
    float lightIntensity = light.color.a;
    float range = length(light.direction.xyz);
    lightIntensity = lightIntensity * max(sign(dot(normal, lightDir)), 0);
    lightIntensity =  lightIntensity / ((1) + (0.09) * distance + (0.032) * (distance * distance));
    lightIntensity = lightIntensity * clamp((1-(distance / range)) * 10, 0., 1.);

    return CalculateLighting(lightColor, lightDir, lightIntensity);
}

vec3 AddSpotLight(Light light)
{
    vec3 lightColor = light.color.rgb;
    vec3 lightPos = light.position.xyz;
    vec3 lightDir = normalize(lightPos - position);
    float lightRange = length(light.direction.xyz);
    vec3 lightTarget = normalize(light.direction.xyz);
    float cutOff = light.direction.w;

    if (dot(normalize(lightPos - position), lightTarget) <= cos(cutOff))
    {
        return vec3(0);
    }

    float distance = distance(position, lightPos);
    float lightIntensity = light.color.a;
    lightIntensity = lightIntensity * max(sign(dot(normal, lightDir)), 0);
    lightIntensity =  lightIntensity / ((1) + (0.09) * distance + (0.032) * (distance * distance));
    lightIntensity = lightIntensity * clamp((1-(distance / lightRange)) * 10, 0., 1.);

    return CalculateLighting(lightColor, lightDir, lightIntensity);
}

vec3 getNormalFromNormalMap() {
    // Get current fragment's normal and transform to world space.
    vec3 N = normalize(2.0 * texture(normalTexture, texcoord).rgb - 1.0);
    N = normalize(tangentBasis * N);
    return N;
}

void main()
{
    vec4 texAlbedo = texture(albedoTexture, texcoord);

    alpha      = texAlbedo.a;
    albedo     = pow(texAlbedo.rgb, vec3(2.2)) * materialData.TintColor.rgb * vColor.rgb;
    emissive   = pow(texture(emissiveTexture, texcoord).rgb, vec3(2.2)) * materialData.Emissive.rgb;
    normal     = getNormalFromNormalMap();
//    normal     = tangentBasis[2];
    metallic   = length(texture(metalnessTexture, texcoord) * vec4(materialData.MetalFactor, 0));
    roughness  = length(texture(roughnessTexture, texcoord) * vec4(materialData.RoughFactor, 0));
    ao         = texture(ambientOcclusionTexture, texcoord).r;

    vec3 ligthsColor = vec3(0);
    for (int i = 0; i < lightData.lightCount; ++i)
    {
        Light light = lightData.lights[i];
        if (light.type == LIGHT_POINT) {
            ligthsColor += AddPointLight(light);
        } else if (light.type == LIGHT_DIRECTIONAL) {
            ligthsColor += AddDirectionalLight(light);
        } else if (light.type == LIGHT_SPOT) {
            ligthsColor += AddSpotLight(light);
        }
    }

    if (lightData.lightCount == 0) {
        ligthsColor = albedo * ao;
    }

    vec3 ambient = vec3(0.1) * albedo * ao;
    vec3 color = ambient + ligthsColor + emissive;

    //    color = color / (color + vec3(1.0));
    //    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, alpha);
    // FragColor = vec4(radiance, 1.0);
    // FragColor = vec4(albedo, 1.0);
    //    FragColor = vec4(emissive, 1.0);
//     FragColor = vec4(normal+1.0 / 2.0, 1.0);
}
