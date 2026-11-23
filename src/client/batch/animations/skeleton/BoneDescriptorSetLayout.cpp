#include "BoneDescriptorSetLayout.hpp"

#include <stdexcept>

BoneDescriptorSetLayout::BoneDescriptorSetLayout(
    VkDevice device
)
    :
    device(device)
{
    VkDescriptorSetLayoutBinding binding{};

    binding.binding = 0;

    binding.descriptorType =
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    binding.descriptorCount = 1;

    binding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT;

    binding.pImmutableSamplers =
        nullptr;

    VkDescriptorSetLayoutCreateInfo createInfo{};

    createInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    createInfo.bindingCount = 1;

    createInfo.pBindings =
        &binding;

    if (
        vkCreateDescriptorSetLayout(
            device,
            &createInfo,
            nullptr,
            &descriptorSetLayout
        ) != VK_SUCCESS
    )
    {
        throw std::runtime_error(
            "Failed to create BoneDescriptorSetLayout."
        );
    }
}

BoneDescriptorSetLayout::~BoneDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(
        device,
        descriptorSetLayout,
        nullptr
    );
}