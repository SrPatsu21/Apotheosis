#include "VertexManager.hpp"

VertexManager::VertexManager(const std::vector<Vertex> vertices, VkCommandPool commandPool)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createVertexBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
    allocateVertexBufferMemory(stagingBuffer, bufferSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferMemory);
    vkBindBufferMemory(CoreVulkan::getDevice(), stagingBuffer, stagingBufferMemory, 0);

    void* data;
    vkMapMemory(CoreVulkan::getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(CoreVulkan::getDevice(), stagingBufferMemory);

    createVertexBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, this->vertexBuffer);
    allocateVertexBufferMemory(stagingBuffer, bufferSize, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->vertexBufferMemory);
    vkBindBufferMemory(CoreVulkan::getDevice(), this->vertexBuffer, this->vertexBufferMemory, 0);

    copyBuffer(stagingBuffer, this->vertexBuffer, bufferSize, commandPool);

    vkDestroyBuffer(CoreVulkan::getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), stagingBufferMemory, nullptr);
};

VertexManager::~VertexManager()
{
    vkDestroyBuffer(CoreVulkan::getDevice(), this->vertexBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), this->vertexBufferMemory, nullptr);
}

void VertexManager::createVertexBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(CoreVulkan::getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }
};

void VertexManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(CoreVulkan::getDevice(), &allocInfo, &commandBuffer);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(CoreVulkan::getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(CoreVulkan::getGraphicsQueue());

    vkFreeCommandBuffers(CoreVulkan::getDevice(), commandPool, 1, &commandBuffer);
}

void VertexManager::allocateVertexBufferMemory(VkBuffer buffer, VkDeviceSize bufferSize, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory){
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(CoreVulkan::getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = CoreVulkan::findMemoryType(memRequirements.memoryTypeBits, properties);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(CoreVulkan::getPhysicalDevice(), &memProperties);

    bool memTypeFound = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags &
            (properties)) ==
                (properties)) {
            allocInfo.memoryTypeIndex = i;
            memTypeFound = true;
            break;
        }
    }

    if (!memTypeFound) {
        throw std::runtime_error("failed to find suitable memory type for vertex buffer!");
    }

    if (vkAllocateMemory(CoreVulkan::getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
};