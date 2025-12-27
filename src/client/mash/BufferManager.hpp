#pragma once

#include "../CoreVulkan.hpp"

class BufferManager
{
private:
public:
    BufferManager();
    void createBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer);
    void allocateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool);
    ~BufferManager();
};
