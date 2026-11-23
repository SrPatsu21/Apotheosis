#include "BoneBufferManager.hpp"

#include "../../../BufferManager.hpp"

BoneBufferManager::BoneBufferManager(
    VkDevice device,
    BufferManager* bufferManager,
    size_t maxBones
)
    :
    device(device),
    maxBones(maxBones)
{
    VkDeviceSize size =
        sizeof(glm::mat4) * maxBones;

    bufferManager->createBuffer(
        size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        buffer
    );

    bufferManager->allocateBufferMemory(
        buffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memory
    );

    vkBindBufferMemory(
        device,
        buffer,
        memory,
        0
    );
}

BoneBufferManager::~BoneBufferManager()
{
    vkDestroyBuffer(
        device,
        buffer,
        nullptr
    );

    vkFreeMemory(
        device,
        memory,
        nullptr
    );
}

void BoneBufferManager::update(
    const std::vector<glm::mat4>& matrices
)
{
    void* data;

    vkMapMemory(
        device,
        memory,
        0,
        sizeof(glm::mat4) * matrices.size(),
        0,
        &data
    );

    memcpy(
        data,
        matrices.data(),
        sizeof(glm::mat4) * matrices.size()
    );

    vkUnmapMemory(
        device,
        memory
    );
}