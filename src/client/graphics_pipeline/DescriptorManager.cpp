#include "DescriptorManager.hpp"
#include "../camera/UniformBufferObject.hpp"

DescriptorManager::DescriptorManager(CameraBufferManager* cameraBufferManager, uint32_t max_frames_in_flight)
{
    createDescriptorSetLayout();

    createDescriptorPool(max_frames_in_flight);
    createDescriptorSets(max_frames_in_flight);
    populateDescriptorSets(cameraBufferManager, max_frames_in_flight);
};

void DescriptorManager::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(CoreVulkan::getDevice(), &layoutInfo, nullptr, &this->descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void DescriptorManager::createDescriptorPool(uint32_t max_frames_in_flight){
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = max_frames_in_flight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = max_frames_in_flight;

    if (vkCreateDescriptorPool(CoreVulkan::getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorManager::createDescriptorSets(uint32_t max_frames_in_flight) {
    std::vector<VkDescriptorSetLayout> layouts(max_frames_in_flight, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = max_frames_in_flight;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(max_frames_in_flight);
    if (vkAllocateDescriptorSets(CoreVulkan::getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void DescriptorManager::populateDescriptorSets(CameraBufferManager* cameraBufferManager, uint32_t max_frames_in_flight)
{
    for (size_t i = 0; i < max_frames_in_flight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = cameraBufferManager->getUniformBuffers()[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(CoreVulkan::getDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

DescriptorManager::~DescriptorManager() {
    if (this->descriptorPool != VK_NULL_HANDLE) {
        // No need to vkFreeDescriptorSets unless you are reusing the pool
        vkDestroyDescriptorPool(CoreVulkan::getDevice(), this->descriptorPool, nullptr);
    }
    if (this->descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(CoreVulkan::getDevice(), this->descriptorSetLayout, nullptr);
    }
}