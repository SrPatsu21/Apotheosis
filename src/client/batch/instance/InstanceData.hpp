#pragma once

#include <glm/glm.hpp>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

struct InstanceData {
    alignas(16) glm::mat4 model;
    alignas(16) glm::uvec4 materialIndices;
    // x = baseColor
    // y = normal
    // z = metallicRoughness
    // w = reserved

    InstanceData() : 
        model(1.0f),
        materialIndices(0, 0, 0, 0)
    {}

    InstanceData(glm::mat4 model) : model(model){}

    ~InstanceData() = default;

    InstanceData(const InstanceData& other) : model(other.model) {}
    InstanceData& operator=(const InstanceData& other) {
        if (this != &other) {
            model = other.model;
        }
        return *this;
    }

    InstanceData(InstanceData&& other) noexcept : model(std::move(other.model)) {}
    InstanceData& operator=(InstanceData&& other) noexcept {
        if (this != &other) {
            model = std::move(other.model);
        }
        return *this;
    }
};