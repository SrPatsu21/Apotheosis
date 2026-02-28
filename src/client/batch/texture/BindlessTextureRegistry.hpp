#pragma once

#include "../../CoreVulkan.hpp"
#include "TextureImage.hpp"

#include <vector>
#include <memory>
#include <stdexcept>

class BindlessTextureRegistry
{
public:
    class BindlessTextureHandle
    {
    friend class BindlessTextureRegistry;
    private:
        std::unique_ptr<TextureImage> texture;
        uint32_t index = 0;
        BindlessTextureRegistry* registry = nullptr;

    public:
        BindlessTextureHandle(
            std::unique_ptr<TextureImage> tex,
            uint32_t idx,
            BindlessTextureRegistry* reg
        )
            : texture(std::move(tex)), index(idx), registry(reg)
        {}

        ~BindlessTextureHandle()
        {
            if (registry)
                registry->release(index);
            // texture é destruída automaticamente
        }

        BindlessTextureHandle(const BindlessTextureHandle&) = delete;
        BindlessTextureHandle& operator=(const BindlessTextureHandle&) = delete;
        BindlessTextureHandle(BindlessTextureHandle&&) = delete;
        BindlessTextureHandle& operator=(BindlessTextureHandle&&) = delete;

        TextureImage* get() const { return texture.get(); }
        uint32_t getIndex() const { return index; }
    };

private:
    VkDevice device{ VK_NULL_HANDLE };
    VkDescriptorSetLayout layout{ VK_NULL_HANDLE };
    VkDescriptorPool pool{ VK_NULL_HANDLE };
    VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };

    uint32_t capacity = 0;

    std::vector<std::shared_ptr<BindlessTextureHandle>> handles;

    void release(uint32_t index);
public:
    BindlessTextureRegistry(
        VkDevice device,
        VkDescriptorPool pool,
        VkDescriptorSetLayout layout,
        uint32_t capacity
    );

    ~BindlessTextureRegistry() = default;

    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> registerTexture(
        std::unique_ptr<TextureImage> texture
    );

    VkDescriptorSet getDescriptorSet() const { return descriptorSet; }
};