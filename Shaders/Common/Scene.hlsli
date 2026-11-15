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

    float4 g_misc;             // x - 1: кодировать гамму в шейдере, y - экспозиция
};

float4 MulCols(float4 cols[4], float4 v)
{
    return cols[0] * v.x + cols[1] * v.y + cols[2] * v.z + cols[3] * v.w;
}

// ACES (Narkowicz) + sRGB-кодирование. Все расчёты в шейдерах - в линейном
// пространстве, как и в Godot.
float3 Tonemap(float3 color)
{
    color *= g_misc.y;

    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    color = saturate((color * (a * color + b)) / (color * (c * color + d) + e));

    if (g_misc.x > 0.5)
    {
        color = pow(color, 1.0 / 2.2);
    }

    return color;
}

#endif
