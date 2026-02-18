#pragma once

#include "../../CoreVulkan.hpp"
#include "MaterialLayoutBuilder.hpp"

class MaterialDescriptorManager
{
public:
private:
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorPool descriptorPool{};
public:
    class IMaterialLayoutProvider
    {
    public:
        virtual ~IMaterialLayoutProvider() = default;
        virtual void contribute(MaterialLayoutBuilder& builder) = 0;
    };

    class EngineMaterialProvider : public IMaterialLayoutProvider
    {
    public:
        void contribute(MaterialLayoutBuilder& builder) override
        {
            // Binding 0 reserved for albedo
            builder.addEngineBinding(
                0,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT
            );

            // Binding 1 reserved for normal
            builder.addEngineBinding(
                1,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                1,
                VK_SHADER_STAGE_FRAGMENT_BIT
            );
        }
    };

    MaterialDescriptorManager(
        VkDevice device,
        uint32_t maxMaterials,
        std::vector<MaterialDescriptorManager::IMaterialLayoutProvider*> providers
    );

    ~MaterialDescriptorManager();

    VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
};
