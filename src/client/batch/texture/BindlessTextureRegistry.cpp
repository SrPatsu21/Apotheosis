#include "BindlessTextureRegistry.hpp"

BindlessTextureRegistry::BindlessTextureRegistry(
    VkDevice device,
    VkDescriptorPool pool,
    VkDescriptorSetLayout layout,
    uint32_t capacity
)
    : device(device), pool(pool), layout(layout), capacity(capacity)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate bindless descriptor set");

    handles.reserve(capacity);
}

std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle>
    BindlessTextureRegistry::registerTexture(
        TextureImage* tex
) {
    std::unique_ptr<TextureImage> texture(tex);
    uint32_t index = static_cast<uint32_t>(handles.size());
    if (index >= capacity)
        throw std::runtime_error("Bindless texture capacity exceeded");

    index++;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture->getImageView();
    imageInfo.sampler = texture->getSampler();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstBinding = 0;
    write.dstArrayElement = index;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

    auto handle = std::make_shared<BindlessTextureHandle>(
        std::move(texture),
        index,
        this
    );

    if (handles.size() <= index)
        handles.push_back(handle.get());
    else
        handles[index] = handle.get();

    return handle;
}

void BindlessTextureRegistry::release(uint32_t index)
{
    uint32_t nextIndex = static_cast<uint32_t>(handles.size());
    if (index >= nextIndex)
        return;

    uint32_t lastIndex = nextIndex - 1;

    if (index != lastIndex)
    {
        // Move last texture descriptor into freed slot

        BindlessTextureHandle* lastHandle = handles[lastIndex];

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = lastHandle->texture->getImageView();
        imageInfo.sampler = lastHandle->texture->getSampler();

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSet;
        write.dstBinding = 0;
        write.dstArrayElement = index;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

        // Update moved handle index
        lastHandle->index = index;

        handles[index] = lastHandle;
    }

    handles.pop_back();
}