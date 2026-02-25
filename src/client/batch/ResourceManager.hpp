#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "mesh/Mesh.hpp"
#include "material/Material.hpp"
#include "texture/SamplerManager.hpp"
#include "texture/TextureImage.hpp"
#include "texture/BindlessTextureRegistry.hpp"

class ResourceManager
{
private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    BufferManager* bufferManager;
    BindlessTextureRegistry* bindlessRegistry;
    SamplerManager samplerManager;

    std::unordered_map<std::string, std::weak_ptr<Mesh>> meshes;
    std::unordered_map<
        std::string,
        std::weak_ptr<BindlessTextureRegistry::BindlessTextureHandle>
        > textures;
    std::unordered_map<std::string, std::weak_ptr<Material>> materials;
public:
    ResourceManager(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        BufferManager* bufferManager,
        BindlessTextureRegistry* bindlessRegistry
    );
    ~ResourceManager() = default;

    std::shared_ptr<Mesh> getMesh(
        const std::string& meshPath
    );
    std::vector<std::shared_ptr<Material>> getMaterialsForMesh(
        const Mesh& mesh
    );
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> getTexture(
        const std::string& path
    );
};
