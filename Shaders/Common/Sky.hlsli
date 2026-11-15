#ifndef EMPIRE_SKY_HLSLI
#define EMPIRE_SKY_HLSLI

/*
 * Небо. Порт Godot-шейдера `stylized_sky.gdshader` (GDQuest, MIT) на HLSL.
 * Логика оригинала сохранена; см. Shaders/README.md про отличия.
 *
 * Соответствие Godot -> Empire:
 *   EYEDIR           -> eye (нормализованное направление взгляда в мире)
 *   LIGHT0_DIRECTION -> g_lightDir.xyz (направление К солнцу/луне)
 *   TIME             -> g_cameraPos.w
 *   SKY_COORDS.y     -> acos(eye.y) / PI   (0 - зенит, 0.5 - горизонт)
 *   half-res pass    -> убран: облака считаются в том же проходе,
 *                       композит mix(sky, clouds.rgb, clouds.a) тот же
 *   sampler2D        -> Texture2D из Scene.hlsli (шумы генерируются в движке)
 *   cloud_curves     -> HeightCurve() ниже
 *   astro_sampler    -> радиальный градиент, считается формулой
 */

#include "Scene.hlsli"
#include "Noise.hlsli"

// ---------------------------------------------------------------------
// Базис, повёрнутый в сторону direction.
// Godot: direction_to_matrix(direction) * v
// mat3 из векторов-столбцов даёт строки (x_axis, y_axis, direction),
// то есть результат = (dot(x, v), dot(y, v), dot(dir, v)).
// ---------------------------------------------------------------------
float3 DirectionBasis(float3 direction, float3 v)
{
    float3 up = abs(direction.y) > 0.999 ? float3(1.0, 0.0, 0.0) : float3(0.0, 1.0, 0.0);

    float3 xAxis = normalize(cross(up, direction));
    float3 yAxis = normalize(cross(direction, xAxis));

    return float3(dot(xAxis, v), dot(yAxis, v), dot(direction, v));
}

// ---------------------------------------------------------------------
// Падающие звёзды.
// ---------------------------------------------------------------------
float3 RandomDirection(float seed)
{
    float phi = Rand(seed) * PI;
    float costheta = Rand(seed + 100.0) * 2.0 - 1.0;
    float theta = acos(costheta);

    // (theta * sin(phi)) вместо sin(theta) - так в оригинале, оставлено как есть.
    return float3(sin(theta) * cos(phi), theta * sin(phi), cos(theta));
}

float GetShootingStar(float3 eye)
{
    float shootingStar = 0.0;
    float time = g_cameraPos.w;

    for (int i = 0; i < 4; i++)
    {
        float baseRand = Rand(float(i));
        float t = time + baseRand * 2.0;
        float duration = 0.5 + baseRand;
        float seed = floor(t / duration) * duration + baseRand;
        float progress = Mod(t, duration) / duration;
        float randValue = Rand(seed + 100.0);
        float randScale = baseRand * 10.0;

        float a = randValue * 0.8;
        float c = cos(a);
        float s = sin(a);

        // Godot: mat3(vec3(c,-s,0), vec3(s,c,0), vec3(0,0,1)) * eyedir
        float3 rotated = float3(c * eye.x + s * eye.y, -s * eye.x + c * eye.y, eye.z);

        float3 shootingDir = DirectionBasis(RandomDirection(seed), rotated);

        float2 shootingUv =
            ((shootingDir.xy + float2(0.0, progress * 0.4)) * (8.0 + randScale)) + float2(0.5, 0.5);

        float shootingMask = ceil(
            saturate(shootingUv.x * (1.0 - shootingUv.x)) *
            saturate(shootingUv.y * (1.0 - shootingUv.y))) * ceil(shootingDir.z);

        // SampleLevel: внутри цикла/ветвления градиенты недоступны.
        float tex = g_shootingStarTex.SampleLevel(g_shootingStarSampler, shootingUv, 0).r;

        shootingStar = saturate(shootingStar + tex * sin(progress * PI) * shootingMask * randValue);
    }

    return saturate(shootingStar);
}

// ---------------------------------------------------------------------
// Облака.
// ---------------------------------------------------------------------

// cloud_curves из Godot: кривая (0,0) -> (0.1,1) -> (1,0.8).
// Оригинал читает её в точке progress = 3..4, а текстура кривой без repeat:
// координата зажимается к краю, поэтому на деле всегда получается значение
// в x = 1, то есть 0.8. Здесь это сделано явно, с той же кривой.
float HeightCurve(float progress)
{
    float x = saturate(progress);

    return x < 0.1 ? x * 10.0 : 1.0 - (x - 0.1) / 0.9 * 0.2;
}

float CloudDensity(float3 p, float progress, float2 lods)
{
    float time = g_cameraPos.w;

    float tOffset = time * 0.001;
    float tOffsetSmall = time * -0.005;

    float cloudsScale = g_cloudsParams.y;
    float smoothness = g_cloudsParams.z;

    float noise = g_cloudNoiseTex.SampleLevel(g_cloudNoiseSampler, p.xy * 4.0 + tOffsetSmall, lods.y).r * 0.1 + 0.9;
    float cloudsShape = g_cloudShapeTex.SampleLevel(g_cloudShapeSampler, (p.xy + tOffset) * cloudsScale, lods.x).r;
    float heightCurve = HeightCurve(progress);

    float baseDensity = 1.0 - g_cloudsParams.x;

    // HLSL: smoothstep требует edge0 < edge1, smoothness > 0 это гарантирует.
    return smoothstep(
        baseDensity - smoothness,
        baseDensity + smoothness,
        cloudsShape * noise * heightCurve);
}

// x - плотность, y - "затенённость" (сколько облаков между точкой и солнцем).
float2 CloudRayMarch(float3 direction, float3 sunDirection, float2 lods)
{
    float density = 0.0;
    float light = 0.0;

    const float height = 0.03;

    int samples = (int)g_cloudsSamples.x;
    int shadowSamples = (int)g_cloudsSamples.y;

    int loopOffset = samples * 3;

    [loop]
    for (int i = loopOffset; i < samples + loopOffset; i++)
    {
        float progress = float(i) / float(samples);
        float3 samplePoint = direction * height * progress;

        density += CloudDensity(samplePoint, progress, lods);

        float pointLight = 0.0;

        [loop]
        for (int f = 0; f < shadowSamples; f++)
        {
            float shadowProgress = float(f) / float(shadowSamples);
            float3 shadowOffset = sunDirection * height * shadowProgress;

            pointLight += CloudDensity(samplePoint + shadowOffset, progress, lods);
        }

        light += pointLight;
    }

    return float2(saturate(density), light / float(shadowSamples * samples));
}

// ---------------------------------------------------------------------
// Небо без облаков: градиент, закатное свечение, звёзды, солнце/луна.
// Дёшево, поэтому годится и для отражений на объектах.
// ---------------------------------------------------------------------
float3 SkyBase(float3 eye, bool withStars)
{
    float time = g_cameraPos.w;

    float horizonMask = abs(eye.y);

    // Godot: smoothstep(0.5, 0.45, SKY_COORDS.y). В HLSL порядок аргументов
    // менять нельзя, поэтому 1 - smoothstep(0.45, 0.5, y).
    float skyY = acos(clamp(eye.y, -1.0, 1.0)) / PI;
    float bottomMask = 1.0 - smoothstep(0.45, 0.5, skyY);

    // Astro (солнце/луна): радиальный градиент, повёрнутый к LIGHT0_DIRECTION.
    float3 dir = DirectionBasis(g_lightDir.xyz, eye);
    float2 astroUv = (-(dir.xy / (abs(dir.z) < 1e-5 ? 1e-5 : dir.z)) * g_skyParams.w) + float2(0.5, 0.5);

    float astroMask = ceil(
        saturate(astroUv.x * (1.0 - astroUv.x)) *
        saturate(astroUv.y * (1.0 - astroUv.y))) * ceil(max(dir.z, 0.0));

    // GradientTexture2D: заливка radial от (0.5,0.5), t = расстояние / 0.5;
    // alpha линейно падает с 1 до 0 между g_misc.z и g_misc.w.
    float t = length(astroUv - float2(0.5, 0.5)) / 0.5;
    float astroAlpha = saturate((g_misc.w - t) / (g_misc.w - g_misc.z));

    // Цвет неба.
    float3 skyGradient = lerp(g_bottomColor.rgb, g_topColor.rgb, saturate(eye.y));
    float3 sunsetColor = g_sunScatter.rgb * (1.0 - horizonMask);
    float3 skyColor = saturate(skyGradient + sunsetColor);

    // Звёзды.
    if (withStars && g_skyParams.x > 0.0)
    {
        float2 stars = Voronoi(eye * 25.0).xz;

        float edge = 0.025 + ((1.0 + sin(time + stars.y)) / 2.0) * 0.05;

        // Godot: smoothstep(edge, 0.0, x) == 1 - smoothstep(0.0, edge, x)
        skyColor += (1.0 - smoothstep(0.0, edge, stars.x)) * g_skyParams.x;
    }

    // Солнце / луна.
    skyColor = lerp(
        skyColor,
        float3(1.0, 1.0, 1.0) * g_skyParams.z * g_astroTint.rgb,
        astroAlpha * astroMask * bottomMask);

    return skyColor;
}

float3 SkyFull(float3 eye)
{
    float time = g_cameraPos.w;

    float horizonMask = abs(eye.y);
    float skyY = acos(clamp(eye.y, -1.0, 1.0)) / PI;
    float bottomMask = 1.0 - smoothstep(0.45, 0.5, skyY);

    // Уровень mip для облачных текстур. В Godot его выбирает железо
    // (filter_linear_mipmap_anisotropic); внутри циклов градиентов нет,
    // поэтому считаем один раз здесь, в единообразном потоке, по экранным
    // производным координаты облаков (берём большую ось, как при обычной
    // трилинейной фильтрации).
    float2 cloudsXY = eye.xz / max(saturate(eye.y), 1e-4);
    float footprint = max(length(ddx(cloudsXY)), length(ddy(cloudsXY)));
    float uvPerPixel = footprint * 0.03 * 4.0; // 4 - наибольший progress

    // Размеры текстур: cloud_shape 1024 px (cloud_noise 512 px читается с uv * 4).
    float2 lods = float2(
        max(log2(max(uvPerPixel * g_cloudsParams.y * 1024.0, 1e-6)), 0.0),
        max(log2(max(uvPerPixel * 4.0 * 512.0, 1e-6)), 0.0));

    float3 skyColor = SkyBase(eye, true);

    // Падающие звёзды.
    if (g_skyParams.y > 0.0)
    {
        skyColor += GetShootingStar(eye) * g_skyParams.y * g_shootingTint.rgb;
    }

    // Высокие облака.
    float highCloudsDensity = g_cloudsSamples.z;

    if (highCloudsDensity > 0.0)
    {
        float2 highUv = (eye.xz / max(saturate(eye.y), 1e-4)) * 0.25 + time * 0.001;
        float highMask = g_highCloudsTex.SampleLevel(g_highCloudsSampler, highUv, 0).r;

        skyColor = lerp(
            skyColor,
            g_cloudsLightColor.rgb,
            smoothstep(0.0, 1.0, highMask) * horizonMask * bottomMask * highCloudsDensity);
    }

    // Основные облака (в Godot - half-res проход). Ниже горизонта их нет.
    if (eye.y > 0.0)
    {
        float3 cloudsDirection = float3(eye.xz / saturate(eye.y), 1.0);
        float2 clouds = CloudRayMarch(cloudsDirection, g_lightDir.xyz, lods);

        // Тень облака - bottom_color, свет - clouds_light_color (как в оригинале).
        float3 cloudsColor = lerp(
            g_bottomColor.rgb,
            g_cloudsLightColor.rgb,
            exp(-clouds.y * g_cloudsParams.w));

        float cloudsAlpha = 1.0 - exp(-clouds.x * horizonMask * bottomMask * 10.0);

        skyColor = lerp(skyColor, cloudsColor, cloudsAlpha);
    }

    return skyColor;
}

#endif
