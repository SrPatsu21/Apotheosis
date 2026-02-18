#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <unordered_set>

class MaterialLayoutBuilder
{
public:
    struct BindingInfo
    {
        uint32_t binding;
        VkDescriptorType type;
        uint32_t count;
        VkShaderStageFlags stages;
    };

private:
    std::vector<BindingInfo> bindings;
    std::unordered_set<uint32_t> usedBindings;
    std::unordered_map<VkDescriptorType, uint32_t> descriptorCountByType;

    uint32_t reservedStart;
    uint32_t reservedEnd;

public:

    MaterialLayoutBuilder(
        uint32_t reservedStart = 0,
        uint32_t reservedEnd = 7
    );

    void addBinding(
        uint32_t binding,
        VkDescriptorType type,
        uint32_t count,
        VkShaderStageFlags stages
    );

    void addEngineBinding(
        uint32_t binding,
        VkDescriptorType type,
        uint32_t count,
        VkShaderStageFlags stages
    );

    const std::vector<BindingInfo>& getBindings() const { return bindings; }
    const std::unordered_map<VkDescriptorType, uint32_t>& getDescriptorCounts() const { return descriptorCountByType; }
};
