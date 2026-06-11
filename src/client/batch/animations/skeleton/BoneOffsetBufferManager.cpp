#include "BoneOffsetBufferManager.hpp"

BoneOffsetBufferManager::BoneOffsetBufferManager(
    VkDevice device,
    BufferManager* bufferManager,
    size_t maxInstances
)
    :
    device(device),
    maxInstances(maxInstances)
{
    VkDeviceSize size =
        sizeof(uint32_t) * maxInstances;

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

BoneOffsetBufferManager::~BoneOffsetBufferManager()
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

void BoneOffsetBufferManager::update(
    const std::vector<uint32_t>& offsets
)
{
    if (offsets.empty())
        return;

    void* data;

    if (offsets.size() > maxInstances)
    {
        throw std::runtime_error(
            "BoneOffsetBufferManager overflow."
        );
    }

    vkMapMemory(
        device,
        memory,
        0,
        sizeof(uint32_t) * offsets.size(),
        0,
        &data
    );

    memcpy(
        data,
        offsets.data(),
        sizeof(uint32_t) * offsets.size()
    );

    vkUnmapMemory(
        device,
        memory
    );
}