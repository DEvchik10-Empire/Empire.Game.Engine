#ifndef EMPIRE_SKY_HLSLI
#define EMPIRE_SKY_HLSLI

/*
 * Небо. Порт Godot-шейдера `shader_type sky` на HLSL.
 *
 * Соответствие Godot -> Empire:
 *   EYEDIR           -> eye (нормализованное направление взгляда в мире)
 *   LIGHT0_DIRECTION -> g_lightDir.xyz
 *   TIME             -> g_cameraPos.w
 *   SKY_COORDS.y     -> acos(eye.y) / PI   (0 - зенит, 0.5 - горизонт)
 *   half-res pass    -> убран: облака считаются в том же проходе
 *                       (композит mix(sky, clouds.rgb, clouds.a) тот же)
 *   текстуры         -> процедурные шумы (см. Noise.hlsli)
 *
 * Сверх оригинала (чтобы небо выглядело как небо, а не как градиент):
 *   - нелинейный градиент и свечение заката в сторону солнца;
 *   - ореол и диск солнца/луны (вместо текстуры astro);
 *   - земля/дымка под горизонтом;
 *   - облака с мягкой тенью и серебристой каймой против солнца.
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

// Вместо shooting_star_sampler: яркая "голова" и затухающий хвост.
float ShootingStarTexture(float2 uv)
{
    float x = uv.x - 0.5;
    float tail = pow(saturate(uv.y), 4.0);

    return tail * exp(-abs(x) * 45.0);
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
            saturate(shootingUv.y * (1.0 - shootingUv.y))) * ceil(max(shootingDir.z, 0.0));

        shootingStar = saturate(
            shootingStar +
            ShootingStarTexture(shootingUv) * sin(progress * PI) * shootingMask * randValue);
    }

    return saturate(shootingStar);
}

// ---------------------------------------------------------------------
// Облака (ray-march как в Godot, параметры подобраны под процедурный шум).
// ---------------------------------------------------------------------

// Кривая высоты (cloud_curves): облако тонкое снизу и сверху, плотное в середине.
float HeightCurve(float progress)
{
    // Godot: progress идёт 3..4, текстура с repeat -> берётся дробная часть.
    return 0.55 + 0.9 * sin(PI * frac(progress));
}

float CloudDensity(float3 p, float progress)
{
    float time = g_cameraPos.w;

    float tOffset = time * 0.001;
    float tOffsetSmall = time * -0.005;

    float cloudsScale = g_cloudsParams.y;
    float smoothness = g_cloudsParams.z;

    float noise = Fbm((p.xy * 4.0 + tOffsetSmall) * 7.0, 2) * 0.14 + 0.86;

    float shape = Fbm((p.xy + tOffset) * cloudsScale * 5.0, 5);
    shape = saturate((shape - 0.5) * 2.2 + 0.5);

    float baseDensity = 1.0 - g_cloudsParams.x;

    // HLSL: smoothstep требует edge0 < edge1, smoothness > 0 это гарантирует.
    return smoothstep(
        baseDensity - smoothness,
        baseDensity + smoothness,
        shape * noise * HeightCurve(progress));
}

// x - плотность, y - "затенённость" (сколько облаков между точкой и солнцем).
float2 CloudRayMarch(float3 direction, float3 sunDirection)
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

        density += CloudDensity(samplePoint, progress);

        float pointLight = 0.0;

        [loop]
        for (int f = 0; f < shadowSamples; f++)
        {
            float shadowProgress = float(f) / float(shadowSamples);
            float3 shadowOffset = sunDirection * height * 1.5 * shadowProgress;

            pointLight += CloudDensity(samplePoint + shadowOffset, progress);
        }

        light += pointLight;
    }

    // Делим на 3: 16 слоёв суммируются, без нормировки облака всегда сплошные.
    return float2(saturate(density / 3.0), light / float(shadowSamples * samples));
}

// ---------------------------------------------------------------------
// Небо без облаков: градиент, закат, ореол и диск солнца/луны, звёзды, земля.
// Дёшево, поэтому годится и для отражений на объектах.
// ---------------------------------------------------------------------
float3 SkyBase(float3 eye, bool withStars)
{
    float time = g_cameraPos.w;
    float3 L = normalize(g_lightDir.xyz);

    float y = eye.y;
    float horizonMask = abs(y);

    float3 top = g_topColor.rgb;
    float3 bottom = g_bottomColor.rgb;

    // Градиент поднимается от горизонта быстрее, чем линейно.
    float3 color = lerp(bottom, top, pow(saturate(y), 0.5));

    // Закат: свечение вдоль горизонта, сильнее в сторону солнца.
    float2 eyeXZ = eye.xz / max(length(eye.xz), 1e-3);
    float2 sunXZ = L.xz / max(length(L.xz), 1e-3);
    float az = saturate(dot(eyeXZ, sunXZ) * 0.5 + 0.5);

    float glowHorizon = pow(1.0 - saturate(horizonMask), 3.0) * (0.35 + 0.65 * az * az);
    color += g_sunScatter.rgb * glowHorizon;

    // Ореол и диск солнца/луны.
    float sunDot = saturate(dot(eye, L));
    float astro = g_skyParams.z;

    float halo = pow(sunDot, 64.0) * 0.7 + pow(sunDot, 8.0) * 0.18;
    color += g_astroTint.rgb * astro * halo;

    float3 b = DirectionBasis(L, eye);
    float2 uv = -(b.xy / (abs(b.z) < 1e-4 ? 1e-4 : b.z)) * g_skyParams.w + 0.5;
    float r = length(uv - 0.5) * 2.0;
    float disc = (1.0 - smoothstep(0.10, 0.12, r)) * (b.z > 0.0 ? 1.0 : 0.0);

    // Лёгкие "кратеры", заметны на луне.
    float surface = 0.88 + 0.12 * Fbm(uv * 28.0, 3);
    color += g_astroTint.rgb * astro * disc * 2.5 * surface;

    // Звёзды (только над горизонтом, у горизонта гаснут).
    if (withStars && g_skyParams.x > 0.0)
    {
        float2 stars = Voronoi(eye * 25.0).xz;
        float edge = 0.025 + ((1.0 + sin(time + stars.y)) / 2.0) * 0.05;

        // Godot smoothstep(edge, 0, x) == 1 - smoothstep(0, edge, x)
        color += (1.0 - smoothstep(0.0, edge, stars.x)) * g_skyParams.x * saturate(y * 4.0);
    }

    // Земля: тёмная дымка, плавно темнеющая вниз.
    float3 haze = bottom + g_sunScatter.rgb * 0.6;
    float groundT = saturate(-y * 1.6);
    float3 ground = haze * 0.4 * (1.0 - groundT) + bottom * 0.08 * groundT;

    return lerp(ground, color, smoothstep(-0.015, 0.01, y));
}

float3 SkyFull(float3 eye)
{
    float time = g_cameraPos.w;
    float3 L = normalize(g_lightDir.xyz);

    float3 skyColor = SkyBase(eye, true);

    // Падающие звёзды.
    if (g_skyParams.y > 0.0)
    {
        skyColor += GetShootingStar(eye) * g_skyParams.y * g_shootingTint.rgb * saturate(eye.y * 4.0);
    }

    // Высокие облака (тонкая дымка).
    float highCloudsDensity = g_cloudsSamples.z;

    if (highCloudsDensity > 0.0 && eye.y > 0.0)
    {
        float2 highUv = (eye.xz / max(eye.y, 0.05)) * 0.25 + time * 0.001;
        float highMask = Fbm(highUv * 3.0, 4);

        skyColor = lerp(
            skyColor,
            g_cloudsLightColor.rgb,
            smoothstep(0.35, 1.0, highMask) * smoothstep(0.0, 0.3, eye.y) * highCloudsDensity * 0.5);
    }

    // Основные облака. Под горизонтом и прямо у него их нет.
    if (eye.y > 0.002)
    {
        float3 cloudsDirection = float3(eye.xz / saturate(eye.y), 1.0);
        float2 clouds = CloudRayMarch(cloudsDirection, L);

        float3 top = g_topColor.rgb;
        float3 lightColor = g_cloudsLightColor.rgb;

        // В Godot тень - bottom_color. Здесь она мягче: из цвета света и неба,
        // иначе на закате облака становятся оранжевыми кляксами.
        float3 shadowColor = lightColor * 0.45 + top * 0.35;

        float3 cloudsColor = lerp(shadowColor, lightColor, exp(-clouds.y * g_cloudsParams.w));

        // Серебристая кайма у краёв облаков со стороны солнца.
        float sunDot = saturate(dot(eye, L));
        cloudsColor += g_astroTint.rgb * 0.25 * pow(sunDot, 6.0) * exp(-clouds.x * 1.5);

        float cloudsAlpha = (1.0 - exp(-clouds.x * 10.0)) * smoothstep(0.0, 0.18, eye.y);

        skyColor = lerp(skyColor, cloudsColor, cloudsAlpha);
    }

    return skyColor;
}

#endif
