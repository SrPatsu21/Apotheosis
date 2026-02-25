#pragma once

#include <cstdint>

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
    uint32_t baseColorIndex;
    uint32_t normalIndex;
    uint32_t metallicRoughnessIndex;

public:
    Material(
        uint32_t baseColorIndex,
        uint32_t normalIndex,
        uint32_t metallicRoughnessIndex
    );

    uint32_t getBaseColorIndex() const { return baseColorIndex; }
    uint32_t getNormalIndex() const { return normalIndex; }
    uint32_t getMetallicRoughnessIndex() const { return metallicRoughnessIndex; }
};