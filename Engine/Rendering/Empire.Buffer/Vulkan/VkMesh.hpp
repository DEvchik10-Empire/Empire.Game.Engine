#pragma once

#include <memory>
#include <vector>

#include "../../Empire.Mesh/Mesh.hpp"
#include "VkBuffer.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            // Меш на GPU: вершинный + индексный буферы.
            class GpuMesh
            {
                public:
                    GpuMesh(const Device& device, const MeshData& mesh);

                    GpuMesh(const GpuMesh&) = delete;
                    GpuMesh& operator=(const GpuMesh&) = delete;

                    // Привязывает буферы и рисует. Pipeline уже должен быть привязан.
                    void Draw(VkCommandBuffer commandBuffer) const;

                    // Покусочная отрисовка (по одному куску на материал):
                    // Bind один раз, затем DrawRange для каждого куска.
                    void Bind(VkCommandBuffer commandBuffer) const;
                    void DrawRange(VkCommandBuffer commandBuffer, Types::u32 firstIndex, Types::u32 indexCount) const;

                    // Куски меши. Если у меши нет своих - один кусок на всё с материалом 0.
                    const std::vector<SubMesh>& GetSubMeshes() const;

                    Types::u32 GetIndexCount() const;

                private:
                    std::unique_ptr<Buffer> vertexBuffer;
                    std::unique_ptr<Buffer> indexBuffer;
                    Types::u32 indexCount = 0;
                    std::vector<SubMesh> submeshes;
            };
        }
    }
}
