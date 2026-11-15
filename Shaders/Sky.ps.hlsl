/*
 * Небо: пиксельный шейдер. Порт Godot `stylized_sky.gdshader` (GDQuest) -> см. Common/Sky.hlsli.
 */

#include "Common/Scene.hlsli"
#include "Common/Sky.hlsli"

struct PSInput
{
    float4 position : SV_Position;
    float2 ndc      : TEXCOORD0;
};

float4 main(PSInput input) : SV_Target0
{
    // Точка на дальней плоскости -> в мир -> направление от камеры.
    float4 farPoint = MulCols(g_invViewProj, float4(input.ndc, 1.0, 1.0));
    float3 eye = normalize(farPoint.xyz / farPoint.w - g_cameraPos.xyz);

    // Отладка (--sky-debug=N, значение лежит в g_cloudsSamples.w):
    //   1 - текстура формы облаков, 2 - текстура шума (как есть, без mip),
    //   3 - плотность облаков (r) и затенённость (g), 4 - выбранный mip / 8.
    int debugMode = (int)(g_cloudsSamples.w + 0.5);

    if (debugMode == 1)
    {
        float v = g_cloudShapeTex.SampleLevel(g_cloudShapeSampler, input.ndc * 0.5 + 0.5, 0).r;
        return float4(v, v, v, 1.0);
    }

    if (debugMode == 2)
    {
        float v = g_cloudNoiseTex.SampleLevel(g_cloudNoiseSampler, input.ndc * 0.5 + 0.5, 0).r;
        return float4(v, v, v, 1.0);
    }

    if (debugMode == 3)
    {
        float2 xy = eye.xz / max(saturate(eye.y), 1e-4);
        float2 clouds = CloudRayMarch(xy, g_lightDir.xyz, ddx(xy) * 2.0, ddy(xy) * 2.0);
        return float4(clouds.x, clouds.y, 0.0, 1.0);
    }

    float3 color = SkyFull(eye);

    return float4(Tonemap(color), 1.0);
}
