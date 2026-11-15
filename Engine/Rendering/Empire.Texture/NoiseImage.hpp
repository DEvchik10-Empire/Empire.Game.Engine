#pragma once

#include <vector>

#include "Kernel/Types.hpp"

namespace Engine
{
    namespace Rendering
    {
        using namespace Kernel;

        // Одноканальная картинка 8 бит, строки сверху вниз. Не зависит от Vulkan.
        struct ImageR8
        {
            Types::u32 width = 0;
            Types::u32 height = 0;
            std::vector<Types::u8> pixels;
        };

        /*
         * Бесшовные шумы для неба (замена NoiseTexture2D + FastNoiseLite из Godot).
         * Значения растянуты на весь диапазон 0..255 (как normalize в Godot).
         */

        // Фрактальный шум Перлина (fbm), бесшовный.
        //   baseCells - сколько ячеек шума на ширину картинки в первой октаве
        //               (у FastNoiseLite частота 0.01 на 512 px ~ 5 ячеек),
        //   octaves   - число октав (gain 0.5, lacunarity 2),
        //   gain      - контраст: 0 - без изменений, иначе сигмоида
        //               1 / (1 + exp(-gain * (v - 0.5))) (мягкий, без плато),
        //   bias      - сдвиг сигмоиды: больше - больше светлых участков
        //               (для облаков: больше покрытие неба),
        //   shiftX/Y  - циклический сдвиг картинки в пикселях: какой участок
        //               шума окажется в координате uv = (0, 0). Для неба это
        //               то, что видно прямо над головой.
        ImageR8 MakeFbmNoiseImage(
            Types::u32 size,
            Types::u32 baseCells,
            Types::u32 octaves,
            Types::u32 seed,
            Types::f32 gain = 0.0f,
            Types::u32 shiftX = 0,
            Types::u32 shiftY = 0,
            Types::f32 bias = 0.0f
        );

        /*
         * Аналог NoiseTexture2D + FastNoiseLite из Godot с настройками по
         * умолчанию: симплекс-шум, frequency 0.01 (в координатах пикселей),
         * fbm, lacunarity 2, gain 0.5, seamless, normalize.
         *   size      - сторона картинки (в Godot 512),
         *   octaves   - 5 по умолчанию (cloud_noise), 10 у cloud_shape,
         *   skirt     - seamless_blend_skirt: 0.3 по умолчанию, 1.0 у cloud_shape.
         * Алгоритм симплекса не побитово тот же, что у FastNoiseLite, но
         * статистика и вид такие же.
         */
        ImageR8 MakeSimplexFbmImage(
            Types::u32 size,
            Types::u32 octaves,
            Types::u32 seed,
            Types::f32 skirt,
            Types::f32 frequency = 0.01f
        );

        // Клеточный шум (расстояние до ближайшей точки), бесшовный.
        ImageR8 MakeCellularImage(Types::u32 size, Types::u32 cells, Types::u32 seed);

        // Точные текстуры неба из проекта GDQuest (выгружены из Godot 4.2.2):
        // cloud_shape, cloud_noise, high_clouds - 512x512.
        ImageR8 GetGodotCloudShapeImage();
        ImageR8 GetGodotCloudNoiseImage();
        ImageR8 GetGodotHighCloudsImage();

        // Оригинальная текстура падающей звезды GDQuest (128x128, вшита в код).
        ImageR8 GetOriginalShootingStarImage();

        // Хвост падающей звезды (собственный вариант): голова сверху, хвост вниз.
        ImageR8 MakeShootingStarImage(Types::u32 size);
    }
}
