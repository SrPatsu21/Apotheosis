#include "VertexBufferManager.hpp"

VertexBufferManager::VertexBufferManager(const std::vector<Vertex> vertices, VkCommandPool commandPool)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BufferManager bufferManager = BufferManager();

    bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
    bufferManager.allocateBufferMemory(stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);
    vkBindBufferMemory(CoreVulkan::getDevice(), stagingBuffer, stagingBufferMemory, 0);

    void* data;
    vkMapMemory(CoreVulkan::getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(CoreVulkan::getDevice(), stagingBufferMemory);

    bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, this->vertexBuffer);
    bufferManager.allocateBufferMemory(this->vertexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->vertexBufferMemory);
    vkBindBufferMemory(CoreVulkan::getDevice(), this->vertexBuffer, this->vertexBufferMemory, 0);

    bufferManager.copyBuffer(stagingBuffer, this->vertexBuffer, bufferSize, commandPool);

    vkDestroyBuffer(CoreVulkan::getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), stagingBufferMemory, nullptr);
};

VertexBufferManager::~VertexBufferManager()
{
    vkDestroyBuffer(CoreVulkan::getDevice(), this->vertexBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), this->vertexBufferMemory, nullptr);
}