#include "BoneOffsetBufferManager.hpp"

BoneOffsetBufferManager::BoneOffsetBufferManager(
    VkDevice device,
    BufferManager* bufferManager,
    size_t maxInstances,
    BoneDescriptorSetLayout* layout,
    uint32_t framesInFlight
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

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = framesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = framesInFlight;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    if (vkCreateDescriptorPool(
        device,
        &poolInfo,
        nullptr,
        &descriptorPool
    ) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create BoneOffset descriptor pool."
        );
    }

    std::vector<VkDescriptorSetLayout> layouts(
        framesInFlight,
        layout->getDescriptorSetLayout()
    );

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = framesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(framesInFlight);

    if (vkAllocateDescriptorSets(
        device,
        &allocInfo,
        descriptorSets.data()
    ) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate BoneOffset descriptor sets."
        );
    }

    for (uint32_t i = 0; i < framesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(uint32_t) * maxInstances;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSets[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(
            device,
            1,
            &write,
            0,
            nullptr
        );
    }


}

BoneOffsetBufferManager::~BoneOffsetBufferManager()
{
    vkDestroyDescriptorPool(
        device,
        descriptorPool,
        nullptr
    );

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
    uint32_t offset,
    const std::vector<uint32_t>& offsets
)
{
    if (offsets.empty())
        return;

    if (offset + offsets.size() > maxInstances)
    {
        throw std::runtime_error(
            "BoneOffsetBufferManager overflow."
        );
    }

    void* mapped = nullptr;

    VkDeviceSize byteOffset =
        sizeof(uint32_t) * offset;

    VkDeviceSize byteSize =
        sizeof(uint32_t) * offsets.size();

    vkMapMemory(
        device,
        memory,
        byteOffset,
        byteSize,
        0,
        &mapped
    );

    memcpy(
        mapped,
        offsets.data(),
        byteSize
    );

    vkUnmapMemory(
        device,
        memory
    );
}