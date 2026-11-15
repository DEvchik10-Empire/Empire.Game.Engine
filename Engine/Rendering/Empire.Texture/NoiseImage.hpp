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

        // Клеточный шум (расстояние до ближайшей точки), бесшовный.
        ImageR8 MakeCellularImage(Types::u32 size, Types::u32 cells, Types::u32 seed);

        // Оригинальная текстура падающей звезды GDQuest (128x128, вшита в код).
        ImageR8 GetOriginalShootingStarImage();

        // Хвост падающей звезды (собственный вариант): голова сверху, хвост вниз.
        ImageR8 MakeShootingStarImage(Types::u32 size);
    }
}
