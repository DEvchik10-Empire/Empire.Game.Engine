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

// cloud_curves из Godot: кривая (0,0) -> (0.1,1) -> (1,0.8) по высоте облака.
// Оригинал читает её в точке progress = 3..4. Подгонка по превью GDQuest
// показала: кривая идёт с повтором (progress берётся по модулю 1), тогда низ
// облаков скругляется (0 -> 1 на первых 10%), а верх чуть сужается (до 0.8).
// Именно это даёт округлые «пушистые» облака с плоским дном.
float HeightCurve(float progress)
{
    float x = frac(progress);

    return x < 0.1 ? x * 10.0 : 1.0 - (x - 0.1) / 0.9 * 0.2;
}

// gx, gy - экранные производные точки p.xy (по x и по y экрана), уже с учётом
// высоты и progress. С ними текстуры читаются через SampleGrad: так работает
// анизотропная фильтрация (filter_linear_mipmap_anisotropic в Godot), и край
// облака у горизонта остаётся чётким, а не размазывается в "плиты".
float CloudDensity(float2 p, float progress, float2 gx, float2 gy)
{
    float time = g_cameraPos.w;

    float tOffset = time * 0.001;
    float tOffsetSmall = time * -0.005;

    float cloudsScale = g_cloudsParams.y;
    float smoothness = g_cloudsParams.z;

    float noise = g_cloudNoiseTex.SampleGrad(
        g_cloudNoiseSampler, p * 4.0 + tOffsetSmall, gx * 4.0, gy * 4.0).r * 0.1 + 0.9;

    float cloudsShape = g_cloudShapeTex.SampleGrad(
        g_cloudShapeSampler, (p + tOffset) * cloudsScale, gx * cloudsScale, gy * cloudsScale).r;

    float heightCurve = HeightCurve(progress);

    float baseDensity = 1.0 - g_cloudsParams.x;

    // HLSL: smoothstep требует edge0 < edge1, smoothness > 0 это гарантирует.
    return smoothstep(
        baseDensity - smoothness,
        baseDensity + smoothness,
        cloudsShape * noise * heightCurve);
}

// x - плотность, y - "затенённость" (сколько облаков между точкой и солнцем).
// cloudsXY - eye.xz / eye.y (в Godot это direction.xy, direction.z = 1 не используется).
float2 CloudRayMarch(float2 cloudsXY, float3 sunDirection, float2 cloudsDdx, float2 cloudsDdy)
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
        float2 samplePoint = cloudsXY * height * progress;

        float2 gx = cloudsDdx * height * progress;
        float2 gy = cloudsDdy * height * progress;

        density += CloudDensity(samplePoint, progress, gx, gy);

        float pointLight = 0.0;

        [loop]
        for (int f = 0; f < shadowSamples; f++)
        {
            float shadowProgress = float(f) / float(shadowSamples);
            float2 shadowOffset = sunDirection.xy * height * shadowProgress;

            pointLight += CloudDensity(samplePoint + shadowOffset, progress, gx, gy);
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

    // Производные координаты облаков берём здесь, в единообразном потоке
    // (внутри циклов их брать нельзя). Godot считает облака в half-res проходе,
    // пиксель там вдвое крупнее - отсюда множитель 2.
    float2 cloudsXY = eye.xz / max(saturate(eye.y), 1e-4);
    float2 cloudsDdx = ddx(cloudsXY) * 2.0;
    float2 cloudsDdy = ddy(cloudsXY) * 2.0;

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
        float2 clouds = CloudRayMarch(cloudsXY, g_lightDir.xyz, cloudsDdx, cloudsDdy);

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
