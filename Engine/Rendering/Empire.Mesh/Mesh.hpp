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

        // Картинка RGBA, 8 бит на канал, строки сверху вниз (как в glTF/Vulkan).
        struct ImageRGBA8
        {
            Types::u32 width = 0;
            Types::u32 height = 0;
            std::vector<Types::u8> pixels;
        };

        // Материал из glTF (подмножество: base color + альфа-режим + металл/шероховатость).
        struct MaterialData
        {
            Types::f32 baseColorFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // линейные
            Types::i32 baseColorImage = -1;                           // индекс в MeshData::images
            Types::f32 metallic = 0.0f;
            Types::f32 roughness = 0.6f;
            Types::f32 alphaCutoff = -1.0f; // < 0 - непрозрачный; иначе discard при alpha < cutoff
        };

        // Диапазон индексов с одним материалом.
        struct SubMesh
        {
            Types::u32 firstIndex = 0;
            Types::u32 indexCount = 0;
            Types::u32 material = 0;
        };

        struct MeshData
        {
            std::vector<Vertex> vertices;
            std::vector<Types::u32> indices;

            // Заполняются загрузчиком glTF. Пусто у встроенных примитивов и .obj:
            // тогда рисуется всё одним куском с материалом по умолчанию.
            std::vector<SubMesh> submeshes;
            std::vector<MaterialData> materials;
            std::vector<ImageRGBA8> images;
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

        // Загрузка glTF 2.0: .glb (бинарный) или .gltf с внешним .bin.
        // Берётся статичная геометрия всей сцены (иерархия узлов применяется,
        // анимации и скиннинг игнорируются; из материалов берутся base color
        // (текстура и множитель), металл/шероховатость-множители и alphaMode),
        // примитивы сливаются в одну мешу с кусками по материалам, модель центрируется и масштабируется до радиуса 1.
        // Не поддерживаются сжатые файлы (Draco, meshopt) и данные в base64.
        // false - причина печатается в std::cerr.
        Types::b8 LoadGltf(const char* path, MeshData& out);

        // Вспомогательные (используются загрузчиками).
        void NormalizeMeshToUnit(MeshData& mesh);
        void ComputeMeshSmoothNormals(MeshData& mesh);
    }
}
