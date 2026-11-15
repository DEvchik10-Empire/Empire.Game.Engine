/*
 * Небо: пиксельный шейдер. Порт Godot `shader_type sky` -> см. Common/Sky.hlsli.
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

    float3 color = SkyFull(eye);

    return float4(Tonemap(color), 1.0);
}
