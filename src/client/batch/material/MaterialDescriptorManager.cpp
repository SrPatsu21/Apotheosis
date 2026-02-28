#include "MaterialDescriptorManager.hpp"

#include <stdexcept>

MaterialDescriptorManager::MaterialDescriptorManager(
    VkDevice device,
    uint32_t maxMaterials,
    bool bindless,
    std::vector<MaterialDescriptorManager::IMaterialLayoutProvider*> providers
) :
    device(device),
    bindlessMode(bindless)
{
    #ifndef NDEBUG
    std::cout << "bindlessMode: " << (bindlessMode ? "true" : "false") << std::endl;
    #endif
    MaterialLayoutBuilder builder(0, 7);

    // Engine provider
    EngineMaterialProvider engineProvider;
    engineProvider.contribute(builder);

    // External providers
    for (auto* p : providers)
        p->contribute(builder);

    //* Descriptor Set Layout
    std::vector<VkDescriptorSetLayoutBinding> vkBindings;

    for (const auto& b : builder.getBindings())
    {
        VkDescriptorSetLayoutBinding vk{};
        vk.binding = b.binding;
        vk.descriptorType = b.type;
        vk.descriptorCount = b.count;
        vk.stageFlags = b.stages;
        vk.pImmutableSamplers = nullptr;

        vkBindings.push_back(vk);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(vkBindings.size());
    layoutInfo.pBindings = vkBindings.data();

    // Bindless flags
    std::vector<VkDescriptorBindingFlags> bindingFlags;
    VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{};

    if (bindlessMode)
    {
        bindingFlags.resize(vkBindings.size(), 0);

        for (size_t i = 0; i < vkBindings.size(); ++i)
        {
            // Aplicar flags apenas para bindings que são arrays
            if (vkBindings[i].descriptorCount > 1)
            {
                bindingFlags[i] =
                    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
            }
        }

        flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        flagsInfo.bindingCount = static_cast<uint32_t>(bindingFlags.size());
        flagsInfo.pBindingFlags = bindingFlags.data();

        layoutInfo.pNext = &flagsInfo;
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    }

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create material descriptor layout");

    //* Descriptor Pool
    std::vector<VkDescriptorPoolSize> poolSizes;

    for (const auto& [type, count] : builder.getDescriptorCounts())
    {
        VkDescriptorPoolSize ps{};
        ps.type = type;
        ps.descriptorCount = count * maxMaterials;
        poolSizes.push_back(ps);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxMaterials;

    if (bindlessMode)
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create material descriptor pool");
}

MaterialDescriptorManager::~MaterialDescriptorManager()
{
    if (descriptorPool)
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    if (descriptorSetLayout)
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}