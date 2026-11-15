#pragma once

#include <vector>

#include "Kernel/Types.hpp"

namespace Engine
{
    namespace Rendering
    {
        using namespace Kernel;

        /*
         * Меша на стороне CPU: не зависит ни от Vulkan, ни от окна.
         * Раскладка Vertex используется и как формат вершинного буфера GPU.
         */
        struct Vertex
        {
            Types::f32 position[3] = {};
            Types::f32 normal[3] = {};
            Types::f32 uv[2] = {};
        };

        struct MeshData
        {
            std::vector<Vertex> vertices;
            std::vector<Types::u32> indices;
        };

        // Все примитивы центрированы в нуле и вписаны в сферу радиуса ~1.
        MeshData MakeCube();
        MeshData MakeSphere(Types::u32 segments = 64, Types::u32 rings = 32);
        MeshData MakeTorusKnot(
            Types::u32 tubularSegments = 220,
            Types::u32 radialSegments = 24,
            Types::u32 p = 2,
            Types::u32 q = 3
        );

        // Загрузка Wavefront .obj (v/vt/vn/f, многоугольники режутся на треугольники).
        // Если нормалей в файле нет - считаются сглаженные.
        // Модель центрируется и масштабируется до радиуса 1.
        // false - файл не открылся или в нём нет геометрии.
        Types::b8 LoadObj(const char* path, MeshData& out);
    }
}
