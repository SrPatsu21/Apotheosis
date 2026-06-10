#pragma once

#include "../../CoreVulkan.hpp"
#include "Vertex.hpp"
#include "../../BufferManager.hpp"

#include <cstring>

class SkinnedVertexBufferManager
{
private:
    VkDevice device;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
public:
    SkinnedVertexBufferManager(
        VkDevice device,
        BufferManager* bufferManager,
        const std::vector<SkinnedVertex>& vertices
    );

    SkinnedVertexBufferManager(const SkinnedVertexBufferManager&) = delete;
    SkinnedVertexBufferManager& operator=(const SkinnedVertexBufferManager&) = delete;

    SkinnedVertexBufferManager(SkinnedVertexBufferManager&&) noexcept = delete;
    SkinnedVertexBufferManager& operator=(SkinnedVertexBufferManager&&) noexcept = delete;

    ~SkinnedVertexBufferManager();

    VkBuffer getVertexBuffer() const {return vertexBuffer;}
    VkDeviceMemory getVertexBufferMemory() const {return vertexBufferMemory;}
};