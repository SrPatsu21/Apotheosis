#pragma once

#include "../../CoreVulkan.hpp"
#include "../texture/TextureImage.hpp"
#include <memory>

/**
 * @class Material
 * @brief Represents a PBR material backed by a Vulkan descriptor set.
 *
 * A Material owns:
 * - A descriptor set
 * - References to texture resources (base color, normal, metallic-roughness)
 *
 * The descriptor set binds textures as combined image samplers
 * for use in fragment shaders.
 *
 * Texture bindings:
 * - Binding 0 → Base color
 * - Binding 1 → Normal map
 * - Binding 2 → Metallic-Roughness map
 *
 * The descriptor set layout provided must match these bindings.
 */
class Material
{
private:
    VkDevice device;

    std::shared_ptr<TextureImage> baseColor;
    std::shared_ptr<TextureImage> normal;
    std::shared_ptr<TextureImage> metallicRoughness;

    VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };

public:
    /**
     * @brief Creates a material and allocates its descriptor set.
     *
     * Allocates a descriptor set from the given pool and updates it
     * with the provided texture images.
     *
     * Expected descriptor layout bindings:
     * - 0: VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER (base color)
     * - 1: VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER (normal)
     * - 2: VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER (metallic-roughness)
     *
     * Textures may be null. If null, the corresponding descriptor
     * entry remains uninitialized.
     *
     * @param device Vulkan logical device.
     * @param descriptorPool Pool used for descriptor allocation.
     * @param layout Descriptor set layout matching material bindings.
     * @param baseColor Base color texture.
     * @param normal Normal map texture.
     * @param metallicRoughness Metallic-roughness texture.
     *
     * @throws std::runtime_error if descriptor allocation fails.
     */
    Material(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        VkDescriptorSetLayout layout,
        std::shared_ptr<TextureImage> baseColor,
        std::shared_ptr<TextureImage> normal,
        std::shared_ptr<TextureImage> metallicRoughness
    );

    ~Material() = default;

    VkDescriptorSet getDescriptorSet() const { return descriptorSet; }
};