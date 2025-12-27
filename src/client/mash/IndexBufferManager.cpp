#include "IndexBufferManager.hpp"

IndexBufferManager::IndexBufferManager(const std::vector<uint16_t> indices, VkCommandPool commandPool) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BufferManager bufferManager = BufferManager();

    bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
    bufferManager.allocateBufferMemory(stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);
    vkBindBufferMemory(CoreVulkan::getDevice(), stagingBuffer, stagingBufferMemory, 0);

    void* data;
    vkMapMemory(CoreVulkan::getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(CoreVulkan::getDevice(), stagingBufferMemory);

    bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, this->indexBuffer);
    bufferManager.allocateBufferMemory(this->indexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->indexBufferMemory);
    vkBindBufferMemory(CoreVulkan::getDevice(), this->indexBuffer, this->indexBufferMemory, 0);

    bufferManager.copyBuffer(stagingBuffer, this->indexBuffer, bufferSize, commandPool);

    vkDestroyBuffer(CoreVulkan::getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), stagingBufferMemory, nullptr);
}

IndexBufferManager::~IndexBufferManager(){
    vkDestroyBuffer(CoreVulkan::getDevice(), this->indexBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), this->indexBufferMemory, nullptr);
}