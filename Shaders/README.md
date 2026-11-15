# Shaders (HLSL -> SPIR-V)

Все шейдеры движка пишутся на HLSL и компилируются в SPIR-V через DXC.

| Файл | Что делает |
|---|---|
| `Sky.vs.hlsl` / `Sky.ps.hlsl` | Небо: порт Godot `shader_type sky` (градиент, закатное свечение, звёзды Вороного, падающие звёзды, солнце/луна, высокие облака, ray-march облака с тенями) |
| `Lit.vs.hlsl` / `Lit.ps.hlsl` | Модель: переливающийся материал + GGX/Fresnel + отражение неба |
| `Common/Scene.hlsli` | cbuffer кадра (set 0, binding 0), тонмаппинг |
| `Common/Sky.hlsli` | вся логика неба |
| `Common/Noise.hlsli` | шумы (замена текстурам из Godot) |

## Компиляция

    xmake build Empire.Shaders        # или:
    ./Shaders/compile.sh              # результат: build/shaders/*.spv

Нужен `dxc` (DirectXShaderCompiler). Путь можно задать: `DXC=/path/to/dxc`.

## Соглашения

- Точка входа всегда `main`; `*.vs.hlsl` -> vs_6_0, `*.ps.hlsl` -> ps_6_0.
- Матрицы в uniform/push передаются как 4 столбца `float4` и умножаются
  вручную (`MulCols`) - без зависимости от row/column-major в dxc.
- Раскладка `SceneParams` в `Common/Scene.hlsli` должна совпадать со структурой
  `SceneUniformData` в `Tests/Rendering/SceneMain.cpp`.

## Отличия от Godot-версии

Логика шейдера та же (градиент, sun_scatter, звёзды Вороного, падающие звёзды,
astro, высокие облака, ray-march облаков с тенью к солнцу). Параметры дня и
ночи - точные значения из `day_sky.tres` / `night_sky.tres`; тонемаппинг - Filmic,
как в их сцене. Что сделано иначе:

- Текстуры шума (cloud_shape, cloud_noise, high_clouds, shooting_star)
  генерируются в движке (`Empire.Texture/NoiseImage.cpp`) и подаются как Texture2D
  (set 0, binding 1..4). Оригинальные картинки (CC-BY-NC-SA) не используются.
  Форма облаков зависит от этих шумов, поэтому не идентична оригиналу.
- Нет half-res прохода: облака считаются в том же проходе, композит тот же.
- Уровень mip для облачных текстур считается один раз по производным
  (в Godot - аппаратная анизотропная фильтрация).
- cloud_curves: в оригинале из-за clamp всегда даёт 0.8; сделано явно.
- В HLSL `smoothstep(a, b, x)` требует a < b, поэтому `smoothstep(0.5, 0.45, y)`
  записано как `1 - smoothstep(0.45, 0.5, y)`.
