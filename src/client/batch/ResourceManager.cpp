#include "ResourceManager.hpp"

ResourceManager::ResourceManager(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    BufferManager* bufferManager,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout layout
) :
    physicalDevice(physicalDevice),
    device(device),
    bufferManager(bufferManager),
    descriptorPool(descriptorPool),
    layout(layout)
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

std::shared_ptr<Material> ResourceManager::getMaterial(
    const std::string& texturePath
) {
    auto it = materials.find(texturePath);

    if (it != materials.end())
    {
        if (auto mat = it->second.lock())
            return mat;
    }

    //TODO auto texture = getTexture(texturePath);

    TextureImage::TextureImageDesc textureImageDesc = TextureImage::TextureImageDesc();
    std::shared_ptr<TextureImage> texture = std::make_shared<TextureImage>(
        physicalDevice,
        device,
        texturePath,
        bufferManager,
        textureImageDesc,
        &TextureImage::DefaultImageTransitionPolicy::instance()
    );

    std::shared_ptr<Material> material = std::make_shared<Material>(
        device,
        descriptorPool,
        layout,
        texture
    );

    materials[texturePath] = material;
    return material;
}
