#ifndef EMPIRE_SCENE_HLSLI
#define EMPIRE_SCENE_HLSLI

/*
 * Общие данные кадра (set 0, binding 0). Раскладка ДОЛЖНА совпадать
 * со структурой SceneUniformData в Tests/Rendering/SceneMain.cpp.
 *
 * Матрицы лежат как 4 столбца float4 (column-major, как в GLSL/Vulkan).
 * Умножаем через MulCols() - так не зависим от того, как dxc трактует
 * float4x4 в uniform-буфере.
 */

[[vk::binding(0, 0)]]
cbuffer SceneParams
{
    float4 g_viewProj[4];
    float4 g_invViewProj[4];

    float4 g_cameraPos;        // xyz - позиция, w - время (сек)
    float4 g_lightDir;         // xyz - НАПРАВЛЕНИЕ К свету (солнцу/луне)
    float4 g_lightColor;       // rgb - цвет света, уже с интенсивностью

    float4 g_topColor;
    float4 g_bottomColor;
    float4 g_sunScatter;
    float4 g_cloudsLightColor;

    float4 g_cloudsParams;     // x density, y scale, z smoothness, w shadow intensity
    float4 g_cloudsSamples;    // x samples, y shadow samples, z high clouds density
    float4 g_skyParams;        // x stars, y shooting stars, z astro intensity, w astro scale
    float4 g_astroTint;
    float4 g_shootingTint;

    float4 g_misc;             // x - 1: кодировать гамму в шейдере, y - экспозиция,
                               // z, w - края радиального градиента диска солнца/луны (astro)
};

// Текстуры неба (set 0, binding 1..4). Это замена sampler2D из Godot-шейдера.
// combinedImageSampler: текстура и сэмплер делят один binding (VkDescriptor
// COMBINED_IMAGE_SAMPLER). Читаем только .r (R8).
[[vk::combinedImageSampler]][[vk::binding(1, 0)]] Texture2D g_cloudShapeTex;
[[vk::combinedImageSampler]][[vk::binding(1, 0)]] SamplerState g_cloudShapeSampler;

[[vk::combinedImageSampler]][[vk::binding(2, 0)]] Texture2D g_cloudNoiseTex;
[[vk::combinedImageSampler]][[vk::binding(2, 0)]] SamplerState g_cloudNoiseSampler;

[[vk::combinedImageSampler]][[vk::binding(3, 0)]] Texture2D g_highCloudsTex;
[[vk::combinedImageSampler]][[vk::binding(3, 0)]] SamplerState g_highCloudsSampler;

[[vk::combinedImageSampler]][[vk::binding(4, 0)]] Texture2D g_shootingStarTex;
[[vk::combinedImageSampler]][[vk::binding(4, 0)]] SamplerState g_shootingStarSampler;

float4 MulCols(float4 cols[4], float4 v)
{
    return cols[0] * v.x + cols[1] * v.y + cols[2] * v.z + cols[3] * v.w;
}

// Filmic - как tonemap_mode = Filmic в Godot 4 (так сделаны превью gdquest),
// затем sRGB-кодирование. Все расчёты в шейдерах - в линейном пространстве.
float3 TonemapFilmic(float3 color, float white)
{
    const float exposureBias = 2.0;
    const float A = 0.22 * exposureBias * exposureBias;
    const float B = 0.30 * exposureBias;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.01;
    const float F = 0.30;

    float3 mapped = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float whiteMapped = ((white * (A * white + C * B) + D * E) / (white * (A * white + B) + D * F)) - E / F;

    return mapped / whiteMapped;
}

float3 LinearToSrgb(float3 c)
{
    c = saturate(c);
    float3 low = c * 12.92;
    float3 high = 1.055 * pow(c, 1.0 / 2.4) - 0.055;

    return lerp(high, low, step(c, 0.0031308));
}

float3 Tonemap(float3 color)
{
    color = TonemapFilmic(max(color * g_misc.y, 0.0), 1.0);

    // Если swapchain в SRGB-формате, кодирование делает железо.
    return g_misc.x > 0.5 ? LinearToSrgb(color) : saturate(color);
}

#endif
