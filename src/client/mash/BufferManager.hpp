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
    void copyBufferToImage(
        VkCommandPool commandPool,
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height
    );
    void endSingleTimeCommands(
        VkCommandPool commandPool,
        VkCommandBuffer commandBuffer
    );
    VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);
    ~BufferManager();
};
