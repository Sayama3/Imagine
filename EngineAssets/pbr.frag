#version 450 core

#extension GL_GOOGLE_include_directive: require
#include "pbr_structures.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 vColor;
layout (location = 4) in mat3 tangentBasis;

layout (location = 0) out vec4 FragColor;

/*
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
    vec3 reflectDir = normalize(reflect(lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor * lightIntensity;

    return max(((ambient * ao) + diffuse + specular) * albedo, vec3(0));
}

vec3 AddDirectionalLight(Light light)
{
    vec3 lightColor = light.color.rgb;
    vec3 lightDir = normalize(-light.direction.xyz);
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
    vec3 lightTarget = normalize(-light.direction.xyz);
    float cutOff = light.direction.w;

    if (dot(lightDir, -lightTarget) >= cos(cutOff))
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

    vec3 N = normalize(2.0 * texture(normalTexture, texcoord).rgb - 1.0);
    N = normalize(tangentBasis * N);
    N = normalize(vnormal + N*0.25);
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
*/


// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha   = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// layout (location = 0) in vec3 position; // Just a reminder
// layout (location = 1) in vec3 normal; // Just a reminder
// layout (location = 2) in vec2 texcoord; // Just a reminder
// layout (location = 3) in vec4 vColor; // Just a reminder
// layout (location = 4) in mat3 tangentBasis; // Just a reminder
// layout (location = 0) out vec4 FragColor; // Just a reminder


float alpha;
vec3 albedo;
vec3 emissive;
//vec3 normal;
float metallic;
float roughness;
float ao;

void main() {

    vec4 texAlbedo = texture(albedoTexture, texcoord);

    alpha      = texAlbedo.a;
//    albedo     = pow(texAlbedo.rgb, vec3(2.2)) * materialData.TintColor.rgb * vColor.rgb;
//    emissive   = pow(texture(emissiveTexture, texcoord).rgb, vec3(2.2)) * materialData.Emissive.rgb;
    albedo     = texAlbedo.rgb * materialData.TintColor.rgb * vColor.rgb;
    albedo     = max(albedo, 0.1);
    emissive   = texture(emissiveTexture, texcoord).rgb * materialData.Emissive.rgb;

    metallic   = length(texture(metalnessTexture, texcoord) * vec4(materialData.MetalFactor, 0));
    roughness  = length(texture(roughnessTexture, texcoord) * vec4(materialData.RoughFactor, 0));
    ao         = texture(ambientOcclusionTexture, texcoord).r;

    // Outgoing light direction (vector from world-space fragment position to the "eye").
    vec3 Lo = normalize(sceneData.camPos.xyz - position);

    // Get current fragment's normal and transform to world space.
    vec3 N = normalize(2.0 * texture(normalTexture, texcoord).rgb - 1.0);
    N = normalize(tangentBasis * N);
    N = normalize(vnormal + N*0.25);

    // Angle between surface normal and outgoing light direction.
    float cosLo = max(0.0, dot(N, Lo));

    // Specular reflection vector.
    vec3 Lr = 2.0 * cosLo * N - Lo;

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    vec3 F0 = mix(Fdielectric, albedo, metallic);

    // Direct lighting calculation for analytical lights.
    vec3 directLighting = vec3(0);
//    if(lightData.lightCount <= 0) {
//        directLighting = albedo * ao;
//    }

    for(int i=0; i<min(lightData.lightCount,MaxLight); ++i)
    {
        Light light = lightData.lights[i];
        vec3 Li;

        if(light.type == LIGHT_POINT)
        {
            Li = normalize(position - light.position.xyz);
        }
        else
        {
            Li = -normalize(light.direction.xyz);
        }

        vec3 Lradiance = (light.color.rgb * light.color.a);


        // Half-vector between Li and Lo.
        vec3 Lh = normalize(Li + Lo);

        // Calculate angles between surface normal and various light vectors.
        float cosLi = max(0.0, dot(N, Li));
        float cosLh = max(0.0, dot(N, Lh));

        // Calculate Fresnel term for direct lighting.
        vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
        // Calculate normal distribution for specular BRDF.
        float D = cosLh;//ndfGGX(cosLh, roughness);
        // Calculate geometric attenuation for specular BRDF.
        float G = gaSchlickGGX(cosLi, cosLo, roughness);

        // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
        // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
        // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
        vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

        // Lambert diffuse BRDF.
        // We don't scale by 1/PI for lighting & material units to be more convenient.
        // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
        vec3 diffuseBRDF = kd * albedo;

        // Cook-Torrance specular microfacet BRDF.
        vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

        // Total contribution for this light.
//        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
        directLighting += (diffuseBRDF) * Lradiance * cosLi;
    }

    // Ambient lighting (IBL).
    vec3 ambientLighting;
    {
        // Sample diffuse irradiance at normal direction.
//        vec3 irradiance = texture(irradianceTexture, N).rgb;
        vec3 irradiance = vec3(1);

        // Calculate Fresnel term for ambient lighting.
        // Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
        // use cosLo instead of angle with light's half-vector (cosLh above).
        // See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
        vec3 F = fresnelSchlick(F0, cosLo);

        // Get diffuse contribution factor (as with direct lighting).
        vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

        // Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
        vec3 diffuseIBL = kd * albedo * irradiance;

//        // Sample pre-filtered specular reflection environment at correct mipmap level.
//        int specularTextureLevels = textureQueryLevels(specularTexture);
//        vec3 specularIrradiance = textureLod(specularTexture, Lr, roughness * specularTextureLevels).rgb;
//
//        // Split-sum approximation factors for Cook-Torrance specular BRDF.
//        vec2 specularBRDF = texture(specularBRDF_LUT, vec2(cosLo, roughness)).rg;
//
//        // Total specular IBL contribution.
//        vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
//
////         Total ambient lighting contribution.
//        ambientLighting = diffuseIBL + specularIBL;
        ambientLighting = diffuseIBL;//max(diffuseIBL, (0.2 * albedo));
    }

    // Final fragment color.
    vec3 finalColor = directLighting + ambientLighting;
    vec3 minAlbedo = albedo * ao * 0.1;
    vec3 maxAlbedo = min(1.5 * albedo * ao, vec3(1));
    finalColor = min(max(finalColor, minAlbedo), maxAlbedo);
    FragColor = vec4(finalColor, alpha);
//    FragColor = vec4(N, 1.0);
}
