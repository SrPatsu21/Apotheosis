#include "ResourceManager.hpp"

ResourceManager::ResourceManager(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    BufferManager* bufferManager,
    BindlessTextureRegistry* bindlessRegistry
) :
    physicalDevice(physicalDevice),
    device(device),
    bufferManager(bufferManager),
    bindlessRegistry(bindlessRegistry),
    samplerManager(physicalDevice, device)
{
}

std::shared_ptr<Mesh> ResourceManager::getMesh(
    const std::string& meshPath
) {
    auto it = meshes.find(meshPath);

    if (it != meshes.end())
    {
        if (auto mesh = it->second.lock())
            return mesh;
    }

    auto mesh = std::make_shared<Mesh>(
        meshPath,
        device,
        bufferManager
    );
    meshes[meshPath] = mesh;

    return mesh;
}

std::vector<std::shared_ptr<Material>>
ResourceManager::getMaterialsForMesh(const Mesh& mesh)
{
    std::vector<std::shared_ptr<Material>> result;

    for (const auto& matData : mesh.getMaterials())
    {
        std::string key =
            matData.baseColorPath + "|" +
            matData.normalPath + "|" +
            matData.metallicRoughnessPath;

        std::shared_ptr<Material> material = nullptr;

        auto it = materials.find(key);
        if (it != materials.end())
        {
            material = it->second.lock();
        }

        if (!material)
        {
            std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> baseColorHandle = nullptr;
            std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> normalHandle = nullptr;
            std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> mrHandle = nullptr;

            if (!matData.baseColorPath.empty())
                baseColorHandle = getTexture(matData.baseColorPath);

            if (!matData.normalPath.empty())
                normalHandle = getTexture(matData.normalPath);

            if (!matData.metallicRoughnessPath.empty())
                mrHandle = getTexture(matData.metallicRoughnessPath);

            material = std::make_shared<Material>(
                baseColorHandle,
                normalHandle,
                mrHandle
            );

            materials[key] = material;
        }

        result.push_back(material);
    }

    return result;
}

std::shared_ptr<Material>
ResourceManager::getMaterialForSubMesh(
    const Mesh& mesh,
    const Mesh::SubMesh& subMesh
)
{
    const auto& materialsData = mesh.getMaterials();

    if (subMesh.materialIndex >= materialsData.size())
        throw std::runtime_error("Invalid material index in SubMesh");

    const auto& matData = materialsData[subMesh.materialIndex];

    std::string key =
        matData.baseColorPath + "|" +
        matData.normalPath + "|" +
        matData.metallicRoughnessPath;

    std::shared_ptr<Material> material = nullptr;

    auto it = materials.find(key);
    if (it != materials.end())
        material = it->second.lock();

    if (!material)
    {
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> baseColorHandle = nullptr;
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> normalHandle = nullptr;
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> mrHandle = nullptr;

        if (!matData.baseColorPath.empty())
            baseColorHandle = getTexture(matData.baseColorPath);

        if (!matData.normalPath.empty())
            normalHandle = getTexture(matData.normalPath);

        if (!matData.metallicRoughnessPath.empty())
            mrHandle = getTexture(matData.metallicRoughnessPath);

        material = std::make_shared<Material>(
            baseColorHandle,
            normalHandle,
            mrHandle
        );

        materials[key] = material;
    }

    return material;
}

std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle>
ResourceManager::getTexture(const std::string& path)
{
    auto it = textures.find(path);

    if (it != textures.end())
    {
        if (auto handle = it->second.lock())
            return handle;
    }

    TextureAsset asset(path, physicalDevice);

    auto textureImage = new TextureImage(
        physicalDevice,
        device,
        bufferManager,
        samplerManager.getSampler(asset.getRecommendedSamplerDesc()),
        asset,
        &TextureImage::DefaultImageTransitionPolicy::instance()
    );

    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> handle
        = bindlessRegistry->registerTexture(textureImage);
    textures[path] = handle;

    return handle;
}