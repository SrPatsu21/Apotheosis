#include "VertexManager.hpp"

VertexManager::VertexManager(const std::vector<Vertex>* vertices)
{
    VkDeviceSize bufferSize = sizeof((*vertices)[0]) * vertices->size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(*CoreVulkan::getDevice(), &bufferInfo, nullptr, this->vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*CoreVulkan::getDevice(), *this->vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*CoreVulkan::getPhysicalDevice(), &memProperties);

    bool memTypeFound = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags &
            (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) ==
                (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            allocInfo.memoryTypeIndex = i;
            memTypeFound = true;
            break;
        }
    }

    if (!memTypeFound) {
        throw std::runtime_error("failed to find suitable memory type for vertex buffer!");
    }

    if (vkAllocateMemory(*CoreVulkan::getDevice(), &allocInfo, nullptr, this->vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(*CoreVulkan::getDevice(), *this->vertexBuffer, *this->vertexBufferMemory, 0);

    void* data;
    vkMapMemory(*CoreVulkan::getDevice(), *this->vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices->data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(*CoreVulkan::getDevice(), *this->vertexBufferMemory);
};

VertexManager::~VertexManager()
{
    vkDestroyBuffer(*CoreVulkan::getDevice(), *this->vertexBuffer, nullptr);
    vkFreeMemory(*CoreVulkan::getDevice(), *this->vertexBufferMemory, nullptr);
}