/*
 * Модель: пиксельный шейдер.
 *
 * Цвет модели циклически меняется со временем + PBR-подобное
 * освещение (GGX, Fresnel, Smith), отражение НЕБА (тот же код неба, что
 * рисуется на фоне) и мягкая подсветка по краю. Свет - солнце или луна,
 * в зависимости от времени суток.
 */

#include "Common/Scene.hlsli"
#include "Common/Sky.hlsli"

// Должно совпадать с PushConstants в Lit.vs.hlsl.
struct PushConstants
{
    float4 model0;
    float4 model1;
    float4 model2;
    float4 model3;

    float4 baseColor;
    float4 material;
};

[[vk::push_constant]] PushConstants g_push;

// Set 1: материал (текстура base color; sRGB -> линейное делает сам GPU).
[[vk::combinedImageSampler]][[vk::binding(0, 1)]] Texture2D g_baseColorTex;
[[vk::combinedImageSampler]][[vk::binding(0, 1)]] SamplerState g_baseColorSampler;

struct PSInput
{
    float4 position  : SV_Position;
    float3 worldPos  : TEXCOORD0;
    float3 worldNrm  : TEXCOORD1;
    float3 localPos  : TEXCOORD2;
    float2 uv        : TEXCOORD3;
};

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;

    return a2 / (PI * d * d);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float gv = NdotV / (NdotV * (1.0 - k) + k);
    float gl = NdotL / (NdotL * (1.0 - k) + k);

    return gv * gl;
}

float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float4 main(PSInput input) : SV_Target0
{
    float time = g_cameraPos.w;

    float3 N = normalize(input.worldNrm);
    float3 V = normalize(g_cameraPos.xyz - input.worldPos);
    float3 L = normalize(g_lightDir.xyz);
    float3 H = normalize(V + L);

    // ---- материал ----
    float3 albedo;
    float metallic;
    float roughness;

    if (g_push.material.x < 0.5)
    {
        // Куб и прочие примитивы: цвет плавно "дышит" - три синусоиды со
        // сдвигом 120 градусов (так же менялся цвет закраски экрана в первом тесте).
        float3 cycle = 0.5 + 0.5 * sin(time + float3(0.0, 2.0944, 4.1888));

        albedo = pow(cycle, 2.2); // sRGB -> линейное
        metallic = 0.2;
        roughness = 0.28;
    }
    else
    {
        // Модель glTF: base color = текстура * множитель материала.
        float4 baseColor = g_baseColorTex.Sample(g_baseColorSampler, input.uv) * g_push.baseColor;

        // alphaMode MASK / BLEND: отсекаем прозрачное.
        if (g_push.material.y >= 0.0 && baseColor.a < g_push.material.y)
        {
            discard;
        }

        albedo = baseColor.rgb;
        metallic = saturate(g_push.material.z);
        roughness = clamp(g_push.material.w, 0.05, 1.0);
    }

    float3 f0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V)) + 1e-4;
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));

    // ---- прямой свет ----
    float3 F = FresnelSchlick(VdotH, f0);
    float D = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);

    float3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-4);
    float3 kd = (1.0 - F) * (1.0 - metallic);

    float3 direct = (kd * albedo / PI + specular) * g_lightColor.rgb * NdotL;

    // ---- свет от неба ----
    float3 ambientSky = lerp(g_bottomColor.rgb, g_topColor.rgb, N.y * 0.5 + 0.5);
    float3 ambient = ambientSky * albedo * (1.0 - metallic) * 0.9;

    float3 R = reflect(-V, N);
    float3 reflection = SkyBase(R, false);

    float3 fEnv = f0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), f0) - f0)
                       * pow(1.0 - NdotV, 5.0);

    float3 environment = reflection * fEnv;

    // ---- подсветка по краю цветом заката ----
    float rim = pow(1.0 - NdotV, 3.5);
    float3 rimColor = (g_sunScatter.rgb + g_cloudsLightColor.rgb * 0.15) * rim * 0.8;

    float3 color = direct + ambient + environment + rimColor;

    return float4(Tonemap(color), 1.0);
}
