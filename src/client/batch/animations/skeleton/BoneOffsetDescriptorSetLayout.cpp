#include "BoneOffsetDescriptorSetLayout.hpp"

#include <stdexcept>

BoneOffsetDescriptorSetLayout::BoneOffsetDescriptorSetLayout(
    VkDevice device
)
    : device(device)
{
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 1;
    info.pBindings = &binding;

    if (vkCreateDescriptorSetLayout(
            device,
            &info,
            nullptr,
            &descriptorSetLayout
        ) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "failed to create BoneOffsetDescriptorSetLayout"
        );
    }
}

BoneOffsetDescriptorSetLayout::~BoneOffsetDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(
        device,
        descriptorSetLayout,
        nullptr
    );
}