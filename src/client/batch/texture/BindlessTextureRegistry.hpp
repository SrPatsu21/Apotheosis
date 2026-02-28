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
        }

        TextureImage* get() const { return texture.get(); }
        uint32_t getIndex() const { return index; }

        BindlessTextureHandle(const BindlessTextureHandle&) = delete;
        BindlessTextureHandle& operator=(const BindlessTextureHandle&) = delete;
    };
private:
    VkDevice device;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    uint32_t capacity;

    std::vector<std::shared_ptr<BindlessTextureHandle>> handles;

public:
    BindlessTextureRegistry(
        VkDevice device,
        uint32_t capacity
    );

    ~BindlessTextureRegistry();

    std::shared_ptr<BindlessTextureHandle>
    registerTexture(std::unique_ptr<TextureImage> texture);

    VkDescriptorSetLayout getLayout() const { return layout; }
    VkDescriptorSet getDescriptorSet() const { return descriptorSet; }

private:
    void release(uint32_t index);

};