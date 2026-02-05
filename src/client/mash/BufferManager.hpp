#pragma once

#include "../CoreVulkan.hpp"

class BufferManager
{
private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;

    struct ImmediateSubmitContext {
        VkCommandPool pool;
        VkCommandBuffer cmd;
        VkFence fence;
    };
    ImmediateSubmitContext immediate;
    void initImmediateContext(
        uint32_t graphicsQueueFamily
    );
    void destroyImmediateContext();
public:
    BufferManager(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkQueue graphicsQueue,
        uint32_t graphicsQueueFamily
    );
    void createBuffer(
        VkDeviceSize bufferSize,
        VkBufferUsageFlags usage,
        VkBuffer& buffer
    );
    void allocateBufferMemory(
        VkBuffer buffer,
        VkMemoryPropertyFlags properties,
        VkDeviceMemory& bufferMemory
    );
    VkCommandBuffer beginImmediate();
    void endImmediate();
    void copyBuffer(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size
    );
    void copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height
    );
    ~BufferManager();
};
