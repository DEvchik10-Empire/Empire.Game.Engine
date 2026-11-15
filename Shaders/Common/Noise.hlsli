#ifndef EMPIRE_NOISE_HLSLI
#define EMPIRE_NOISE_HLSLI

/*
 * Процедурные шумы.
 *
 * В оригинальном Godot-шейдере облака, звёзды и т.д. берутся из текстур
 * (cloud_shape_sampler, cloud_noise_sampler, high_clouds_sampler ...).
 * В Empire текстур в RHI пока нет, поэтому вместо них - шумы, посчитанные
 * прямо в шейдере. Картинка того же типа, но без внешних ассетов.
 */

static const float PI = 3.14159265359;

// GLSL mod(): результат всегда в [0, y) даже для отрицательных x.
// (fmod из HLSL берёт знак от x, это не то же самое.)
float Mod(float x, float y)
{
    return x - y * floor(x / y);
}

float Rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float Hash21(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(p.x * p.y);
}

// Inigo Quilez, MIT: https://www.shadertoy.com/view/ldl3Dl
float3 Hash33(float3 x)
{
    x = float3(
        dot(x, float3(127.1, 311.7,  74.7)),
        dot(x, float3(269.5, 183.3, 246.1)),
        dot(x, float3(113.5, 271.9, 124.6)));

    return frac(sin(x) * 43758.5453123);
}

// x = расстояние до ближайшей точки, y = до второй, z = id ближайшей.
float3 Voronoi(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);

    float id = 0.0;
    float2 res = float2(100.0, 100.0);

    [unroll]
    for (int k = -1; k <= 1; k++)
    {
        [unroll]
        for (int j = -1; j <= 1; j++)
        {
            [unroll]
            for (int i = -1; i <= 1; i++)
            {
                float3 b = float3(float(i), float(j), float(k));
                float3 r = b - f + Hash33(p + b);
                float d = dot(r, r);

                if (d < res.x)
                {
                    id = dot(p + b, float3(1.0, 57.0, 113.0));
                    res = float2(d, res.x);
                }
                else if (d < res.y)
                {
                    res.y = d;
                }
            }
        }
    }

    return float3(sqrt(res), abs(id));
}

// Значение-шум, 0..1.
float ValueNoise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);

    float a = Hash21(i);
    float b = Hash21(i + float2(1.0, 0.0));
    float c = Hash21(i + float2(0.0, 1.0));
    float d = Hash21(i + float2(1.0, 1.0));

    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// Фрактальный шум, нормирован в 0..1.
float Fbm(float2 p, int octaves)
{
    float sum = 0.0;
    float amplitude = 0.5;
    float norm = 0.0;

    for (int i = 0; i < octaves; i++)
    {
        sum += ValueNoise(p) * amplitude;
        norm += amplitude;
        p = p * 2.03 + float2(17.1, 9.2);
        amplitude *= 0.5;
    }

    return sum / norm;
}

#endif
