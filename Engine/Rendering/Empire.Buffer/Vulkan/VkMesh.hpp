#pragma once

#include <memory>

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

                    Types::u32 GetIndexCount() const;

                private:
                    std::unique_ptr<Buffer> vertexBuffer;
                    std::unique_ptr<Buffer> indexBuffer;
                    Types::u32 indexCount = 0;
            };
        }
    }
}
