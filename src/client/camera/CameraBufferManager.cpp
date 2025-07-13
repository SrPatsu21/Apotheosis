#include "CameraBufferManager.hpp"

void CameraBufferManager::createUniformBuffer(){
        VkDeviceSize bufferSize = sizeof(glm::mat4);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(*CoreVulkan::getDevice(), &bufferInfo, nullptr, this->uniformBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create uniform buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*CoreVulkan::getDevice(), *this->uniformBuffer, &memRequirements);

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
            throw std::runtime_error("failed to find suitable memory type for uniform buffer!");
        }

        if (vkAllocateMemory(*CoreVulkan::getDevice(), &allocInfo, nullptr, this->uniformBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate uniform buffer memory!");
        }

        vkBindBufferMemory(*CoreVulkan::getDevice(), *this->uniformBuffer, *this->uniformBufferMemory, 0);
    }

    CameraBufferManager::CameraBufferManager()
    {
        createUniformBuffer();
    };
    CameraBufferManager::~CameraBufferManager()
    {
        vkDestroyBuffer(*CoreVulkan::getDevice(), *this->uniformBuffer, nullptr);
        vkFreeMemory(*CoreVulkan::getDevice(), *this->uniformBufferMemory, nullptr);
    };