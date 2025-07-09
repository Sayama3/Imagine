#version 450 core

#extension GL_GOOGLE_include_directive: require
#include "pbr_structures.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in mat3 tangentBasis;

layout (location = 0) out vec4 FragColor;


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 getNormalFromNormalMap() {
    // Get current fragment's normal and transform to world space.
    vec3 N = normalize(2.0 * texture(normalTexture, texcoord).rgb - 1.0);
    N = normalize(tangentBasis * N);
    return N;
}

void main()
{
    vec3 albedo     = pow(texture(albedoTexture, texcoord).rgb, vec3(2.2)) * materialData.TintColor.rgb;
    vec3 emissive   = pow(texture(emissiveTexture, texcoord).rgb, vec3(2.2)) * materialData.Emissive.rgb;
    vec3 normal     = getNormalFromNormalMap();
    float metallic  = length(texture(metalnessTexture, texcoord) * vec4(materialData.MetalFactor, 0));
    float roughness = length(texture(roughnessTexture, texcoord) * vec4(materialData.RoughFactor, 0));
    float ao        = texture(ambientOcclusionTexture, texcoord).r;

    vec3 N = normal;
    vec3 V = normalize(sceneData.camPos.xyz - position);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightData.lightCount; ++i)
    {
        Light light = lightData.lights[i];
        vec3 radiance;
        vec3 L;
        if(light.type == LIGHT_POINT)
        {
            L = normalize(light.position - position);
            vec3 lightColor = light.color.rgb;
            vec3 lightPos = light.position.xyz;
            float dst = distance(lightPos, position);
            float range = max(length(light.direction.xyz), Epsilon);
            float lightIntensity = max(light.color.w, 0);
            lightIntensity =  lightIntensity / ((1) + (0.09) * dst + (0.032) * (dst * dst));
            lightIntensity = lightIntensity * clamp((1-(dst / range)) * 10, 0., 1.);
            radiance = light.color.rgb * lightIntensity;
        } else if(light.type == LIGHT_DIRECTIONAL) {
            L = -normalize(light.direction.xyz);
            float intensity = max(light.color.w, 0);
            radiance = light.color.rgb * intensity;
        } else if(light.type == LIGHT_SPOT) {
            float range = max(length(light.direction.xyz), Epsilon);
            float cutOff = light.direction.w;
            vec3 lightDir = -normalize(light.direction.xyz);
            vec3 lightPos = light.position.xyz;
            L = normalize(lightPos - position);
            if(dot(L , lightDir) <= cos(cutOff))
            {
                continue;
            }

            // calculate per-light radiance
            float dst = distance(lightPos, position);
            float lightIntensity = max(light.color.w, 0);
            lightIntensity =  lightIntensity / ((1) + (0.09) * dst + (0.032) * (dst * dst));
            lightIntensity = lightIntensity * clamp((1-(dst / range)) * 10, 0., 1.);
            radiance = light.color.rgb * lightIntensity;
        }

        vec3 H = normalize(V + L);
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo + emissive;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
