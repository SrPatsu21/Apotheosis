#pragma once

#include <cstdint>
#include "../texture/BindlessTextureRegistry.hpp"

/**
 * @brief CPU-side representation of a material for bindless rendering.
 *
 * A Material stores indices into global bindless descriptor arrays.
 *
 * These indices are used in the shader to fetch textures.
 */
class Material
{
private:
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> baseColorHandle;
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> normalHandle;
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> metallicRoughnessHandle;

public:
    Material(
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> baseColorHandle,
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> normalHandle,
        std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> metallicRoughnessHandle
    );

    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> getBaseColorHandle() const { return baseColorHandle; }
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> getNnormalHandle() const { return normalHandle; }
    std::shared_ptr<BindlessTextureRegistry::BindlessTextureHandle> getMetallicRoughnessHandle() const { return metallicRoughnessHandle; }
};