/*
 * Модель: вершинный шейдер.
 * Матрица модели приходит через push constants (4 столбца).
 */

#include "Common/Scene.hlsli"

struct PushConstants
{
    float4 model0;
    float4 model1;
    float4 model2;
    float4 model3;
};

[[vk::push_constant]] PushConstants g_push;

struct VSInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
};

struct VSOutput
{
    float4 position  : SV_Position;
    float3 worldPos  : TEXCOORD0;
    float3 worldNrm  : TEXCOORD1;
    float3 localPos  : TEXCOORD2;
    float2 uv        : TEXCOORD3;
};

VSOutput main(VSInput input)
{
    float4 local = float4(input.position, 1.0);

    float4 world =
        g_push.model0 * local.x +
        g_push.model1 * local.y +
        g_push.model2 * local.z +
        g_push.model3 * local.w;

    // Нормаль: считаем, что масштаб у модели равномерный.
    float3 normal =
        g_push.model0.xyz * input.normal.x +
        g_push.model1.xyz * input.normal.y +
        g_push.model2.xyz * input.normal.z;

    VSOutput output;
    output.position = MulCols(g_viewProj, world);
    output.worldPos = world.xyz;
    output.worldNrm = normalize(normal);
    output.localPos = input.position;
    output.uv = input.uv;

    return output;
}
