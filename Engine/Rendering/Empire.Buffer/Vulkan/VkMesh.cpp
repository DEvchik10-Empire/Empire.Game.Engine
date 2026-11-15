#include "VkMesh.hpp"

#include <stdexcept>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            GpuMesh::GpuMesh(const Device& device, const MeshData& mesh)
            {
                if (mesh.vertices.empty() || mesh.indices.empty())
                {
                    throw std::runtime_error("GpuMesh: mesh is empty");
                }

                const VkDeviceSize vertexBytes = mesh.vertices.size() * sizeof(Vertex);
                const VkDeviceSize indexBytes = mesh.indices.size() * sizeof(Types::u32);

                vertexBuffer = std::make_unique<Buffer>(
                    device, vertexBytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

                indexBuffer = std::make_unique<Buffer>(
                    device, indexBytes, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

                vertexBuffer->Upload(mesh.vertices.data(), vertexBytes);
                indexBuffer->Upload(mesh.indices.data(), indexBytes);

                indexCount = static_cast<Types::u32>(mesh.indices.size());
            }

            void GpuMesh::Draw(VkCommandBuffer commandBuffer) const
            {
                const VkBuffer vertex = vertexBuffer->Get();
                const VkDeviceSize offset = 0;

                vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertex, &offset);
                vkCmdBindIndexBuffer(commandBuffer, indexBuffer->Get(), 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
            }

            Types::u32 GpuMesh::GetIndexCount() const
            {
                return indexCount;
            }
        }
    }
}
