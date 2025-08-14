#include "CameraBufferManager.hpp"
#include "UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

void CameraBufferManager::createUniformBuffer(){
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(CoreVulkan::getDevice(), &bufferInfo, nullptr, &this->uniformBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create uniform buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(CoreVulkan::getDevice(), this->uniformBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(CoreVulkan::getPhysicalDevice(), &memProperties);

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

    if (vkAllocateMemory(CoreVulkan::getDevice(), &allocInfo, nullptr, &this->uniformBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate uniform buffer memory!");
    }

    vkBindBufferMemory(CoreVulkan::getDevice(), this->uniformBuffer, this->uniformBufferMemory, 0);
}

void CameraBufferManager::updateUniformBuffer(SwapchainManager* swapchainManager, float time) {
    UniformBufferObject ubo{};

    // Model: identity
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, time, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time, glm::vec3(0.0f, 0.2f, 0.0f));
        model = glm::rotate(model, time, glm::vec3(0.0f, 0.0f, 0.5f));
    ubo.model = model;

    // View: camera looking at -Z (as if it were at (2,2,2) looking at (0,0,0))
    ubo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),  // posição da câmera
        glm::vec3(0.0f, 0.0f, 0.0f),  // para onde ela olha
        glm::vec3(0.0f, 0.0f, 1.0f)   // "up"
    );

    ubo.proj = glm::perspective(glm::radians(45.0f), swapchainManager->getExtent().width / (float) swapchainManager->getExtent().height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1; //TODO remove and make right from begin

    void* data;
    vkMapMemory(CoreVulkan::getDevice(), this->uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(CoreVulkan::getDevice(), this->uniformBufferMemory);
}

CameraBufferManager::CameraBufferManager()
{
    createUniformBuffer();
};
CameraBufferManager::~CameraBufferManager()
{
    vkDestroyBuffer(CoreVulkan::getDevice(), this->uniformBuffer, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), this->uniformBufferMemory, nullptr);
};