#include "Material.hpp"
#include <array>
#include <stdexcept>

Material::Material(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout layout,
    std::shared_ptr<TextureImage> baseColor,
    std::shared_ptr<TextureImage> normal,
    std::shared_ptr<TextureImage> metallicRoughness
) :
    device(device),
    baseColor(std::move(baseColor)),
    normal(std::move(normal)),
    metallicRoughness(std::move(metallicRoughness))
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate material descriptor set");

    std::array<VkWriteDescriptorSet, 3> writes{};
    std::array<VkDescriptorImageInfo, 3> imageInfos{};

    auto fillImageInfo = [&](uint32_t index, const std::shared_ptr<TextureImage>& tex)
    {
        if (!tex)
            return;

        imageInfos[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[index].imageView = tex->getImageView();
        imageInfos[index].sampler = tex->getSampler();

        writes[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[index].dstSet = descriptorSet;
        writes[index].dstBinding = index;
        writes[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[index].descriptorCount = 1;
        writes[index].pImageInfo = &imageInfos[index];
    };

    fillImageInfo(0, baseColor);
    fillImageInfo(1, normal);
    fillImageInfo(2, metallicRoughness);

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(writes.size()),
        writes.data(),
        0,
        nullptr
    );
}