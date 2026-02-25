#include "Material.hpp"

Material::Material(
    uint32_t baseColorIndex,
    uint32_t normalIndex,
    uint32_t metallicRoughnessIndex
) :
    baseColorIndex(baseColorIndex),
    normalIndex(normalIndex),
    metallicRoughnessIndex(metallicRoughnessIndex)
{}