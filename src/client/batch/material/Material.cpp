#include "Material.hpp"

Material::Material(
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> baseColorHandle,
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> normalHandle,
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> metallicRoughnessHandle
) :
    baseColorHandle(baseColorHandle),
    normalHandle(normalHandle),
    metallicRoughnessHandle(metallicRoughnessHandle)
{}