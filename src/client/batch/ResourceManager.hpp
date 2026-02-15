#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "mesh/Mesh.hpp"
#include "material/Material.hpp"

class ResourceManager
{
private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    BufferManager* bufferManager;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout layout;

    std::unordered_map<std::string, std::weak_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::weak_ptr<Material>> materials;
public:
    ResourceManager(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        BufferManager* bufferManager,
        VkDescriptorPool descriptorPool,
        VkDescriptorSetLayout layout
    );
    ~ResourceManager() = default;

    std::shared_ptr<Mesh> getMesh(
        const std::string& meshPath
    );

    std::shared_ptr<Material> getMaterial(
        const std::string& texturePath
    );
};
