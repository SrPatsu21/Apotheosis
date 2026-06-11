#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "mesh/Mesh.hpp"
#include "material/Material.hpp"
#include "texture/SamplerManager.hpp"
#include "texture/TextureImage.hpp"
#include "mesh/SkinnedMesh.hpp"

class ResourceManager
{
private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    BufferManager* bufferManager;
    SamplerManager samplerManager;
    MaterialDescriptorManager* descriptorManager;

    std::unordered_map<std::string, std::weak_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::weak_ptr<SkinnedMesh>> skinnedMeshs;
    std::unordered_map<std::string, std::weak_ptr<TextureImage>> textures;
    std::unordered_map<std::string, std::weak_ptr<Material>> materials;
public:
    ResourceManager(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        BufferManager* bufferManager,
        MaterialDescriptorManager* descriptorManager
    );
    ~ResourceManager() = default;

    std::shared_ptr<Mesh> getMesh(
        const std::string& meshPath
    );

    std::shared_ptr<SkinnedMesh> getskinnedMesh(
        const std::string& meshPath
    );

    std::vector<std::shared_ptr<Material>> getMaterialsForMesh(
        const Mesh& mesh
    );

    std::shared_ptr<Material> getMaterialForSubMesh(
        const Mesh& mesh,
        const Mesh::SubMesh& subMesh
    );

    std::shared_ptr<Material> getMaterialForSkinnedSubMesh(
        const SkinnedMesh& mesh,
        const SkinnedMesh::SubMesh& subMesh
    );

    std::shared_ptr<TextureImage> getTexture(
        const std::string& path
    );
};
