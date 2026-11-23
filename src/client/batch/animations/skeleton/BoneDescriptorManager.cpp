#include "BoneDescriptorManager.hpp"

BoneDescriptorManager::BoneDescriptorManager(
    VkDevice device,
    BoneDescriptorSetLayout* layout,
    BoneBufferManager* boneBuffer,
    uint32_t framesInFlight
)
:
device(device),
layout(layout)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type =
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount =
        framesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets =
        framesInFlight;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    vkCreateDescriptorPool(
        device,
        &poolInfo,
        nullptr,
        &descriptorPool
    );

    std::vector<VkDescriptorSetLayout> layouts(
        framesInFlight,
        layout->getDescriptorSetLayout()
    );

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool =
        descriptorPool;
    allocInfo.descriptorSetCount =
        framesInFlight;
    allocInfo.pSetLayouts =
        layouts.data();

    descriptorSets.resize(framesInFlight);

    vkAllocateDescriptorSets(
        device,
        &allocInfo,
        descriptorSets.data()
    );

    for (uint32_t i = 0; i < framesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer =
            boneBuffer->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range =
            VK_WHOLE_SIZE;

        VkWriteDescriptorSet write{};
        write.sType =
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet =
            descriptorSets[i];
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType =
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.pBufferInfo =
            &bufferInfo;

        vkUpdateDescriptorSets(
            device,
            1,
            &write,
            0,
            nullptr
        );
    }
}

BoneDescriptorManager::~BoneDescriptorManager()
{
    if (descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(
            device,
            descriptorPool,
            nullptr
        );
    }
    descriptorSets.clear();
}